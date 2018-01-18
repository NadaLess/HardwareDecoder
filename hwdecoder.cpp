#include "hwdecoder.h"

extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
}

#include <QByteArray>
#include <QtConcurrent/QtConcurrent>
#include <QtAV/VideoFrame.h>
#include <QtAV/VideoRenderer.h>
#include <QtAV/ZeroCopyChecker.h>
#include <QDebug>

struct ScopedAVFrameDeleter
{
    static inline void cleanup(void *pointer) {
        if (pointer)
            av_frame_free((AVFrame**)&pointer);
    }
};

struct ScopedAVPacketDeleter
{
    static inline void cleanup(void *pointer) {
        if (pointer)
            av_packet_unref((AVPacket*)pointer);
    }
};

AVPixelFormat HWDecoder::hw_pix_fmt = AV_PIX_FMT_NONE;

HWDecoder::HWDecoder(QObject * parent): QObject(parent), hw_device_ctx(nullptr), m_zeroCopy(false)
{
    m_zeroCopy = QtAV::ZeroCopyChecker::instance()->enabled();
    av_register_all();
}

HWDecoder::~HWDecoder()
{
    m_processFuture.waitForFinished();
    flush();
    close();
}

int HWDecoder::init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
                                      NULL, NULL, 0)) < 0) {
        qWarning() << "Failed to create specified HW device.";
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    return err;
}

void HWDecoder::close()
{
    sendFrame(QtAV::VideoFrame());
    avcodec_free_context(&decoder_ctx);
    avformat_close_input(&input_ctx);
    av_buffer_unref(&hw_device_ctx);

    decoder_ctx = nullptr;
    input_ctx = nullptr;
    hw_device_ctx = nullptr;
}

void HWDecoder::flush()
{
    if (decoder_ctx) {
        QScopedPointer<AVPacket, ScopedAVPacketDeleter> packet(av_packet_alloc());
        decode(decoder_ctx, packet.data());
    }
}

enum AVPixelFormat HWDecoder::getFormat(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts)
{
    Q_UNUSED(ctx)
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    qWarning() << "Failed to get HW surface format.";
    return AV_PIX_FMT_NONE;
}

int HWDecoder::decode(AVCodecContext *avctx, AVPacket *packet)
{
    QScopedPointer<AVFrame, ScopedAVFrameDeleter> frame;

    int ret = avcodec_send_packet(avctx, packet);
    if (ret < 0) {
        qWarning() << "Error during decoding";
        return ret;
    }

    while (ret >= 0) {
        frame.reset(av_frame_alloc());

        if (!frame.data()) {
            qWarning() << "Can not alloc frame";
            ret = AVERROR(ENOMEM);
            return ret;
        }

        ret = avcodec_receive_frame(avctx, frame.data());
        if (ret == AVERROR(EAGAIN)) {
            return 0;
        } else if (ret < 0) {
            qWarning() << "Error while decoding";
            return ret;
        }

        QtAV::VideoFrame videoFrame;
        if (frame->format == hw_pix_fmt) {
            videoFrame = createHWVideoFrame(frame.data());
        }

        sendFrame(videoFrame);
    }

    return 0;
}

void HWDecoder::processFile(const QString & input)
{
    int video_stream, ret;
    AVStream *video = NULL;
    AVCodec *decoder = NULL;
    AVPacket packet;

    int i;

    m_type = av_hwdevice_find_type_by_name(m_deviceName.toStdString().c_str());
    if (m_type == AV_HWDEVICE_TYPE_NONE) {
        qWarning() << "Device type" << m_deviceName << "is not supported.";
        qWarning() << "Available device types:";
        while((m_type = av_hwdevice_iterate_types(m_type)) != AV_HWDEVICE_TYPE_NONE)
            qWarning() << QString::fromStdString(av_hwdevice_get_type_name(m_type));
        return;
    }

    /* open the input file */
    if (avformat_open_input(&input_ctx, input.toStdString().c_str(), NULL, NULL) != 0) {
        qWarning() << "Cannot open input file" << input;
        return;
    }

    if (avformat_find_stream_info(input_ctx, NULL) < 0) {
        qWarning() << "Cannot find input stream information.";
        return;
    }

    /* find the video stream information */
    ret = av_find_best_stream(input_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (ret < 0) {
        qWarning() << "Cannot find a video stream in the input file";
        return;
    }
    video_stream = ret;

    for (i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
        if (!config) {
            qWarning() << "Decoder" << QString::fromStdString(decoder->name)
                       << "does not support device type"
                       << QString::fromStdString(av_hwdevice_get_type_name(m_type));
            return;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                config->device_type == m_type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }

    if (!(decoder_ctx = avcodec_alloc_context3(decoder))) {
        return;
    }

    video = input_ctx->streams[video_stream];
    if (avcodec_parameters_to_context(decoder_ctx, video->codecpar) < 0)
        return;

    decoder_ctx->get_format  = getFormat;
    av_opt_set_int(decoder_ctx, "refcounted_frames", 1, 0);

    if (init(decoder_ctx, m_type) < 0)
        return;

    if ((ret = avcodec_open2(decoder_ctx, decoder, NULL)) < 0) {
        qWarning() << "Failed to open codec for stream" << video_stream;
        return;
    }

    //Decoding loop
    while (ret >= 0) {
        if ((ret = av_read_frame(input_ctx, &packet)) < 0)
            break;

        if (video_stream == packet.stream_index)
            ret = decode(decoder_ctx, &packet);

        av_packet_unref(&packet);
    }

    flush();
    close();
}

void HWDecoder::decodeVideo(const QUrl &input)
{
    if (!m_processFuture.isRunning()) {
        //Call processFile in another thread
        QtConcurrent::run(this, &HWDecoder::processFile, input.toLocalFile());
    }
}

QObject* HWDecoder::getPlayer() const
{
    return (QObject*)&m_player;
}

void HWDecoder::sendFrame(const QtAV::VideoFrame &frame)
{
    Q_EMIT frameDecoded(frame);

    //Send to local render
    for(QtAV::VideoRenderer* output : m_player.videoOutputs()) {
        if (!output || !output->isAvailable()) continue;
        output->receive(frame);
    }
}

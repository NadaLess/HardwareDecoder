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

AVPixelFormat HWDecoder::m_hwPixFmt = AV_PIX_FMT_NONE;

HWDecoder::HWDecoder(QObject * parent)
    : QObject(parent), m_type(AV_HWDEVICE_TYPE_NONE),
      m_hwDeviceCtx(nullptr), m_decoder(nullptr),
      m_inputCtx(nullptr), m_decoderCtx(nullptr), m_zeroCopy(false)
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

bool HWDecoder::init(AVCodecParameters* codecParameters)
{
    m_type = av_hwdevice_find_type_by_name(m_deviceName.toStdString().c_str());
    if (m_type == AV_HWDEVICE_TYPE_NONE) {
        qWarning() << "Device type" << m_deviceName << "is not supported.";
        qWarning() << "Available device types:";
        while((m_type = av_hwdevice_iterate_types(m_type)) != AV_HWDEVICE_TYPE_NONE)
            qWarning() << QString::fromStdString(av_hwdevice_get_type_name(m_type));
        return false;
    }

    for (int i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(m_decoder, i);
        if (!config) {
            qWarning() << "Decoder" << QString::fromStdString(m_decoder->name)
                       << "does not support device type"
                       << QString::fromStdString(av_hwdevice_get_type_name(m_type));
            return false;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                config->device_type == m_type) {
            m_hwPixFmt = config->pix_fmt;
            break;
        }
    }

    if (!(m_decoderCtx = avcodec_alloc_context3(m_decoder))) {
        return false;
    }

    if (avcodec_parameters_to_context(m_decoderCtx, codecParameters) < 0)
        return false;

    m_decoderCtx->get_format  = getFormat;
    av_opt_set_int(m_decoderCtx, "refcounted_frames", 1, 0);

    if (initHWContext(m_decoderCtx, m_type) < 0)
        return false;

    return true;
}

int HWDecoder::initHWContext(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&m_hwDeviceCtx, type,
                                      NULL, NULL, 0)) < 0) {
        qWarning() << "Failed to create specified HW device.";
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(m_hwDeviceCtx);

    return err;
}

bool HWDecoder::open()
{
    if (m_decoder && m_decoderCtx
            && avcodec_open2(m_decoderCtx, m_decoder, NULL) == 0) {
        return true;
    }

    qWarning() << "Failed to open codec";
    return false;
}


void HWDecoder::close()
{
    sendFrame(QtAV::VideoFrame());
    avcodec_free_context(&m_decoderCtx);
    avformat_close_input(&m_inputCtx);
    av_buffer_unref(&m_hwDeviceCtx);

    m_decoderCtx = nullptr;
    m_inputCtx = nullptr;
    m_hwDeviceCtx = nullptr;
}

void HWDecoder::flush()
{
    if (m_decoderCtx) {
        QScopedPointer<AVPacket, ScopedAVPacketDeleter> packet(av_packet_alloc());
        decode(m_decoderCtx, packet.data());
    }
}

enum AVPixelFormat HWDecoder::getFormat(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts)
{
    Q_UNUSED(ctx)
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == m_hwPixFmt)
            return *p;
    }

    qWarning() << "Failed to get HW surface format.";
    return AV_PIX_FMT_NONE;
}

int HWDecoder::decode(AVCodecContext *avctx, AVPacket *packet)
{

    int ret = avcodec_send_packet(avctx, packet);
    if (ret < 0) {
        qWarning() << "Error during decoding";
        return ret;
    }

    QScopedPointer<AVFrame, ScopedAVFrameDeleter> frame;
    while (ret >= 0) {
        frame.reset(av_frame_alloc());

        if (!frame.data()) {
            qWarning() << "Can not alloc frame to get decoded data!";
            return AVERROR(ENOMEM);
        }

        ret = avcodec_receive_frame(avctx, frame.data());
        if (ret == AVERROR(EAGAIN)) {
            return 0;
        } else if (ret < 0) {
            qWarning() << "Error while decoding";
            return ret;
        }

        QtAV::VideoFrame videoFrame;
        if (frame->format == m_hwPixFmt) {
            videoFrame = createHWVideoFrame(frame.data());
        } else {
            qWarning() << "Sw decoded frame, not implemented";
        }

        sendFrame(videoFrame);
    }

    return 0;
}

void HWDecoder::processFile(const QString & input)
{
    int video_stream, ret;
    AVPacket packet;

    /* open the input file */
    if (avformat_open_input(&m_inputCtx, input.toStdString().c_str(), NULL, NULL) != 0) {
        qWarning() << "Cannot open input file" << input;
        return;
    }

    if (avformat_find_stream_info(m_inputCtx, NULL) < 0) {
        qWarning() << "Cannot find input stream information.";
        return;
    }

    /* find the video stream information */
    ret = av_find_best_stream(m_inputCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &m_decoder, 0);
    if (ret < 0) {
        qWarning() << "Cannot find a video stream in the input file";
        return;
    }
    video_stream = ret;
    AVCodecParameters* codecParameters = m_inputCtx->streams[video_stream]->codecpar;

    if (!init(codecParameters)) {
        return;
    }

    if (!open()) {
        return;
    }

    //Decoding loop
    while (ret >= 0) {
        if ((ret = av_read_frame(m_inputCtx, &packet)) < 0)
            break;

        if (video_stream == packet.stream_index)
            ret = decode(m_decoderCtx, &packet);

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
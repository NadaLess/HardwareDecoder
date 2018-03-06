#include "hwdecoder.h"
#include <QDebug>

AVPixelFormat HWDecoder::m_hwPixFmt = AV_PIX_FMT_NONE;

HWDecoder::HWDecoder(QObject * parent)
    : QObject(parent),
      m_type(AV_HWDEVICE_TYPE_NONE),
      m_hwDeviceCtx(nullptr),
      m_decoder(nullptr),
      m_decoderCtx(nullptr)
{
    av_register_all();
}

HWDecoder::~HWDecoder()
{
    flush();
    close();
}

bool HWDecoder::init(AVCodecParameters* codecParameters)
{
    if (!codecParameters) return false;

    m_decoder = avcodec_find_decoder(codecParameters->codec_id);
    if (!m_decoder) return false;

    m_type = av_hwdevice_find_type_by_name(m_deviceName.toStdString().c_str());
    if (m_type == AV_HWDEVICE_TYPE_NONE) {
        qWarning() << "Device type" << m_deviceName << "is not supported.";
        qWarning() << "Available device types:";
        while((m_type = av_hwdevice_iterate_types(m_type)) != AV_HWDEVICE_TYPE_NONE)
            qWarning() << QString::fromStdString(av_hwdevice_get_type_name(m_type));
        return false;
    }

    if (!(m_decoderCtx = avcodec_alloc_context3(m_decoder))) {
        return false;
    }

    if (avcodec_parameters_to_context(m_decoderCtx, codecParameters) < 0)
        return false;

    m_decoderCtx->get_format  = getFormat;
    m_decoderCtx->refcounted_frames = 1;

    if (initHWContext(m_type) < 0)
        return false;

    return true;
}

int HWDecoder::initHWContext(const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&m_hwDeviceCtx, type,
                                      NULL, NULL, 0)) < 0) {
        qWarning() << "Failed to create specified HW device.";
        return err;
    }
    m_decoderCtx->hw_device_ctx = av_buffer_ref(m_hwDeviceCtx);

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
    avcodec_free_context(&m_decoderCtx);
    av_buffer_unref(&m_hwDeviceCtx);

    m_decoderCtx = nullptr;
    m_hwDeviceCtx = nullptr;
}

void HWDecoder::flush()
{
    if (m_decoderCtx) {
        QScopedPointer<AVPacket, AVPacketDeleter> packet(av_packet_alloc());
        decode(packet.data());
    }
}

enum AVPixelFormat HWDecoder::getFormat(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts)
{
    Q_UNUSED(ctx)
    const enum AVPixelFormat *p;

    //nadaless: Try to decode baseline profiles with HW (Android WebRTC Streams)
    if (ctx->profile == FF_PROFILE_H264_BASELINE)
        ctx->hwaccel_flags |= AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == m_hwPixFmt)
            return *p;
    }

    qWarning() << "Failed to get HW surface format.";
    return AV_PIX_FMT_NONE;
}

int HWDecoder::decode(AVPacket *packet)
{

    int ret = avcodec_send_packet(m_decoderCtx, packet);
    if (ret < 0) {
        qWarning() << "Error during decoding";
        return ret;
    }

    QScopedPointer<AVFrame, AVFrameDeleter> frame;
    while (ret >= 0) {
        frame.reset(av_frame_alloc());

        if (!frame.data()) {
            qWarning() << "Can not alloc frame to get decoded data!";
            return AVERROR(ENOMEM);
        }

        ret = avcodec_receive_frame(m_decoderCtx, frame.data());
        if (ret == AVERROR(EAGAIN)) {
            return 0;
        } else if (ret < 0) {
            switch(ret) {
                case AVERROR_EOF:                    
                    sendFrame(new VideoFrame());
                    break;
                default:
                    qWarning() << "Error while decoding, code:" << ret;
                    break;
            }
            return ret;
        }

        VideoFrame* videoFrame;
        if (frame->format == m_hwPixFmt) {
            videoFrame = createHWVideoFrame(frame.data());
        } else {
            videoFrame = createSWVideoFrame(frame.data());
        }

        sendFrame(videoFrame);
    }

    return 0;
}

VideoFrame* HWDecoder::createSWVideoFrame(const AVFrame *frame)
{
    Q_UNUSED(frame)
    Q_UNIMPLEMENTED();
    return new VideoFrame();
}

void HWDecoder::sendFrame(VideoFrame *frame)
{
    VideoFramePtr sharedFrame(frame);
    Q_EMIT frameDecoded(sharedFrame);
}

#ifndef HWDECODER_H
#define HWDECODER_H

#include "videoframe.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
#include <libavformat/avformat.h>
}

class HWDecoder: public QObject
{
    Q_OBJECT
public:
    static enum AVPixelFormat getFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    HWDecoder(QObject * parent = nullptr);
    ~HWDecoder();

    bool init(AVCodecParameters* codecParameters);
    bool open();
    void close();
    void flush();

    int decode(AVPacket *packet);

Q_SIGNALS:
    void frameDecoded(VideoFramePtr frame);

protected:
    QString m_deviceName;
    static AVPixelFormat m_hwPixFmt;

private:
    int initHWContext(const enum AVHWDeviceType m_type);
    void sendFrame(VideoFrame * frame);

    virtual VideoFrame* createHWVideoFrame(const AVFrame * frame) = 0;
    VideoFrame* createSWVideoFrame(const AVFrame *frame);

    AVCodecContext *m_decoderCtx;
    AVHWDeviceType m_type;
    AVBufferRef *m_hwDeviceCtx;

    AVCodec *m_decoder;    
};

struct AVFrameDeleter
{
    static inline void cleanup(void *pointer) {
        if (pointer)
            av_frame_free((AVFrame**)&pointer);
    }
};

struct AVPacketDeleter
{
    static inline void cleanup(void *pointer) {
        if (pointer)
            av_packet_unref((AVPacket*)pointer);
    }
};

struct AVFormatContextDeleter
{
    static inline void cleanup(void *pointer) {
        if (pointer)
            avformat_close_input((AVFormatContext**)&pointer);
    }
};

#endif // HWDECODER_H

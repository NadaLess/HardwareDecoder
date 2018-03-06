#ifndef HWDECODER_H
#define HWDECODER_H

#include <QObject>
#include <QFile>
#include <QFuture>

#include "videosource.h"
#include "videoframe.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
#include <libavformat/avformat.h>
}

class HWDecoder: public QObject
{
    Q_OBJECT
    Q_PROPERTY(VideoSource* source READ getSource WRITE setSource NOTIFY sourceChanged)
public:
    static enum AVPixelFormat getFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    HWDecoder(QObject * parent = nullptr);
    ~HWDecoder();

    bool init(AVCodecParameters* codecParameters);
    bool open();
    void close();
    void flush();

    Q_INVOKABLE void decodeVideo(const QUrl & input);

    VideoSource *getSource() const;
    void setSource(VideoSource *source);

    static QString kSurfaceInteropKey;

Q_SIGNALS:
    void sourceChanged();
    void frameDecoded(VideoFramePtr frame);

protected:
    QString m_deviceName;
    static AVPixelFormat m_hwPixFmt;

private:
    int initHWContext(AVCodecContext *ctx, const enum AVHWDeviceType m_type);
    int decode(AVCodecContext *avctx, AVPacket *packet);
    void processStream(const QIODevice * buffer);
    void processFile(const QString & input);
    void sendFrame(VideoFrame * frame);

    virtual VideoFrame* createHWVideoFrame(const AVFrame * frame) = 0;
    VideoFrame* createSWVideoFrame(const AVFrame *frame);

    AVCodecContext *m_decoderCtx;
    AVHWDeviceType m_type;
    AVBufferRef *m_hwDeviceCtx;

    AVCodec *m_decoder;
    AVFormatContext *m_inputCtx;

    VideoSource* m_source;
    QFuture<void> m_processFuture;

};

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

#endif // HWDECODER_H

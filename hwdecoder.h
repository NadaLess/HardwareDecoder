#ifndef HWDECODER_H
#define HWDECODER_H

#include <QObject>
#include <QFile>
#include <QFuture>

#include <QtAV/AVPlayer.h>
#include <QtAV/SurfaceInterop.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class HWDecoder: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* source READ getPlayer NOTIFY playerChanged)
public:
    static enum AVPixelFormat getFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    HWDecoder(QObject * parent = nullptr);
    ~HWDecoder();

    int init(AVCodecContext *ctx, const enum AVHWDeviceType m_type);
    void close();
    void flush();

    Q_INVOKABLE void decodeVideo(const QUrl & input);

    QObject* getPlayer() const;

Q_SIGNALS:
    void playerChanged();
    void frameDecoded(const QtAV::VideoFrame & frame);

protected:
    QString m_deviceName;
    bool m_zeroCopy;
    QtAV::VideoSurfaceInteropPtr m_surfaceInterop;

private:
    int decode(AVCodecContext *avctx, AVPacket *packet);
    void processStream(const QIODevice * buffer);
    void processFile(const QString & input);
    void sendFrame(const QtAV::VideoFrame & frame);

    virtual QtAV::VideoFrame createHWVideoFrame(const AVFrame * frame) = 0;

    AVHWDeviceType m_type;
    AVBufferRef *hw_device_ctx;
    static AVPixelFormat hw_pix_fmt;

    AVFormatContext *input_ctx = NULL;
    AVCodecContext *decoder_ctx = NULL;

    QtAV::AVPlayer m_player;
    QFuture<void> m_processFuture;

};


#endif // HWDECODER_H

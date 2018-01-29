#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QQuickFramebufferObject>
#include "videosource.h"

class VideoFBORenderer : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(VideoSource* source READ source WRITE setSource NOTIFY sourceChanged)
public:
    VideoFBORenderer(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const;

    void setSource(VideoSource * source);
    VideoSource *source() const;

    VideoFrame frame() const;

Q_SIGNALS:
    void sourceChanged();

private Q_SLOTS:
    void onFrameReady(const VideoFrame & frame);

private:
    VideoSource * m_source;
    VideoFrame m_frame;
};

#endif // VIDEORENDERER_H

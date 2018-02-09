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

    VideoFramePtr frame() const;

Q_SIGNALS:
    void sourceChanged();

private Q_SLOTS:
    void onFrameReady(VideoFramePtr frame);

private:
    VideoSource * m_source;
    VideoFramePtr m_frame;
};

#endif // VIDEORENDERER_H

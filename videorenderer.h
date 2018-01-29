#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QQuickFramebufferObject>

class VideoSource;
class VideoFBORenderer : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    Renderer *createRenderer() const;
    void setSource(VideoSource * vs);

private:
    VideoSource* m_videoSource;
};

#endif // VIDEORENDERER_H

#include "videorenderer.h"
#include "framerenderer.h"

#include <QOpenGLFramebufferObject>

class VideoRenderer : public QQuickFramebufferObject::Renderer
{
public:
    VideoRenderer()
    {
        frame.initialize();
    }

    void render() {
        frame.render();
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    FrameRenderer frame;
};

QQuickFramebufferObject::Renderer *VideoFBORenderer::createRenderer() const
{
    return new VideoRenderer();
}


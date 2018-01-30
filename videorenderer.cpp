#include "videorenderer.h"
#include "framerenderer.h"
#include "videosource.h"

#include <QOpenGLFramebufferObject>

class VideoRenderer : public QQuickFramebufferObject::Renderer
{
public:
    VideoRenderer()
    {
        m_frameRenderer.initialize();
    }

    void render() {
        m_frameRenderer.render();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

private:
    FrameRenderer m_frameRenderer;

    // Renderer interface
protected:
    virtual void synchronize(QQuickFramebufferObject * renderer) override {
        VideoFBORenderer* fboRenderer = (VideoFBORenderer*)renderer;
        if (fboRenderer)
            m_frameRenderer.setFrame(fboRenderer->frame());
    }
};

VideoFBORenderer::VideoFBORenderer(QQuickItem *parent)
    : QQuickFramebufferObject(parent), m_source(nullptr)
{

}

QQuickFramebufferObject::Renderer *VideoFBORenderer::createRenderer() const
{
    return new VideoRenderer();
}

void VideoFBORenderer::setSource(VideoSource *source)
{
    if (m_source != source){

        if (m_source)
            disconnect(m_source);

        m_source = source;

        if (m_source)
            connect(m_source, &VideoSource::frameReady, this, &VideoFBORenderer::onFrameReady);

        Q_EMIT sourceChanged();
    }
}

VideoSource *VideoFBORenderer::source() const
{
    return m_source;
}

void VideoFBORenderer::onFrameReady(VideoFramePtr frame)
{
    m_frame = frame;
    update();
}

VideoFramePtr VideoFBORenderer::frame() const
{
    return m_frame;
}

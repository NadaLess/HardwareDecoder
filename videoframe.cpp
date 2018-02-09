#include "videoframe.h"

VideoFrame::VideoFrame(QObject *parent): QObject(parent), m_surface(nullptr)
{
    registerMetaType();
}

VideoFrame::VideoFrame(Surface * surface, QObject * parent) : VideoFrame(parent)
{
    m_surface = surface;
}

VideoFrame::~VideoFrame()
{
    if (m_surface)
        delete m_surface;
}

void VideoFrame::registerMetaType()
{
    static bool registered = false;
    if (!registered) {
        qRegisterMetaType<VideoFramePtr>("VideoFramePtr");
        registered = true;
    }
}

bool VideoFrame::map(GLuint name)
{
    if (!m_surface) return false;
    return m_surface->map(name);
}

void VideoFrame::unmap()
{
    if (!m_surface) return;
    m_surface->unmap();
}

int VideoFrame::width()
{
    if (!m_surface) return 0;
    return m_surface->width();
}

int VideoFrame::height()
{
    if (!m_surface) return 0;
    return m_surface->height();
}

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

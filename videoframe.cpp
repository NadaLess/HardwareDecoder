#include "videoframe.h"

VideoFrame::VideoFrame(Surface * surface) : m_surface(surface)
{

}

VideoFrame::~VideoFrame()
{
    if (m_surface)
        delete m_surface;
}

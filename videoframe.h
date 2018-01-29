#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include "surface.h"

class VideoFrame
{
public:
    VideoFrame(Surface * surface = nullptr);
    ~VideoFrame();

private:
    Surface * m_surface;
};

#endif // VIDEOFRAME_H

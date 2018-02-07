#include "framerenderer.h"

#if defined(Q_OS_WIN)
#include "d3d9renderer.h"
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include "x11renderer.h"
#endif

FrameRenderer* FrameRenderer::create() {
#if defined(Q_OS_WIN)
    return new D3D9Renderer();
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    return new X11Renderer();
#endif
}

void FrameRenderer::setFrame(VideoFramePtr frame)
{
    m_frame = frame;
}

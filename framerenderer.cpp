#include "framerenderer.h"

FrameRenderer::FrameRenderer()
{

}

void FrameRenderer::render()
{
    glDepthMask(true);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    map();
    renderFrame();
    unmap();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void FrameRenderer::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(0.f, 0.f, 0.f, 1.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void FrameRenderer::map()
{

}

void FrameRenderer::unmap()
{

}

void FrameRenderer::renderFrame()
{
}

void FrameRenderer::setFrame(const VideoFrame &frame)
{
    m_frame = frame;
}

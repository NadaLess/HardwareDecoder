#include "framerenderer.h"

FrameRenderer::FrameRenderer()
{

}

void FrameRenderer::render()
{
    glDepthMask(true);

//    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    glClearColor(r, g, b, 1.0f);

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

void FrameRenderer::setFrame(VideoFramePtr frame)
{
    m_frame = frame;
}

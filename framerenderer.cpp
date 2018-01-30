#include "framerenderer.h"

FrameRenderer::FrameRenderer()
{
}

void FrameRenderer::render()
{
    if (m_frame.isNull()) return;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

//    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    glClearColor(r, g, b, 1.0f);

    GLuint name;
    glGenTextures(1, &name);

    if (!m_frame->map(name)) return;
    glBindTexture(GL_TEXTURE_2D, name);

    renderFrame();

    glBindTexture(GL_TEXTURE_2D, 0);
    m_frame->unmap();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void FrameRenderer::initialize()
{
    initializeOpenGLFunctions();

    glDepthMask(true);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
}

void FrameRenderer::renderFrame()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    //
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_frame->width(), m_frame->height(),  GL_BGRA, GL_UNSIGNED_BYTE, 0/*m_frame.constBits(p)*/);
}

void FrameRenderer::setFrame(VideoFramePtr frame)
{
    m_frame = frame;
}

#ifndef FRAMERENDERER_H
#define FRAMERENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QTransform>

#include "videoframe.h"

class FrameRenderer: public QOpenGLFunctions
{
public:
    FrameRenderer();
    void render();
    void initialize();

    void setFrame(const VideoFrame &frame);

private:
    virtual void map();
    virtual void unmap();
    virtual void renderFrame();

    QTransform m_transformation;
    VideoFrame m_frame;
};

#endif // FRAMERENDERER_H

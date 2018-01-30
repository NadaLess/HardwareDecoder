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

    void setFrame(VideoFramePtr frame);

private:
    virtual void renderFrame();

    QTransform m_transformation;
    VideoFramePtr m_frame;
};

#endif // FRAMERENDERER_H

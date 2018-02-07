#ifndef FRAMERENDERER_H
#define FRAMERENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QTransform>

#include "videoframe.h"

class FrameRenderer: public QOpenGLFunctions
{
public:
    static FrameRenderer* create();

    virtual void render() = 0;
    virtual void initialize() = 0;
    void setFrame(VideoFramePtr frame);

private:
    VideoFramePtr m_frame;
};

#endif // FRAMERENDERER_H

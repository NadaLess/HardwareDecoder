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
    ~FrameRenderer();

    void render();
    void initialize();

    void setFrame(VideoFramePtr frame);

private:
    virtual void renderFrame();
    void _allocProgram();
    void _deallocProgram();
    void _initGeometries();

    QTransform m_transformation;
    VideoFramePtr m_frame;

    QOpenGLShaderProgram * m_program;

    QVector<QVector3D> in_pos;
    QVector<QVector3D> in_tc;
};

#endif // FRAMERENDERER_H

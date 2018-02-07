#ifndef D3D9RENDERER_H
#define D3D9RENDERER_H

#include "framerenderer.h"

class D3D9Renderer: public FrameRenderer
{
public:
    D3D9Renderer();
    ~D3D9Renderer();

    // FrameRenderer interface
public:
    void render();
    void initialize();

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

#endif // D3D9RENDERER_H

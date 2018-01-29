#ifndef FRAMERENDERER_H
#define FRAMERENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QTransform>

class FrameRenderer: public QOpenGLFunctions
{
public:
    FrameRenderer();
    void render();
    void initialize();

private:
    virtual void map();
    virtual void unmap();
    virtual void renderFrame();

    QTransform m_transformation;
};

#endif // FRAMERENDERER_H

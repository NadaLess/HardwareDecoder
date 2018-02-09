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

    void initialize();
    void render();
    void setFrame(VideoFramePtr frame);

private:
    void initShaderProgram();
    void initGeometry();

    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    QOpenGLShaderProgram m_shaderProgram;
    int m_in_pos;
    int m_in_tc;

    VideoFramePtr m_frame;
    GLuint m_texture;
};

#endif // FRAMERENDERER_H

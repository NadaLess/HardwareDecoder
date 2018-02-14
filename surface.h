#ifndef SURFACE_H
#define SURFACE_H

#include <QObject>
#include <QOpenGLFunctions>

class Surface: public QObject
{
    Q_OBJECT
public:
    Surface(int width, int height, QObject * parent = nullptr);
    virtual ~Surface();

    virtual bool map(GLuint name);
    virtual bool unmap();

    virtual int width();
    virtual int height();

protected:
    void initGLFunctions();

    int m_width;
    int m_height;
    QOpenGLFunctions m_glFunctions;
    QByteArray m_rgbData;
};

#endif // SURFACE_H

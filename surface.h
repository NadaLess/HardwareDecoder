#ifndef SURFACE_H
#define SURFACE_H

#include <QObject>
#include <QtGlobal>
#include <QtOpenGL>
#include <QtOpenGL/QGL>

class Surface: public QObject
{
    Q_OBJECT
public:
    Surface(QObject * parent = nullptr) : QObject(parent) {}
    virtual ~Surface() {}

    virtual bool map(GLuint name) = 0;
    virtual void unmap() = 0;

    virtual UINT width() = 0;
    virtual UINT height() = 0;
};

#endif // SURFACE_H

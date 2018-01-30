#ifndef SURFACE_H
#define SURFACE_H

#include <QObject>
class Surface: public QObject
{
    Q_OBJECT
public:
    Surface(QObject * parent = nullptr);
};

#endif // SURFACE_H

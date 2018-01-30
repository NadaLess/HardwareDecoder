#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include <QObject>
#include <QSharedPointer>
#include "surface.h"

class VideoFrame: public QObject
{
    Q_OBJECT
public:
    VideoFrame(QObject * parent = nullptr);
    VideoFrame(Surface * surface, QObject * parent = nullptr);
    ~VideoFrame();

    bool map(GLuint name);
    void unmap();

    UINT width();
    UINT height();

private:
    Surface * m_surface;

    void registerMetaType();
};

typedef QSharedPointer<VideoFrame> VideoFramePtr;
Q_DECLARE_METATYPE(VideoFramePtr)

#endif // VIDEOFRAME_H

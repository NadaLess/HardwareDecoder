#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <QObject>
#include "videoframe.h"

class VideoSource: public QObject
{
    Q_OBJECT
public:
    VideoSource(QObject * parent = nullptr);

public Q_SLOTS:
    void setFrame(const VideoFrame & frame);

Q_SIGNALS:
    void frameReady(const VideoFrame & frame);

};

#endif // VIDEOSOURCE_H

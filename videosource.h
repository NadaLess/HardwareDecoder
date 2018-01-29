#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "videorenderer.h"

class VideoSource: QObject
{
    Q_OBJECT
public:
    VideoSource(QObject * parent = nullptr);

public Q_SLOTS:
    void setFrame();

Q_SIGNALS:
    void frameReady();

};

#endif // VIDEOSOURCE_H

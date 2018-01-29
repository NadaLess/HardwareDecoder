#include "videosource.h"

VideoSource::VideoSource(QObject * parent) : QObject(parent)
{
}

void VideoSource::setFrame()
{
    Q_EMIT frameReady();
}

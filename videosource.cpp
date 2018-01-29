#include "videosource.h"

VideoSource::VideoSource(QObject * parent) : QObject(parent)
{
}

void VideoSource::setFrame(const VideoFrame & frame)
{
    Q_EMIT frameReady(frame);
}

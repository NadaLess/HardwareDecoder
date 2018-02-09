#include "videosource.h"

VideoSource::VideoSource(QObject * parent) : QObject(parent)
{
}

void VideoSource::setFrame(VideoFramePtr frame)
{
    Q_EMIT frameReady(frame);
}

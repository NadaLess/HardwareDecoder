#ifndef HWWINDOWSDECODER_H
#define HWWINDOWSDECODER_H

#include "hwdecoder.h"

class HWWindowsDecoder: public HWDecoder
{
    Q_OBJECT
public:
    HWWindowsDecoder(QObject * parent = nullptr);

    // HWDecoder interface
private:
    virtual VideoFrame createHWVideoFrame(const AVFrame *frame) override;
};

#endif // HWWINDOWSDECODER_H

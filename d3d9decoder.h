#ifndef HWWINDOWSDECODER_H
#define HWWINDOWSDECODER_H

#include "hwdecoder.h"

class D3D9Decoder: public HWDecoder
{
public:
    D3D9Decoder(QObject * parent = nullptr);

    // HWDecoder interface
private:
    virtual VideoFrame* createHWVideoFrame(const AVFrame *frame) override;
};

#endif // HWWINDOWSDECODER_H

#ifndef VAAPIDECODER_H
#define VAAPIDECODER_H

#include "hwdecoder.h"

class VAAPIDecoder: public HWDecoder
{
public:    
    VAAPIDecoder(QObject * parent = nullptr);

    // HWDecoder interface
private:
    virtual VideoFrame* createHWVideoFrame(const AVFrame *frame) override;
};

#endif // VAAPIDECODER_H

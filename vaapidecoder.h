#ifndef VAAPIDECODER_H
#define VAAPIDECODER_H

#include "hwdecoder.h"
#include <QtAV/vaapi/SurfaceInteropVAAPI.h>

class VAAPIDecoder: public HWDecoder
{
public:    
    VAAPIDecoder(QObject * parent = nullptr);

    // HWDecoder interface
private:
    virtual QtAV::VideoFrame createHWVideoFrame(const AVFrame *frame) override;

    QtAV::vaapi::X11InteropResource m_x11Interop;
    QtAV::vaapi::InteropResourcePtr m_interopPtr;
    QtAV::vaapi::display_ptr m_displayPtr;
    QtAV::vaapi::surface_ptr m_surfacePtr;
};

#endif // VAAPIDECODER_H

#include "vaapidecoder.h"

extern "C" {
#include "libavutil/hwcontext_vaapi.h"
}

VAAPIDecoder::VAAPIDecoder(QObject * parent) : HWDecoder(parent)
{
    m_deviceName = "vaapi";
    m_interopPtr = QtAV::vaapi::InteropResourcePtr(&m_x11Interop);
}

QtAV::VideoFrame VAAPIDecoder::createHWVideoFrame(const AVFrame *frame)
{
    VASurfaceID vaSurface = (VASurfaceID)(uintptr_t)frame->data[3];
    if (vaSurface == VA_INVALID_SURFACE) return QtAV::VideoFrame();

    //Get VAAPI Device Context
    AVHWFramesContext* hwframesCtx = (AVHWFramesContext*)frame->hw_frames_ctx->data;
    AVVAAPIDeviceContext* vaapiDeviceContext = (AVVAAPIDeviceContext*)hwframesCtx->device_ctx->hwctx;
    VADisplay display = vaapiDeviceContext->display;

    //Create NativeDisplay for VADisplay
    if (m_displayPtr.isNull() || m_displayPtr->get() != display) {
        QtAV::vaapi::NativeDisplay nativeDisplay;
        nativeDisplay.type = QtAV::vaapi::NativeDisplay::VA;
        nativeDisplay.handle = (intptr_t)display;
        m_displayPtr = QtAV::vaapi::display_t::create(nativeDisplay);
    }

    m_surfaceInterop = QtAV::VideoSurfaceInteropPtr(new QtAV::vaapi::SurfaceInteropVAAPI(m_interopPtr));
    m_surfacePtr = QtAV::vaapi::surface_ptr(new QtAV::vaapi::surface_t(frame->width,frame->height, vaSurface, m_displayPtr, false));

    ((QtAV::vaapi::SurfaceInteropVAAPI *)m_surfaceInterop.data())->setSurface(m_surfacePtr,frame->width,frame->height);

    //creating frame to add the surface interop
    QtAV::VideoFrame videoFrame(frame->width,
                                frame->height,
                                QtAV::VideoFormat::Format_RGBA32);

    videoFrame.setBytesPerLine(frame->width * 4); //RGBA = 4 bits per pixel
    videoFrame.setMetaData(kSurfaceInteropKey, QVariant::fromValue(m_surfaceInterop));

    return videoFrame;
}

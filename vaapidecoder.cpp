#include "vaapidecoder.h"
#include "surfacevaapi.h"

extern "C" {
#include "libavutil/hwcontext_vaapi.h"
}

VAAPIDecoder::VAAPIDecoder(QObject * parent) : HWDecoder(parent)
{
    m_deviceName = "vaapi";
}

VideoFrame* VAAPIDecoder::createHWVideoFrame(const AVFrame *frame)
{
    //Get VAAPI Display from FFMpeg hw frame context
    AVHWFramesContext* hwframesCtx = (AVHWFramesContext*)frame->hw_frames_ctx->data;
    AVVAAPIDeviceContext* vaapiDeviceContext = (AVVAAPIDeviceContext*)hwframesCtx->device_ctx->hwctx;
    VADisplay display = vaapiDeviceContext->display;

    VASurfaceID surface = (VASurfaceID)(uintptr_t)frame->data[3];

    SurfaceVAAPI* vaapiSurface = new SurfaceVAAPI(display, surface, frame->width, frame->height);

    return new VideoFrame(vaapiSurface);
}

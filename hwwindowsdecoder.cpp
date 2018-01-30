#include "hwwindowsdecoder.h"

#include "surfaced3d9.h"


HWWindowsDecoder::HWWindowsDecoder(QObject * parent) : HWDecoder(parent)
{
    m_deviceName = "dxva2";
}

VideoFrame* HWWindowsDecoder::createHWVideoFrame(const AVFrame *frame)
{
    IDirect3DSurface9 *d3d9surface = (IDirect3DSurface9*)frame->data[3];
    SurfaceD3D9* videoSurface = new SurfaceD3D9(d3d9surface);
    return new VideoFrame(videoSurface);
}

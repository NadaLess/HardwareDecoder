#include "d3d9decoder.h"
#include "surfaced3d9.h"


D3D9Decoder::D3D9Decoder(QObject * parent) : HWDecoder(parent)
{
    m_deviceName = "dxva2";
    m_hwPixFmt = AV_PIX_FMT_DXVA2_VLD;
}

VideoFrame* D3D9Decoder::createHWVideoFrame(const AVFrame *frame)
{
    IDirect3DSurface9 *d3d9surface = (IDirect3DSurface9*)frame->data[3];
    SurfaceD3D9* videoSurface = new SurfaceD3D9(d3d9surface, frame->width, frame->height);
    return new VideoFrame(videoSurface);
}

#include "hwwindowsdecoder.h"

#include "surfaced3d9.h"


HWWindowsDecoder::HWWindowsDecoder(QObject * parent) : HWDecoder(parent)
{
    m_deviceName = "dxva2";
}

VideoFrame HWWindowsDecoder::createHWVideoFrame(const AVFrame *frame)
{
    IDirect3DSurface9 *surface = (IDirect3DSurface9*)frame->data[3];
    return VideoFrame(new SurfaceD3D9(surface));
}

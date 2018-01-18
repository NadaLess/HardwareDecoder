#include "hwwindowsdecoder.h"

#include <QtAV/directx/SurfaceInteropD3D9.h>


HWWindowsDecoder::HWWindowsDecoder(QObject * parent) : HWDecoder(parent)
{
    m_deviceName = "dxva2";
}

QtAV::VideoFrame HWWindowsDecoder::createHWVideoFrame(const AVFrame *frame)
{
    IDirect3DSurface9 *surface = (IDirect3DSurface9*)frame->data[3];
    if (!surface) return QtAV::VideoFrame();

    IDirect3DDevice9 *d3ddev;
    HRESULT hr = surface->GetDevice(&d3ddev);
    if (!SUCCEEDED(hr)) return QtAV::VideoFrame();

    QtAV::d3d9::InteropResourcePtr interopResource(QtAV::d3d9::InteropResource::create(d3ddev));
    m_surfaceInterop.reset(new QtAV::d3d9::SurfaceInterop(interopResource));
    d3ddev->Release();

    ((QtAV::d3d9::SurfaceInterop *)m_surfaceInterop.data())->setSurface(surface, frame->width, frame->height);

    QtAV::VideoFrame videoFrame(frame->width,
                                frame->height,
                                QtAV::VideoFormat::Format_RGBA32);
    videoFrame.setBytesPerLine(frame->linesize[0] * 4); //RGBA = 4 bits per pixel; NV12 = 2 bpp
    videoFrame.setMetaData(QString::fromStdString("surface_interop"), QVariant::fromValue(m_surfaceInterop));

    if (m_zeroCopy)
        return videoFrame;
    else
        return videoFrame.to(QtAV::VideoFormat::Format_NV12);

}

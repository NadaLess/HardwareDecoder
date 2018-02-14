#include "surfaced3d9.h"
#include "d3d9interop.h"
#include <QDebug>

extern "C" {
#include "libavutil/imgutils.h"
}

#include "yuv2rgb.h"

#define NV12_FORMAT 0x3231564e  // 2 1 V N

SurfaceD3D9::SurfaceD3D9(IDirect3DSurface9 * surface, int width, int height)
    : Surface(width, height)
    , m_device(nullptr)
    , m_texture(nullptr)
    , m_surface(nullptr)
    , m_origSurface(nullptr)
    , m_shareHandle(nullptr)
    , gl_handleD3D(nullptr)
    , gl_handle(nullptr)
{
    if (surface == nullptr) {
        qWarning() << Q_FUNC_INFO << "Null Surface!!!";
        return;
    }

    m_origSurface = surface;
    if (FAILED(m_origSurface->GetDevice(&m_device))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device from the Surface";
        return;
    }

    m_origSurface->GetDesc(&m_surfaceDesc);

    if (D3D9Interop::instance()->enabled()) {
        //Copy Original Surface for Zero-Copy Rendering
        m_device->CreateTexture(m_surfaceDesc.Width, m_surfaceDesc.Height, 1,
                                D3DUSAGE_RENDERTARGET,
                                D3DFMT_X8R8G8B8,
                                m_surfaceDesc.Pool,
                                &m_texture,
                                &m_shareHandle);

        m_texture->GetSurfaceLevel(0, &m_surface);
        m_device->StretchRect(surface, NULL, m_surface, NULL, D3DTEXF_NONE);
    } else {
        extractSurfaceData();
    }
}

SurfaceD3D9::~SurfaceD3D9() {

    if (m_surface)
        m_surface->Release();
    m_surface = nullptr;

    if (m_texture)
        m_texture->Release();
    m_texture = nullptr;

    m_device = nullptr;
    gl_handleD3D = nullptr;
    gl_handle = nullptr;
}

bool SurfaceD3D9::map(GLuint name)
{
    if (!D3D9Interop::instance()->enabled())
        return Surface::map(name);

    initGLFunctions();

    if (m_surface == nullptr) return false;

    // required by d3d9 not d3d10&11: https://www.opengl.org/registry/specs/NV/DX_interop2.txt
    D3D9Interop::instance()->wglDXSetResourceShareHandleNV(m_surface, m_shareHandle);

    // register the Direct3D device with GL
    gl_handleD3D = D3D9Interop::instance()->wglDXOpenDeviceNV(m_device);
    if (gl_handleD3D == NULL) {
        qWarning() << Q_FUNC_INFO << "wglDXOpenDeviceNV" << GetLastError();
        return false;
    }

    gl_handle = D3D9Interop::instance()->wglDXRegisterObjectNV(gl_handleD3D,
                                      m_surface,
                                      name,
                                      GL_TEXTURE_2D,
                                      WGL_ACCESS_READ_ONLY_NV);
    if (gl_handle == NULL) {
        qWarning() << Q_FUNC_INFO << "wglDXRegisterObjectNV" << GetLastError();
        return false;
    }

    bool lock = D3D9Interop::instance()->wglDXLockObjectsNV(gl_handleD3D, 1, &gl_handle);
    bool objectAccess = D3D9Interop::instance()->wglDXObjectAccessNV(gl_handle, WGL_ACCESS_READ_ONLY_NV);

    m_glFunctions.glBindTexture(GL_TEXTURE_2D, name);

    return lock && objectAccess;
}

bool SurfaceD3D9::unmap()
{
    m_glFunctions.glBindTexture(GL_TEXTURE_2D, 0);

    if (!D3D9Interop::instance()->enabled()) return false;

    bool unlock = D3D9Interop::instance()->wglDXUnlockObjectsNV(gl_handleD3D, 1, &gl_handle);
    bool unregister = D3D9Interop::instance()->wglDXUnregisterObjectNV(gl_handleD3D, gl_handle);
    bool closeDevice = D3D9Interop::instance()->wglDXCloseDeviceNV(gl_handleD3D);

    gl_handleD3D = NULL;
    gl_handle = NULL;

    return unlock && unregister && closeDevice;
}

void SurfaceD3D9::extractSurfaceData()
{
    if (!m_rgbData.isEmpty()) return;

    if (m_surfaceDesc.Format != NV12_FORMAT)
    {
        qWarning() << Q_FUNC_INFO << "Wrong format, expected NV12";
        return;
    }

    if (m_origSurface)
    {
        D3DLOCKED_RECT lockedRect;
        ZeroMemory(&lockedRect, sizeof(D3DLOCKED_RECT));
        HRESULT hr = m_origSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
        if (SUCCEEDED(hr))
        {
            //Resize RGB Data Buffer
            size_t size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_width, m_height, 1);
            m_rgbData.resize(size);

            //Convert NV12 Buffer to RGB stored Buffer
            nv21_to_bgr((unsigned char*)m_rgbData.data(),
                        (unsigned char*)cropImage(lockedRect).data(),
                        m_width, m_height);

            m_origSurface->UnlockRect();
        }
    }
}

QByteArray SurfaceD3D9::cropImage(D3DLOCKED_RECT & lockedRect)
{
    size_t dstSize = av_image_get_buffer_size(AV_PIX_FMT_NV12, m_width, m_height, 1);
    QByteArray dstData(dstSize, 0x00);
    char * dstYData = dstData.data();
    char * dstUVData = dstYData + (m_width * m_height);

    char * srcYData = (char*)lockedRect.pBits;
    char * srcUVData = srcYData + (lockedRect.Pitch * m_surfaceDesc.Height);

    int chromaHeight = m_height >> 1;
    for (int i = 0; i < m_height; i++) {
        memcpy(dstYData, srcYData, m_width);
        srcYData += lockedRect.Pitch;
        dstYData += m_width;

        if (i < chromaHeight) {
            memcpy(dstUVData, srcUVData, m_width);
            srcUVData += lockedRect.Pitch;
            dstUVData += m_width;
        }
    }

    return dstData;
}

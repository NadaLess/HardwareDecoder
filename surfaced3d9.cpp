#include "surfaced3d9.h"
#include <QDebug>

SurfaceD3D9::SurfaceD3D9(IDirect3DSurface9 * surface)
    : Surface()
    , dx_surface(nullptr)
    , dx_texture(nullptr)
    , sharedHandle(nullptr)
    , gl_handleD3D(nullptr)
    , gl_handle(nullptr)
{
    if (surface == nullptr) return;

    if (FAILED(surface->GetDevice(&dx_device))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device from the Surface";
        return;
    }

    surface->GetDesc(&surfaceDescriptor);

    dx_device->CreateTexture(surfaceDescriptor.Width, surfaceDescriptor.Height, 1,
                             D3DUSAGE_RENDERTARGET,
                             D3DFMT_X8R8G8B8,
                             D3DPOOL_DEFAULT,
                             &dx_texture,
                             &sharedHandle);

    dx_texture->GetSurfaceLevel(0, &dx_surface);
    dx_device->StretchRect(surface, NULL, dx_surface, NULL, D3DTEXF_NONE);

    _resetWGLFunctions();
}

SurfaceD3D9::~SurfaceD3D9() {
    _resetWGLFunctions();

    if (dx_surface)
        dx_surface->Release();
    dx_surface = nullptr;

    if (dx_texture)
        dx_texture->Release();
    dx_texture = nullptr;

    gl_handleD3D = nullptr;
    gl_handle = nullptr;
}

bool SurfaceD3D9::map(GLuint name)
{
    if (!_initWGLFunctions()) return false;

    if (dx_surface == nullptr) return false;

    // required by d3d9 not d3d10&11: https://www.opengl.org/registry/specs/NV/DX_interop2.txt
    wglDXSetResourceShareHandleNV(dx_surface, sharedHandle);

    // register the Direct3D device with GL
    gl_handleD3D = wglDXOpenDeviceNV(dx_device);
    if (gl_handleD3D == NULL)
        return false;

    gl_handle = wglDXRegisterObjectNV(gl_handleD3D,
                                      dx_surface,
                                      name,
                                      GL_TEXTURE_2D,
                                      WGL_ACCESS_READ_ONLY_NV);
    if (gl_handle == NULL)
        return false;

    bool lock = wglDXLockObjectsNV(gl_handleD3D, 1, &gl_handle);
    bool objectAccess = wglDXObjectAccessNV(gl_handle, WGL_ACCESS_READ_ONLY_NV);

    return lock && objectAccess;
}

bool SurfaceD3D9::unmap()
{
    if (!_checkWGLFunctions()) return false;

    bool unlock = wglDXUnlockObjectsNV(gl_handleD3D, 1, &gl_handle);
    bool unregister = wglDXUnregisterObjectNV(gl_handleD3D, gl_handle);
    bool closeDevice = wglDXCloseDeviceNV(gl_handleD3D);

    gl_handleD3D = NULL;
    gl_handle = NULL;

    return unlock && unregister && closeDevice;
}

int SurfaceD3D9::width()
{
    return surfaceDescriptor.Width;
}

int SurfaceD3D9::height()
{
    return surfaceDescriptor.Height;
}

bool SurfaceD3D9::_initWGLFunctions()
{
    if (_checkWGLFunctions()) return true;

    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (context == nullptr) return false;

    QByteArray interopExtension("WGL_NV_DX_interop");

    if (!context->hasExtension(interopExtension)) {
        return false;
    }

    wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)context->getProcAddress("wglDXOpenDeviceNV");
    wglDXCloseDeviceNV = (PFNWGLDXCLOSEDEVICENVPROC)context->getProcAddress("wglDXCloseDeviceNV");

    wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)context->getProcAddress("wglDXRegisterObjectNV");
    wglDXUnregisterObjectNV = (PFNWGLDXUNREGISTEROBJECTNVPROC)context->getProcAddress("wglDXUnregisterObjectNV");

    wglDXLockObjectsNV = (PFNWGLDXLOCKOBJECTSNVPROC)context->getProcAddress("wglDXLockObjectsNV");
    wglDXUnlockObjectsNV = (PFNWGLDXUNLOCKOBJECTSNVPROC)context->getProcAddress("wglDXUnlockObjectsNV");

    wglDXSetResourceShareHandleNV = (PFNWGLDXSETRESOURCESHAREHANDLENVPROC)context->getProcAddress("wglDXSetResourceShareHandleNV");

    wglDXObjectAccessNV = (PFNWGLDXOBJECTACCESSNVPROC)context->getProcAddress("wglDXObjectAccessNV");
    return true;
}

void SurfaceD3D9::_resetWGLFunctions()
{
    wglDXOpenDeviceNV = nullptr;
    wglDXCloseDeviceNV = nullptr;

    wglDXRegisterObjectNV = nullptr;
    wglDXUnregisterObjectNV = nullptr;

    wglDXLockObjectsNV = nullptr;
    wglDXUnlockObjectsNV = nullptr;

    wglDXSetResourceShareHandleNV = nullptr;

    wglDXObjectAccessNV = nullptr;
}

bool SurfaceD3D9::_checkWGLFunctions()
{
    return (wglDXOpenDeviceNV != nullptr &&
            wglDXCloseDeviceNV != nullptr &&
            wglDXRegisterObjectNV != nullptr &&
            wglDXUnregisterObjectNV != nullptr &&
            wglDXLockObjectsNV != nullptr &&
            wglDXUnlockObjectsNV != nullptr &&
            wglDXSetResourceShareHandleNV != nullptr &&
            wglDXObjectAccessNV != nullptr);
}

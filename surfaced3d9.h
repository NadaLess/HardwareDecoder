#ifndef SURFACED3D9_H
#define SURFACED3D9_H

#include "surface.h"
#include <d3d9.h>

#ifdef Q_OS_WIN32
    //#include <GL/wglext.h> //not found in vs2013
    //https://www.opengl.org/registry/specs/NV/DX_interop.txt
#ifndef WGL_ACCESS_READ_ONLY_NV
#define WGL_ACCESS_READ_ONLY_NV           0x00000000
#define WGL_ACCESS_READ_WRITE_NV          0x00000001
#define WGL_ACCESS_WRITE_DISCARD_NV       0x00000002
typedef BOOL (WINAPI * PFNWGLDXSETRESOURCESHAREHANDLENVPROC) (void *dxObject, HANDLE shareHandle);
typedef HANDLE (WINAPI * PFNWGLDXOPENDEVICENVPROC) (void *dxDevice);
typedef BOOL (WINAPI * PFNWGLDXCLOSEDEVICENVPROC) (HANDLE hDevice);
typedef HANDLE (WINAPI * PFNWGLDXREGISTEROBJECTNVPROC) (HANDLE hDevice, void *dxObject, GLuint name, GLenum type, GLenum access);
typedef BOOL (WINAPI * PFNWGLDXUNREGISTEROBJECTNVPROC) (HANDLE hDevice, HANDLE hObject);
typedef BOOL (WINAPI * PFNWGLDXOBJECTACCESSNVPROC) (HANDLE hObject, GLenum access);
typedef BOOL (WINAPI * PFNWGLDXLOCKOBJECTSNVPROC) (HANDLE hDevice, GLint count, HANDLE *hObjects);
typedef BOOL (WINAPI * PFNWGLDXUNLOCKOBJECTSNVPROC) (HANDLE hDevice, GLint count, HANDLE *hObjects);
#endif
#endif

class SurfaceD3D9: public Surface
{    
public:
    SurfaceD3D9(IDirect3DSurface9 * surface);
    ~SurfaceD3D9();

    // Surface interface
    virtual bool map(GLuint name) override;
    virtual bool unmap() override;
    virtual UINT width() override;
    virtual UINT height() override;

private:
    bool _initWGLFunctions();
    void _resetWGLFunctions();
    bool _checkWGLFunctions();

    PFNWGLDXOPENDEVICENVPROC wglDXOpenDeviceNV;
    PFNWGLDXCLOSEDEVICENVPROC wglDXCloseDeviceNV;

    PFNWGLDXREGISTEROBJECTNVPROC wglDXRegisterObjectNV;
    PFNWGLDXUNREGISTEROBJECTNVPROC wglDXUnregisterObjectNV;

    PFNWGLDXLOCKOBJECTSNVPROC wglDXLockObjectsNV;
    PFNWGLDXUNLOCKOBJECTSNVPROC wglDXUnlockObjectsNV;

    PFNWGLDXSETRESOURCESHAREHANDLENVPROC wglDXSetResourceShareHandleNV;
    PFNWGLDXOBJECTACCESSNVPROC wglDXObjectAccessNV;

    IDirect3DDevice9 *dx_device;
    IDirect3DTexture9 *dx_texture;
    IDirect3DSurface9 *dx_surface;
    D3DSURFACE_DESC surfaceDescriptor;

    //NV/DX Interop Handles
    HANDLE sharedHandle;
    HANDLE gl_handleD3D;
    HANDLE gl_handle;
};

#endif // SURFACED3D9_H

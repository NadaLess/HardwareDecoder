#ifndef D3D9INTEROP_H
#define D3D9INTEROP_H

#include <QOpenGLContext>

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

class D3D9Interop
{
public:
    static D3D9Interop* instance();
    bool enabled();

    PFNWGLDXOPENDEVICENVPROC wglDXOpenDeviceNV;
    PFNWGLDXCLOSEDEVICENVPROC wglDXCloseDeviceNV;

    PFNWGLDXREGISTEROBJECTNVPROC wglDXRegisterObjectNV;
    PFNWGLDXUNREGISTEROBJECTNVPROC wglDXUnregisterObjectNV;

    PFNWGLDXLOCKOBJECTSNVPROC wglDXLockObjectsNV;
    PFNWGLDXUNLOCKOBJECTSNVPROC wglDXUnlockObjectsNV;

    PFNWGLDXSETRESOURCESHAREHANDLENVPROC wglDXSetResourceShareHandleNV;
    PFNWGLDXOBJECTACCESSNVPROC wglDXObjectAccessNV;

private:
    D3D9Interop();
    ~D3D9Interop();

    bool _initWGLFunctions();
    void _resetWGLFunctions();
    bool _checkWGLFunctions();
};

#endif // D3D9INTEROP_H

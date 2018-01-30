#include "surfaced3d9.h"
#include <QDebug>

SurfaceD3D9::SurfaceD3D9(IDirect3DSurface9 * surface):
    Surface()
  , m_d3d9Surface(nullptr)
  , m_interopDev(nullptr)
  , m_interopObj(nullptr)
{
    surface->AddRef();
    m_d3d9Surface = surface;

    _resetWGLFunctions();
}

SurfaceD3D9::~SurfaceD3D9() {
    _resetWGLFunctions();

    m_interopDev = nullptr;
    m_interopObj = nullptr;

    if (m_d3d9Surface)
        m_d3d9Surface->Release();
    m_d3d9Surface = nullptr;
}

//    // register the Direct3D device with GL
//    HANDLE gl_handleD3D;
//    gl_handleD3D = wglDXOpenDeviceNV(device);

//    // register the Direct3D color and depth/stencil buffers as
//    // 2D multisample textures in opengl
//    GLuint gl_names[2];
//    HANDLE gl_handles[2];

//    glGenTextures(2, gl_names);

//    gl_handles[0] = wglDXRegisterObjectNV(gl_handleD3D,
//                                          dxColorBuffer,
//                                          gl_names[0],
//            GL_TEXTURE_2D_MULTISAMPLE,
//            WGL_ACCESS_READ_WRITE_NV);

//    gl_handles[1] = wglDXRegisterObjectNV(gl_handleD3D,
//                                          dxDepthBuffer,
//                                          gl_names[1],
//            GL_TEXTURE_2D_MULTISAMPLE,
//            WGL_ACCESS_READ_WRITE_NV);

//    // attach the Direct3D buffers to an FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
//                           GL_TEXTURE_2D_MULTISAMPLE, gl_names[0]);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//                           GL_TEXTURE_2D_MULTISAMPLE, gl_names[1]);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
//                           GL_TEXTURE_2D_MULTISAMPLE, gl_names[1]);

//    // rendering loop
//    while (!done) {
//        // TODO : <direct3d renders to the render targets>

//        // lock the render targets for GL access
//        wglDXLockObjectsNV(gl_handleD3D, 2, gl_handles);

//        // TODO : <opengl renders to the render targets>

//        // unlock the render targets
//        wglDXUnlockObjectsNV(gl_handleD3D, 2, gl_handles);

//        // TODO : <direct3d renders to the render targets and presents the results on the screen>
//    }

bool SurfaceD3D9::map(GLuint name)
{
    if (m_d3d9Surface == nullptr) return false;

    if (!_initWGLFunctions()) return false;

    IDirect3DDevice9 * device = nullptr;
    if (FAILED(m_d3d9Surface->GetDevice(&device))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device from the Surface";
        return false;
    }

    HANDLE share_handle = NULL;
    // required by d3d9 not d3d10&11: https://www.opengl.org/registry/specs/NV/DX_interop2.txt
    if (!wglDXSetResourceShareHandleNV(m_d3d9Surface, share_handle)) {
        qWarning() << Q_FUNC_INFO << "Error wglDXSetResourceShareHandleNV";
        return false;
    }

    // register the Direct3D device with GL
    m_interopDev = wglDXOpenDeviceNV(device);
    if (m_interopDev == nullptr) {
        qWarning() << Q_FUNC_INFO << "Error opening DX device";
        return false;
    }

    // register the Direct3D as 2D multisample textures in opengl
    m_interopObj = wglDXRegisterObjectNV(m_interopDev, m_d3d9Surface, name, GL_TEXTURE_2D, WGL_ACCESS_READ_ONLY_NV);
    if (m_interopObj == nullptr) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) qDebug() << "ERROR_INVALID_HANDLE";
        else if (error == ERROR_INVALID_DATA) qDebug() << "ERROR_INVALID_DATA";
        else if (error == ERROR_OPEN_FAILED) qDebug() << "ERROR_OPEN_FAILED";
        else qDebug() << "UNKNOWN_ERROR";

        qWarning() << Q_FUNC_INFO << "Error registering DX resources";
        if (m_interopDev) {
            wglDXCloseDeviceNV(m_interopDev);
            m_interopDev = nullptr;
        }
        return false;
    }

    // The example attach the Direct3D buffers to an FBO - QtAV do nothing

    // lock dx resources
    if (!wglDXLockObjectsNV(m_interopDev, 1, &m_interopObj)) {
        qWarning() << Q_FUNC_INFO << "Error locking DX resources";

        wglDXUnregisterObjectNV(m_interopDev, m_interopObj);
        wglDXCloseDeviceNV(m_interopDev);

        m_interopObj = NULL;
        m_interopDev = NULL;

        return false;
    }

    return true;
}

void SurfaceD3D9::unmap()
{
    if (!m_interopObj || !m_interopDev || !_checkWGLFunctions())
        return;

    wglDXUnlockObjectsNV(m_interopDev, 1, &m_interopObj);
    wglDXUnregisterObjectNV(m_interopDev, m_interopObj);
    // interop operations end
    wglDXCloseDeviceNV(m_interopDev);

    m_interopObj = NULL;
    m_interopDev = NULL;

    return;
}

UINT SurfaceD3D9::width()
{
    if (m_d3d9Surface == nullptr) return 0;

    IDirect3DDevice9 * device = nullptr;
    if (FAILED(m_d3d9Surface->GetDevice(&device))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device from the Surface";
        return 0;
    }

    D3DSURFACE_DESC description;
    if (FAILED(m_d3d9Surface->GetDesc(&description))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device Description";
        return 0;
    }

    return description.Width;
}

UINT SurfaceD3D9::height()
{
    if (m_d3d9Surface == nullptr) return 0;

    IDirect3DDevice9 * device = nullptr;
    if (FAILED(m_d3d9Surface->GetDevice(&device))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device from the Surface";
        return 0;
    }

    D3DSURFACE_DESC description;
    if (FAILED(m_d3d9Surface->GetDesc(&description))) {
        qWarning() << Q_FUNC_INFO << "Error getting the Device Description";
        return 0;
    }

    return description.Height;
}

bool SurfaceD3D9::_initWGLFunctions()
{
    if (_checkWGLFunctions()) return false;

    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (context == nullptr) return false;

    QByteArray interopExtension("WGL_NV_DX_interop");
    QByteArray interop2Extension("WGL_NV_DX_interop2");

    if (!context->hasExtension(interopExtension) || !context->hasExtension(interop2Extension)) {
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

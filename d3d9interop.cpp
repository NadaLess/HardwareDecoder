#include "d3d9interop.h"
#include <QOffscreenSurface>
#include <QDebug>

D3D9Interop* D3D9Interop::instance()
{
    static D3D9Interop helper;
    return &helper;
}

bool D3D9Interop::enabled()
{
    return _checkWGLFunctions();
}

D3D9Interop::D3D9Interop()
{
    _resetWGLFunctions();
    _initWGLFunctions();
}

D3D9Interop::~D3D9Interop()
{
    _resetWGLFunctions();
}

bool D3D9Interop::_initWGLFunctions()
{
    if (_checkWGLFunctions()) return true;

    //Fnadales: This must be executed on gui thread
    QSurfaceFormat fmt;
    fmt.setMajorVersion(3);
    fmt.setMinorVersion(3);
    fmt.setProfile(QSurfaceFormat::CoreProfile); //whatever this is
    fmt.setRenderableType(QSurfaceFormat::OpenGL);

    QOpenGLContext * context = new QOpenGLContext();
    context->setFormat(fmt);
    context->create();

    QOffscreenSurface *offscreenSurface = new QOffscreenSurface();
    offscreenSurface->setFormat(context->format());
    offscreenSurface->create();

    context->makeCurrent(offscreenSurface);

    bool hasInterop = context->hasExtension("WGL_NV_DX_interop");
    if (hasInterop) {

        wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)context->getProcAddress("wglDXOpenDeviceNV");
        wglDXCloseDeviceNV = (PFNWGLDXCLOSEDEVICENVPROC)context->getProcAddress("wglDXCloseDeviceNV");

        wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)context->getProcAddress("wglDXRegisterObjectNV");
        wglDXUnregisterObjectNV = (PFNWGLDXUNREGISTEROBJECTNVPROC)context->getProcAddress("wglDXUnregisterObjectNV");

        wglDXLockObjectsNV = (PFNWGLDXLOCKOBJECTSNVPROC)context->getProcAddress("wglDXLockObjectsNV");
        wglDXUnlockObjectsNV = (PFNWGLDXUNLOCKOBJECTSNVPROC)context->getProcAddress("wglDXUnlockObjectsNV");

        wglDXSetResourceShareHandleNV = (PFNWGLDXSETRESOURCESHAREHANDLENVPROC)context->getProcAddress("wglDXSetResourceShareHandleNV");

        wglDXObjectAccessNV = (PFNWGLDXOBJECTACCESSNVPROC)context->getProcAddress("wglDXObjectAccessNV");
    }

    delete offscreenSurface;
    delete context;

    return hasInterop;
}

void D3D9Interop::_resetWGLFunctions()
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

bool D3D9Interop::_checkWGLFunctions()
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

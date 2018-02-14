#ifndef SURFACED3D9_H
#define SURFACED3D9_H

#include "surface.h"
#include <d3d9.h>

class SurfaceD3D9: public Surface
{    
public:
    SurfaceD3D9(IDirect3DSurface9 * surface, int width, int height);
    ~SurfaceD3D9();

    // Surface interface
    virtual bool map(GLuint name) override;
    virtual bool unmap() override;

private:
    void extractSurfaceData();
    QByteArray cropImage(D3DLOCKED_RECT & lockedRect);

    IDirect3DDevice9 *m_device;
    IDirect3DTexture9 *m_texture;
    IDirect3DSurface9 *m_surface;
    IDirect3DTexture9 *m_origTexture;
    IDirect3DSurface9 *m_origSurface;
    D3DSURFACE_DESC m_surfaceDesc;

    //NV/DX Interop Handles
    HANDLE m_shareHandle;
    HANDLE gl_handleD3D;
    HANDLE gl_handle;
};

#endif // SURFACED3D9_H

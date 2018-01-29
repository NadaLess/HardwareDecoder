#ifndef SURFACED3D9_H
#define SURFACED3D9_H

#include "surface.h"
#include <d3d9.h>

class SurfaceD3D9: public Surface
{    
public:
    SurfaceD3D9(IDirect3DSurface9 * surface);
    ~SurfaceD3D9();

private:
    IDirect3DSurface9 * m_d3d9Surface;
};

#endif // SURFACED3D9_H

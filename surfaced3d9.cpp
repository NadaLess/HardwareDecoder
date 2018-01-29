#include "surfaced3d9.h"

SurfaceD3D9::SurfaceD3D9(IDirect3DSurface9 * surface): Surface()
{
    surface->AddRef();
    m_d3d9Surface = surface;
}

SurfaceD3D9::~SurfaceD3D9() {
    m_d3d9Surface->Release();
    m_d3d9Surface = nullptr;
}

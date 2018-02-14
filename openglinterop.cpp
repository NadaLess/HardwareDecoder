#include "openglinterop.h"

#if defined(Q_OS_WIN)
#include "d3d9interop.h"
#endif

void OpenGLInterop::initialize()
{
#if defined(Q_OS_WIN)
    D3D9Interop::instance();
#endif
}

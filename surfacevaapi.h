#ifndef SURFACEVAAPI_H
#define SURFACEVAAPI_H

#include "surface.h"
#include <va/va_x11.h>
#include <GL/glx.h>

extern "C" {
#include "libavutil/hwcontext_vaapi.h"
}

typedef void (*t_glx_bind)(Display *, GLXDrawable, int , const int *);
typedef void (*t_glx_release)(Display *, GLXDrawable, int);

class SurfaceVAAPI: public Surface
{
public:
    SurfaceVAAPI(VADisplay display, VASurfaceID surface, int width, int height, QObject * parent = nullptr);
    ~SurfaceVAAPI();

    // Surface interface
public:
    bool map(GLuint name);
    bool unmap();

private:
    bool ensureDisplay();
    bool ensurePixmap();

    //OpenGL X11 Extensions for ZeroCopy
    bool initGLXFunctions();
    void resetGLXFunctions();
    bool checkGLXFunctions();

    t_glx_bind glXBindTexImageEXT;
    t_glx_release glXReleaseTexImageEXT;

    GLXPixmap m_glxPixmap;
    GLXFBConfig m_config;

    //VAAPI instances
    VADisplay m_vaDisplay;
    VASurfaceID m_surface;

    //X11 instances
    Display *m_x11Display;
    Pixmap m_pixmap;
};

#endif // SURFACEVAAPI_H

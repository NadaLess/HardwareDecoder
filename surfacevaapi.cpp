#include "surfacevaapi.h"
#include <QtPlatformHeaders/QGLXNativeContext>

const int pixmap_config[] = {
    GLX_BIND_TO_TEXTURE_RGBA_EXT, True,
    GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT,
    GLX_BIND_TO_TEXTURE_TARGETS_EXT, GLX_TEXTURE_2D_BIT_EXT,
    GLX_DOUBLEBUFFER, False,
    GLX_Y_INVERTED_EXT, GLX_DONT_CARE,
    None
};

const int pixmap_attribs[] = {
    GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
    GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGB_EXT,
    None
};

SurfaceVAAPI::SurfaceVAAPI(VADisplay display, VASurfaceID surface,
                           int width, int height, QObject * parent)
    : Surface(parent),
      m_glxPixmap(0),
      m_config(0),
      m_vaDisplay(display),
      m_surface(surface),
      m_x11Display(nullptr),
      m_pixmap(0),
      m_width(width),
      m_height(height)
{
    resetGLXFunctions();
}

SurfaceVAAPI::~SurfaceVAAPI()
{
    if (m_pixmap) {
        XFreePixmap(m_x11Display, m_pixmap);
        m_pixmap = 0;
    }

    if (m_glxPixmap) {
        glXReleaseTexImageEXT(m_x11Display, m_glxPixmap, GLX_FRONT_EXT);
        XSync(m_x11Display, False);
        glXDestroyPixmap(m_x11Display, m_glxPixmap);
        m_glxPixmap = 0;
    }

    resetGLXFunctions();
}

bool SurfaceVAAPI::map(GLuint name)
{
    glBindTexture(GL_TEXTURE_2D, name);

    if (!ensureDisplay()) return false;

    if (!ensurePixmap()) return false;

    if (!vaDisplayIsValid(m_vaDisplay)) return false;

    VAStatus syncResult = vaSyncSurface(m_vaDisplay, m_surface);
    if (syncResult != VA_STATUS_SUCCESS) return false;

    VAStatus putResult = vaPutSurface(m_vaDisplay, m_surface, m_pixmap
                 , 0, 0, m_width, m_height
                 , 0, 0, m_width, m_height
                 , NULL, 0, VA_FRAME_PICTURE | VA_SRC_BT709);
    if (putResult != VA_STATUS_SUCCESS) {
        qWarning() << Q_FUNC_INFO << "Error vaPutSurface" << putResult;
        return false;
    }

    XSync(m_x11Display, False);

    glXBindTexImageEXT(m_x11Display, m_glxPixmap, GLX_FRONT_EXT, NULL);

    return true;
}

bool SurfaceVAAPI::unmap()
{
    glXReleaseTexImageEXT(m_x11Display, m_glxPixmap, GLX_FRONT_EXT);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

int SurfaceVAAPI::width()
{
    return m_width;
}

int SurfaceVAAPI::height()
{
    return m_height;
}

bool SurfaceVAAPI::ensureDisplay()
{
    if (m_x11Display && m_config) return true;

    m_x11Display = glXGetCurrentDisplay();
    if (!m_x11Display) return false;

    if (!initGLXFunctions()) return false;

    int cn=0;
    int xscreen = DefaultScreen(m_x11Display);
    GLXFBConfig *configs = glXChooseFBConfig(m_x11Display, xscreen, pixmap_config, &cn);
    if(cn) {
        m_config = configs[0];
        XFree(configs);
        return true;
    } else
        return false;
}

bool SurfaceVAAPI::ensurePixmap()
{
    if (m_glxPixmap) return true;

    if (m_pixmap) {
        XFreePixmap(m_x11Display, m_pixmap);
        m_pixmap = 0;
    }

    XWindowAttributes xwa;
    XGetWindowAttributes(m_x11Display, DefaultRootWindow(m_x11Display), &xwa);
    m_pixmap = XCreatePixmap(m_x11Display, DefaultRootWindow(m_x11Display), m_width, m_height, xwa.depth);

    if (!m_pixmap) return false;

    m_glxPixmap = glXCreatePixmap(m_x11Display, m_config, m_pixmap, pixmap_attribs);
    if (!m_glxPixmap)
        return false;

    return true;
}

bool SurfaceVAAPI::initGLXFunctions()
{
    if (checkGLXFunctions()) return true;

    if (!m_x11Display) return false;

    int xscreen = DefaultScreen(m_x11Display);
    const char * exts = glXQueryExtensionsString(m_x11Display, xscreen);
    if(! strstr(exts, "GLX_EXT_texture_from_pixmap"))
    {
        fprintf(stderr, "GLX_EXT_texture_from_pixmap not supported!\n");
        return false;
    }

    glXBindTexImageEXT = (t_glx_bind) glXGetProcAddress((const GLubyte *)"glXBindTexImageEXT");
    glXReleaseTexImageEXT = (t_glx_release) glXGetProcAddress((const GLubyte *)"glXReleaseTexImageEXT");

    return true;
}

void SurfaceVAAPI::resetGLXFunctions()
{
    glXBindTexImageEXT = nullptr;
    glXReleaseTexImageEXT = nullptr;
}

bool SurfaceVAAPI::checkGLXFunctions()
{
    return glXBindTexImageEXT != nullptr
            && glXReleaseTexImageEXT != nullptr;
}

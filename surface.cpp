#include "surface.h"
#include <QImage>

Surface::Surface(int width, int height, QObject * parent)
    : QObject(parent),
      m_width(width),
      m_height(height)
{

}

Surface::~Surface() {

}

bool Surface::map(GLuint name) {
    initGLFunctions();

    //TODO: map Data to 2D Texture
    m_glFunctions.glBindTexture(GL_TEXTURE_2D, name);
    m_glFunctions.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, m_rgbData.data());

    return true;
}

bool Surface::unmap(){
    m_glFunctions.glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

int Surface::width() {
    return m_width;
}

int Surface::height() {
    return m_height;
}

void Surface::initGLFunctions()
{
    static bool glInitialized = false;
    if (!glInitialized)
        m_glFunctions.initializeOpenGLFunctions();
}

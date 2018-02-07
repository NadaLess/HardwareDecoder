#ifndef X11RENDERER_H
#define X11RENDERER_H

#include "framerenderer.h"

class X11Renderer: public FrameRenderer
{
public:
    X11Renderer();
    ~X11Renderer();

    // FrameRenderer interface
public:
    void render();
    void initialize();
};

#endif // X11RENDERER_H

#include "render.h"
#include <X11/Xlib.h>
#include "util.h"

void pixel(XImage *surfaceImage, int x, int y, Color color)
{
    XPutPixel(surfaceImage, x, y, createRGBA(color.r, color.g, color.b, color.a));
}
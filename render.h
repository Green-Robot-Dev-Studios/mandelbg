#ifndef RENDER_H
#define RENDER_H

#include <X11/Xlib.h>
#include "util.h"

void pixel(XImage *surfaceImage, int x, int y, Color color);

#endif
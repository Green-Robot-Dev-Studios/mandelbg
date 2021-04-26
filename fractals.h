#ifndef FRACTALS_H
#define FRACTALS_H

#include <X11/Xlib.h>

// defines all fractal functions
void mandelbrot(XImage *surfaceImage, int x, int y, int w, int h, int c);
void julia(XImage *surfaceImage, int x, int y, int w, int h, int c);

#endif
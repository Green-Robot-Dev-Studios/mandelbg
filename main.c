#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include "util.h"

int width, height;

void pixel(cairo_t *cairo, int x, int y, Color color) {
    cairo_set_source_rgb(cairo, color.r/255, color.g/255, color.b/255);
    cairo_rectangle(cairo, x, y, 1, 1);
    cairo_fill(cairo);
}

// for each pixel (Px, Py) on the screen do
//     x0 := scaled x coordinate of pixel (scaled to lie in the Mandelbrot X scale (-2.5, 1))
//     y0 := scaled y coordinate of pixel (scaled to lie in the Mandelbrot Y scale (-1, 1))
//     x := 0.0
//     y := 0.0
//     iteration := 0
//     max_iteration := 1000
//     while (x*x + y*y ≤ 2*2 AND iteration < max_iteration) do
//         xtemp := x*x - y*y + x0
//         y := 2*x*y + y0
//         x := xtemp
//         iteration := iteration + 1

//     color := palette[iteration]
//     plot(Px, Py, color)

// better optimised pseudo (wikepedia thank you)
// x2 := 0
// y2 := 0

// while (x2 + y2 ≤ 4 and iteration < max_iteration) do
//     y := 2 × x × y + y0
//     x := x2 - y2 + x0
//     x2 := x × x
//     y2 := y × y
//     iteration := iteration + 1

void mandelbrot(cairo_t *cairo, int x, int y, int w, int h) {
    // (((P-x)/w) * 3.5) - 2.5
    printf("Drawing mandelbrot for %d, %d size %d, %d\n",
    x, y, w, h);
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            float x0 = (((i - x) / (float)w) * 3.5) - 2.5;
            float y0 = (((j - y) / (float)h) * 2.0) - 1.0;
            float px = 0, py = 0;
            int maxIter = 1000, iter = 0;
            while (px*px + py*py <= 2*2 && iter < maxIter) {
                float xTemp = px * px - py * py + x0;
                py = 2.0*px*py + y0;
                px = xTemp;
                iter++;
            }
            Color color;
            color.r = iter / maxIter * 255;
            color.g = iter / maxIter * 150;
            color.b = iter / maxIter * 80;
            //printf("%d\n", iter);
            pixel(cairo, i, j, color);
        }
    }
}

void draw(cairo_t *cairo, int x, int y, int w, int h) {
    // cairo_set_source_rgb(cairo, 1.0, 0.0, 0.0);
    // cairo_rectangle(cairo, x, y, w, h);
    // cairo_fill(cairo);
    mandelbrot(cairo, x, y, w, h);
}

int main(int argc, char *argv[]) {
    // parse args
    short int searching = -1;
    short int screens = 0;
    for (int i = 1; i < argc; i++) {
        if (searching != -1) {
            screens++;
        }
        if (strcmp(argv[i], "-m") == 0) {
            searching = i+1;
        }
    }
    char screenNamesToSearch[screens][255];
    for (int i = 0; i < screens; i++) {
        strcpy(screenNamesToSearch[i], argv[i+searching]);
    }
    printf("Screens to draw to: %d\n", screens);
    for (int i = 0; i < screens; i++) {
        printf("Screen %d: %s\n", i, screenNamesToSearch[i]);
    }

    // connect to X server and get root window
    Display *display = XOpenDisplay(NULL);
    int defaultScreen = DefaultScreen(display);
    Window rootWindow = RootWindow(display, defaultScreen);
    width = DisplayWidth(display, defaultScreen);
    height = DisplayHeight(display, defaultScreen);

    // get crtc names
    XRRScreenResources *res = XRRGetScreenResourcesCurrent(display, rootWindow);
    XRROutputInfo *output_info;
    typedef struct ScreenGeometry {
        int x;
        int y;
        int w;
        int h;
    } ScreenGeometry;
    ScreenGeometry screenGeometryToDraw[screens];
    for (int i = 0; i < res->noutput; i++) {
        output_info = XRRGetOutputInfo(display, res, res->outputs[i]);
        if (output_info->connection)
            continue; // No connection no crtcs
        for (int j = 0; j < screens; j++) {
            if (strcmp(output_info->name, screenNamesToSearch[j]) == 0) {
                XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, res, output_info->crtc);
                ScreenGeometry temp;
                temp.x = crtc_info->x;
                temp.y = crtc_info->y;
                temp.w = crtc_info->width;
                temp.h = crtc_info->height;
                screenGeometryToDraw[j] = temp;
            }
        }
        printf("Found moniotr: %s\n", output_info->name);
    }
    for (int i = 0; i < screens; i++) {
        printf("Drawing at (%d,%d) with size of (%d,%d)\n", 
        screenGeometryToDraw[i].x, screenGeometryToDraw[i].y, 
        screenGeometryToDraw[i].w, screenGeometryToDraw[i].h);
    }

    // create surface
    cairo_surface_t *surface = cairo_xlib_surface_create(display, rootWindow, DefaultVisual(display, defaultScreen), width, height);
    cairo_t *cairo = cairo_create(surface);

    // draw to surface
    for (int i = 0; i < screens; i++) {
        draw(cairo, screenGeometryToDraw[i].x, screenGeometryToDraw[i].y,
             screenGeometryToDraw[i].w, screenGeometryToDraw[i].h);
    }
    XSelectInput(display, rootWindow, ExposureMask);
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            for (int i = 0; i < screens; i++) {
                draw(cairo, screenGeometryToDraw[i].x, screenGeometryToDraw[i].y, 
                screenGeometryToDraw[i].w, screenGeometryToDraw[i].h);
            }
        }
    }

    // free mem
    cairo_destroy(cairo);
    cairo_surface_destroy(surface);
    XCloseDisplay(display);

    return 0;
}

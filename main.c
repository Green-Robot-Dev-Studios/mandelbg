#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

int width, height;

void draw(cairo_t *cairo, int x, int y, int w, int h) {
    cairo_set_source_rgb(cairo, 1.0, 0.0, 0.0);
    cairo_rectangle(cairo, x, y, w, h);
    cairo_fill(cairo);
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

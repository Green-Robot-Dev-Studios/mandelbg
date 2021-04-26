#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "util.h"
#include "fractals.h"

int width, height, needsRedraw = 1;

void draw(XImage *surfaceImage, int x, int y, int w, int h, int c)
{
    mandelbrot(surfaceImage, x, y, w, h, c);
    //julia(surfaceImage, x, y, w, h, c);
}

int main(int argc, char *argv[])
{
    // parse args
    short int searching = -1;
    short int screens = 0;
    for (int i = 1; i < argc; i++)
    {
        if (searching != -1)
        {
            screens++;
        }
        if (strcmp(argv[i], "-m") == 0)
        {
            searching = i + 1;
        }
    }
    char screenNamesToSearch[screens][255];
    for (int i = 0; i < screens; i++)
    {
        strcpy(screenNamesToSearch[i], argv[i + searching]);
    }
    printf("Screens to draw to: %d\n", screens);
    for (int i = 0; i < screens; i++)
    {
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
    typedef struct ScreenGeometry
    {
        int x;
        int y;
        int w;
        int h;
    } ScreenGeometry;
    ScreenGeometry screenGeometryToDraw[screens];
    for (int i = 0; i < res->noutput; i++)
    {
        output_info = XRRGetOutputInfo(display, res, res->outputs[i]);
        if (output_info->connection)
            continue; // No connection no crtcs
        for (int j = 0; j < screens; j++)
        {
            if (strcmp(output_info->name, screenNamesToSearch[j]) == 0)
            {
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
    for (int i = 0; i < screens; i++)
    {
        printf("Drawing at (%d,%d) with size of (%d,%d)\n",
               screenGeometryToDraw[i].x, screenGeometryToDraw[i].y,
               screenGeometryToDraw[i].w, screenGeometryToDraw[i].h);
    }

    // init XImage surface and define a graphical context
    XImage *surfaceImage;
    char *data = calloc(width * height, 4);
    surfaceImage = XCreateImage(display, DefaultVisual(display, defaultScreen), 
        DefaultDepth(display, defaultScreen), ZPixmap, 0,
        data, width, height, 32, 0);
    GC gc = XCreateGC(display, rootWindow, 0, 0);
    Pixmap pixmap = XCreatePixmap(display, rootWindow, width, height, 
        DefaultDepth(display, defaultScreen));

    // draw to surface
    // XSelectInput(display, rootWindow, ButtonPressMask);
    // XEvent event;
    // while (1)
    // {
    //     XNextEvent(display, &event);
    //     if (event.type == Expose)
    //     {
    //     }
    // }
    XSelectInput(display, rootWindow, KeyPressMask);
    XEvent event;
    int color = 0;
    XSetWindowBackgroundPixmap(display, rootWindow, pixmap);

    while (1) {
        // working on event code
        // if (needsRedraw == 0)
        //     XNextEvent(display, &event);

        // if (needsRedraw == 0) {
        //     sleep(3);
        //     color++;
        //     if (color > 2) color = 0;
        //     needsRedraw = 1;
        //     printf("Changing Color! %d\n", color);
        // }

        // if (event.type == KeyPress)
        // {
        //     printf("Event filtered!\n");
        //     color = 1;
        //     needsRedraw = 1;
        // }
        if (needsRedraw == 1)
        {
            for (int i = 0; i < screens; i++)
            {
                draw(surfaceImage, screenGeometryToDraw[i].x, screenGeometryToDraw[i].y,
                    screenGeometryToDraw[i].w, screenGeometryToDraw[i].h, color);
                XPutImage(display, pixmap, gc, surfaceImage, 0, 0, 0, 0, width, height);
                XClearWindow(display, rootWindow);
                XSync(display, False);
                XFlush(display);
            }
            needsRedraw = 0;
        }
    }
    
    // free mem
    XCloseDisplay(display);

    return 0;
}

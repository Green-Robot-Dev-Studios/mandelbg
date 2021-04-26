#include "../fractals.h"
#include <X11/Xlib.h>
#include "../util.h"
#include "../render.h"

// R = escape radius  # choose R > 0 such that R**2 - R >= sqrt(cx**2 + cy**2)
// for each pixel (x, y) on the screen, do:   
// {
//     zx = scaled x coordinate of pixel # (scale to be between -R and R)
//        # zx represents the real part of z.
//     zy = scaled y coordinate of pixel # (scale to be between -R and R)
//        # zy represents the imaginary part of z.
//     iteration = 0
//     max_iteration = 1000
//     while (zx * zx + zy * zy < R**2  AND  iteration < max_iteration) 
//     {
//         xtemp = zx * zx - zy * zy
//         zy = 2 * zx * zy  + cy 
//         zx = xtemp + cx
//         iteration = iteration + 1 
//     }
//     if (iteration == max_iteration)
//         return black;
//     else
//         return iteration;
// }

void julia(XImage *surfaceImage, int x, int y, int w, int h, int c) {
    printf("Drawing julia for %d, %d size %d, %d\n",
        x, y, w, h);
    int r = 2;
    float cx = -0.4, cy = 0.6;
    for (int i = x; i < x + w; i++)
    {
        for (int j = y; j < y + h; j++)
        {
            float zx = (((i - x) / (float)w) * r*2) - r;
            float zy = (((j - y) / (float)h) * r*2) - r;
            int maxIter = 1000, iter = 0;
            while (zx*zx + zy*zy < r*r && iter < maxIter) {
                float xTemp = zx*zx - zy*zy;
                zy = 2.0*zx*zy + cy;
                zx = xTemp + cx;
                iter++;
            }
            Color color;
            color.r = 0;
            color.g = 0;
            color.b = 0;
            switch (c)
            {
            case 0:
                color.r = (iter * 255) / maxIter;
                if (iter == maxIter)
                    color.r = 0;
                break;
            case 1:
                color.g = (iter * 255) / maxIter;
                if (iter == maxIter)
                    color.g = 0;
                break;
            case 2:
                color.b = (iter * 255) / maxIter;
                if (iter == maxIter)
                    color.b = 0;
                break;
            }
            color.a = 0;
            pixel(surfaceImage, i, j, color);
        }
    }
}
#include "../fractals.h"
#include <X11/Xlib.h>
#include "../util.h"
#include "../render.h"

// better optimised pseudo (wikepedia thank you)
// x2 := 0
// y2 := 0

// while (x2 + y2 ≤ 4 and iteration < max_iteration) do
//     y := 2 × x × y + y0
//     x := x2 - y2 + x0
//     x2 := x × x
//     y2 := y × y
//     iteration := iteration + 1

void mandelbrot(XImage *surfaceImage, int x, int y, int w, int h, int c)
{
    printf("Drawing mandelbrot for %d, %d size %d, %d\n",
    x, y, w, h);
    for (int i = x; i < x + w; i++)
    {
        for (int j = y; j < y + h; j++)
        {
            float x0 = (((i - x) / (float)w) * 3.5) - 2.5;
            float y0 = (((j - y) / (float)h) * 2.0) - 1.0;
            float x1 = 0, y1 = 0;
            float px = 0, py = 0;
            int maxIter = 35, iter = 0;
            while (x1 + y1 <= 4 && iter < maxIter)
            {
                py = 2.0 * px * py + y0;
                px = x1 - y1 + x0;
                x1 = px * px;
                y1 = py * py;
                iter++;
            }
            //printf("first %f, iter %d\n", x1+y1, iter);
            Color color;
            color.r = 0;
            color.g = 0;
            color.b = 0;
            switch (c) {
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
            pixel(surfaceImage, i, j, color);
        }
    }
}
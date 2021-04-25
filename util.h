#ifndef UTIL_H
#define UTIL_H

typedef struct Color
{
    int r;
    int g;
    int b;
} Color;

unsigned long createRGB(int r, int g, int b);

#endif
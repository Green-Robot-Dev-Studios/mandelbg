#ifndef UTIL_H
#define UTIL_H

typedef struct Color
{
    int r;
    int g;
    int b;
    int a;
} Color;

unsigned long createRGBA(int r, int g, int b, int a);

#endif
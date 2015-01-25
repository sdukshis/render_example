#include "draw.h"

void swap(unsigned int *a, unsigned int *b)
{
    unsigned int t = *a;
    *a = *b;
    *b = t;
}

int abs(int a) {
    return (a >= 0) ? a : -a;
}

int sign(int a) {
    if (a > 0) {
        return 1;
    } else if (a < 0) {
        return -1;
    } else {
        return 0;
    }
}

void line(unsigned int x1, unsigned int y1,
          unsigned int x2, unsigned int y2,
          tgaImage *image, tgaColor color)
{
    int steep = 0;
    if (abs(x2 - x1) < abs(y2 - y1)) {
        steep = 1;
        swap(&x1, &y1);
        swap(&x2, &y2);
    }

    if (x1 > x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    int dx = x2 - x1;
    int dy = y2 - y1;
    int sy = sign(dy);
    int e = 0;
    int de = 2 * abs(dy);

    for (; x1 <= x2; ++x1) {
        if (steep) {
            tgaSetPixel(image, y1, x1, color);
        } else {
            tgaSetPixel(image, x1, y1, color);
        }

        e += de;
        if (e > dx) {
            y1 += sy;
            e -= 2 * dx;
        }
    }
}

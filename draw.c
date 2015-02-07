#include "draw.h"
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



void swap(void *a, void *b, size_t bytes)
{
    void *t = malloc(bytes);
    memcpy(t, a, bytes);
    memmove(a, b, bytes);
    memcpy(b, t, bytes);
    free(t);
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
        swap(&x1, &y1, sizeof(unsigned int));
        swap(&x2, &y2, sizeof(unsigned int));
    }

    if (x1 > x2) {
        swap(&x1, &x2, sizeof(unsigned int));
        swap(&y1, &y2, sizeof(unsigned int));
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

void barycentric(Vec3 *bar, Vec3 *pts, Vec3i *p)
{
    Vec3 v = {pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - (*p)[0]};
    Vec3 w = {pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - (*p)[1]};
    Vec3 u = {0.0, 0.0, 0.0};
    cross_prod(&u, &v, &w);
    if (fabs(u[2]) < 1.0) { // triangle is degenerate 
        (*bar)[0] = -1.0; (*bar)[1] = 1.0; (*bar)[2] = 1.0;
    } else {
        (*bar)[0] = 1.0 - (u[0] + u[1])/u[2];
        (*bar)[1] = u[1]/u[2];
        (*bar)[2] = u[0]/u[2];
    }
}

void triangle(Vec3 *pts,
              tgaImage *image,
              fragment_shader fs,
              int *zbuffer,
              void *shader)
{
    Vec3i bboxmin = {image->width - 1, image->height - 1, 0};
    Vec3i bboxmax = {0, 0, 0};
    Vec3i clamp = {image->width - 1, image->height - 1, 0};

    int i, j;
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 2; ++j) {
            bboxmin[j] = MAX(0,        MIN(bboxmin[j], pts[i][j]));
            bboxmax[j] = MIN(clamp[j], MAX(bboxmax[j], pts[i][j]));
        }
    }

    Vec3i P;
    for (P[0] = bboxmin[0]; P[0] <= bboxmax[0]; ++P[0]) {
        for (P[1] = bboxmin[1]; P[1] <= bboxmax[1]; ++P[1]) {
            Vec3 bc_screen;
            barycentric(&bc_screen, pts, &P);
            if (bc_screen[0] < 0.0 || bc_screen[1] < 0.0 || bc_screen[2] < 0.0) {
                continue;
            }
            Vec3 z_coords = {pts[0][2], pts[1][2], pts[2][2]};
            double z = dot_prod(&z_coords, &bc_screen);
            int idx = P[0] + P[1]*image->width;
            if (zbuffer[idx] > z) {
                continue;
            }
            zbuffer[idx] = z;
            tgaColor color;
            if ((*fs)(&color, &bc_screen, shader)) {
                tgaSetPixel(image, P[0], P[1], color);
            }
        }
    }
}


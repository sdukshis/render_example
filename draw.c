#include "draw.h"
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void triangle(Vec3i *p1, Vec3i *p2, Vec3i *p3,
              Vec3 *uv1, Vec3 *uv2, Vec3 *uv3,
              double intty1, double intty2, double intty3,
              tgaImage *image,
              int *zbuffer, Model *model)
{
    
    if ((*p1)[1] == (*p2)[1] && (*p1)[1] == (*p3)[1])
        return;
    if ((*p1)[1] > (*p2)[1]) {
        swap(p1, p2, sizeof(Vec3i));
        swap(&uv1, &uv2, sizeof(Vec3 *));
        swap(&intty1, &intty2, sizeof(double));
    }
    if ((*p1)[1] > (*p3)[1]) {
        swap(p1, p3, sizeof(Vec3i));
        swap(&uv1, &uv3, sizeof(Vec3 *));
        swap(&intty1, &intty3, sizeof(double));
    }
    if ((*p2)[1] > (*p3)[1]) {
        swap(p2, p3, sizeof(Vec3i));
        swap(&uv2, &uv3, sizeof(Vec3 *));
        swap(&intty2, &intty3, sizeof(double));
    }
    int total_heght = (*p3)[1] - (*p1)[1];
    int i;
    for (i = 0; i < total_heght; ++i) {
        int second_half = (i > (*p2)[1] - (*p1)[1]) || ((*p1)[1] == (*p2)[1]);
        int segment_height = second_half ? (*p3)[1] - (*p2)[1] : (*p2)[1] - (*p1)[1];
        double alpha = ((double)i) / total_heght;
        double beta = ((double)(i - (second_half ? (*p2)[1] - (*p1)[1] : 0))) / segment_height;
        Vec3i A, B;
        Vec3 Auv, Buv;
        double Aintty, Bintty;
        int k;
        for (k = 0; k < 3; ++k) {
            A[k] = (*p1)[k] + (int)((*p3)[k] - (*p1)[k]) * alpha;
            B[k] = second_half ? (*p2)[k] + (int)((*p3)[k] - (*p2)[k]) * beta :
                                 (*p1)[k] + (int)((*p2)[k] - (*p1)[k]) * beta;
            Auv[k] = (*uv1)[k] + ((*uv3)[k] - (*uv1)[k]) * alpha;
            Buv[k] = second_half ? (*uv2)[k] + ((*uv3)[k] - (*uv2)[k]) * beta :
                                   (*uv1)[k] + ((*uv2)[k] - (*uv1)[k]) * beta;
        }
        Aintty = intty1 + (intty3 - intty1) * alpha;
        Bintty = second_half ? intty2 + (intty3 - intty2) * beta :
                               intty1 + (intty2 - intty1) * beta;
        
        if (A[0] > B[0]) {
            swap(&A, &B, sizeof(Vec3i));
            swap(&Auv, &Buv, sizeof(Vec3));
            swap(&Aintty, &Bintty, sizeof(double));
        }
        int j;
        for (j = A[0]; j <= B[0]; ++j) {
            double phi = B[0] == A[0] ? 1.0 : (double)(j - A[0]) / (double)(B[0] - A[0]);
            Vec3i P;
            Vec3 uv;
            double intensity = Aintty + (Bintty - Aintty) * phi;
            if (intensity < 0.0)
                continue;
            for (k = 0; k < 3; ++k) {
                P[k] = A[k] + (B[k] - A[k]) * phi;
                uv[k] = Auv[k] + (Buv[k] - Auv[k]) * phi;
            }
            P[0] = j; P[1] = (*p1)[1] + i; // hack to fill holes (due to int cast precision problems)
            int idx = j + ((*p1)[1] + i) * image->width;
            if (idx < image->height * image->width && zbuffer[idx] < P[2]) {
                zbuffer[idx] = P[2];
                tgaColor color = getDiffuseColor(model, &uv);
                tgaSetPixel(image, P[0], P[1], tgaRGB(intensity * Red(color),
                                                      intensity * Green(color),
                                                      intensity * Blue(color)));
            }
        }
    }
}


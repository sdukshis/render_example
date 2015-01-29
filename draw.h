#ifndef DRAW_H_
#define DRAW_H_

#include "tga.h"
#include "model.h"

typedef int Vec3i[3];

void line(unsigned int x1, unsigned int y1,
          unsigned int x2, unsigned int y2,
          tgaImage *, tgaColor);

void triangle(Vec3i *p1, Vec3i *p2, Vec3i *p3,
              Vec3 *uv1, Vec3 *uv2, Vec3 *uv3,
              tgaImage *image, double intensity,
              int *zbuffer, Model *model);
#endif  // DRAW_H_

#ifndef DRAW_H_
#define DRAW_H_

#include "tga.h"
#include "model.h"

typedef int Vec3i[3];

void line(unsigned int x1, unsigned int y1,
          unsigned int x2, unsigned int y2,
          tgaImage *, tgaColor);

void triangle(Vec3i *pts,
              Vec3 *uv,
              Vec3 *intty,
              tgaImage *image,
              int *zbuffer, Model *model);
#endif  // DRAW_H_

#ifndef DRAW_H_
#define DRAW_H_

#include "tga.h"

typedef int Vec3i[3];

void line(unsigned int x1, unsigned int y1,
          unsigned int x2, unsigned int y2,
          tgaImage *, tgaColor);

void triangle(Vec3i *, Vec3i *, Vec3i *,
              tgaImage *, tgaColor, int *zbuffer);
#endif  // DRAW_H_

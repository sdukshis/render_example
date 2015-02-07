#ifndef DRAW_H_
#define DRAW_H_

#include "tga.h"
#include "geometry.h"

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

typedef int Vec3i[3];

typedef void (*vertex_shader)(Vec3 *sc, int face, int vert, void *);

typedef int (*fragment_shader)(tgaColor *, Vec3 *bar, void *);

void line(unsigned int x1, unsigned int y1,
          unsigned int x2, unsigned int y2,
          tgaImage *, tgaColor);

void triangle(Vec3 *pts,
              tgaImage *image,
              fragment_shader,
              int *zbuffer,
              void *);
#endif  // DRAW_H_

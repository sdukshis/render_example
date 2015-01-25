#ifndef DRAW_H_
#define DRAW_H_

#include "tga.h"

void line(unsigned int x1, unsigned int y1,
          unsigned int x2, unsigned int y2,
          tgaImage *, tgaColor);

#endif  // DRAW_H_

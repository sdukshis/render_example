#include "tga.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// struct tgaHeader {
//     unsigned char id_len,
//     unsigned
// };

tgaColor tgaRGB(unsigned char r, unsigned char g, unsigned char b)
{
    return (r << 24) | (g << 16) | (b << 8) | 0 /* 0 - alpha channel */;
}

tgaImage * tgaNewImage(unsigned int height, unsigned int width)
{
    assert(height && width); /* both must be greater then zero */

    tgaImage *image = (tgaImage *)malloc(sizeof(tgaImage));
    if (!image) {
        return NULL;
    }

    image->height = height;
    image->width = width;
    image->bpp = TRUE_COLOR_BPP;

    unsigned int data_size = image->height * image->width * image->bpp;
    image->data = (unsigned char *)malloc(data_size);
    if (!image->data) {
        free(image);
        return NULL;
    }

    bzero(image->data, data_size); /* TODO: explicit tgaColor? */

    return image;
}

void tgaFreeImage(tgaImage *image)
{
    assert(image);
    assert(image->data);

    free(image->data);
    free(image);
}

int tgaSetPixel(tgaImage *image, unsigned int x, unsigned int y, tgaColor c)
{
    assert(image);
    assert(x <= image->height);
    assert(y <= image->width);

    tgaColor *pixel_pos = (tgaColor *)(image->data + x * y * image->bpp);
    *pixel_pos = c;
    return 0;
}

tgaColor tgaGetPixel(tgaImage *image, unsigned int x, unsigned int y)
{
    assert(image);
    assert(x <= image->height);
    assert(y <= image->width);

    tgaColor *pixel_pos = (tgaColor *)(image->data + x * y * image->bpp);

    return *pixel_pos; 
}

int tgaSaveToFile(tgaImage *image, const char *filename)
{
    return 0;
}

tgaImage * tgaLoadFromFile(const char *filename)
{
    return NULL;
}

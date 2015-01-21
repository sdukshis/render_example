#ifndef TGA_H_
#define TGA_H_

#define TRUE_COLOR_BPP 4 /* R + B + G + A */

typedef struct tgaImage_t {
    unsigned int height;
    unsigned int width;
    unsigned char bpp; /* bytes per pixel */
    unsigned char *data; /* points on byte array with size height*width*bpp */
} tgaImage;

enum tgaFormat {
    GRAYSCALE = 1,
    RGB = 3,
    RGVA = 4
};

typedef unsigned int tgaColor;

tgaColor tgaRGB(unsigned char r, unsigned char g, unsigned char b);

tgaImage * tgaNewImage(unsigned int height, unsigned int width, int format);

void tgaFreeImage(tgaImage *);

int tgaSetPixel(tgaImage *, unsigned int x, unsigned int y, tgaColor);

tgaColor tgaGetPixel(tgaImage *, unsigned int x, unsigned int y);

int tgaSaveToFile(tgaImage *, const char *filename);

tgaImage * tgaLoadFromFile(const char *filename);

#endif  // TGA_H_

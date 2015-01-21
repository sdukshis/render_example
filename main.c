#include <stdio.h>
#include "tga.h"

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s outfile\n", argv[0]);
        return -1;
    }

    int rv = 0;

    tgaImage * image = tgaNewImage(200, 200, RGB);

    int i;
    for (i = 0; i < image->width / 2; ++i) {
        tgaSetPixel(image, i, i, tgaRGB(255, 255, 0));
    }

    if (-1 == tgaSaveToFile(image, argv[1])) {
        perror("tgaSateToFile");
        rv = -1;
    }
    tgaFreeImage(image);

    return rv;
}

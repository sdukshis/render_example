#include <stdio.h>
#include "tga.h"
#include "draw.h"

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s outfile\n", argv[0]);
        return -1;
    }

    int rv = 0;

    tgaImage * image = tgaNewImage(200, 200, RGB);

    line(100, 0, 101, 199, image, tgaRGB(255, 255, 255));

    if (-1 == tgaSaveToFile(image, argv[1])) {
        perror("tgaSateToFile");
        rv = -1;
    }
    tgaFreeImage(image);

    return rv;
}

#include <stdio.h>

#include "tga.h"

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return -1;
    }

    tgaImage *image = tgaLoadFromFile(argv[1]);

    tgaSaveToFile(image, argv[2]);
    
    tgaFreeImage(image);

    return 0;
}

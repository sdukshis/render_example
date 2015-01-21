#include "tga.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma pack(push, 1)
struct tgaHeader {
    unsigned char id_len;
    unsigned char color_map_type;
    unsigned char image_type;
    unsigned short color_map_idx;
    unsigned short color_map_len;
    unsigned char color_map_bpp;
    unsigned short image_pos_x;
    unsigned short image_pos_y;
    unsigned short image_width;
    unsigned short image_height;
    unsigned char image_bpp;
    unsigned char image_descriptor;
};
#pragma pack(pop)

tgaColor tgaRGB(unsigned char r, unsigned char g, unsigned char b)
{
    return 0 | (r << 16) | (g << 8) | (b << 0);
}

tgaImage * tgaNewImage(unsigned int height, unsigned int width, int format)
{
    assert(height && width); /* both must be greater then zero */

    tgaImage *image = (tgaImage *)malloc(sizeof(tgaImage));
    if (!image) {
        return NULL;
    }

    image->height = height;
    image->width = width;
    image->bpp = format;

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
    assert(x < image->height);
    assert(y < image->width);

    unsigned char *pixel_pos = (unsigned char *)(image->data + (x + y * image->width) * image->bpp);
    memcpy(pixel_pos, &c, image->bpp);
    return 0;
}

tgaColor tgaGetPixel(tgaImage *image, unsigned int x, unsigned int y)
{
    assert(image);
    assert(x < image->height);
    assert(y < image->width);

    unsigned char *pixel_pos = (unsigned char *)(image->data + (x + y * image->width) * image->bpp);

    return *(tgaColor *)pixel_pos; 
}

int tgaSaveToFile(tgaImage *image, const char *filename)
{
    assert(image);
    assert(filename);

    FILE *fd = fopen(filename, "wb");
    if (!fd) {
        return -1;
    }
    char new_tga_format_signature[] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    unsigned char extension_offset[] = { 0, 0, 0, 0};
    unsigned char developer_offset[] = { 0, 0, 0, 0};

    struct tgaHeader header;
    header.id_len = 0;
    header.color_map_type = 0; /* without ColorMap */ 
    header.image_type = (image->bpp == GRAYSCALE) ? 3 : 2;
    header.color_map_idx = 0;
    header.color_map_len = 0;
    header.color_map_bpp = 0;
    header.image_pos_x = 0;
    header.image_pos_y = 0;
    header.image_width = image->width;
    header.image_height = image->height;
    header.image_bpp = image->bpp << 3;
    header.image_descriptor = 0x20; /* top-left origin */

    if (-1 == fwrite(&header, sizeof(header), 1, fd)) {
        fclose(fd);
        return -1;
    }

    unsigned int data_size = image->height * image-> width * image->bpp;
    if (-1 == fwrite(image->data, data_size, 1, fd)) {
        fclose(fd);
        return -1;
    }

    if (-1 == fwrite(extension_offset, sizeof(extension_offset), 1, fd)) {
        fclose(fd);
        return -1;
    }

    if (-1 == fwrite(developer_offset, sizeof(developer_offset), 1, fd)) {
        fclose(fd);
        return -1;
    }

    if (-1 == fwrite(new_tga_format_signature, sizeof(new_tga_format_signature), 1, fd)) {
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}

tgaImage * tgaLoadFromFile(const char *filename)
{
    return NULL;
}

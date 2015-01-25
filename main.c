#include <stdio.h>
#include "tga.h"
#include "draw.h"
#include "model.h"

#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

void meshgrid(tgaImage *image, Model *model)
{
    assert(image);
    assert(model);
    int i;
    int h = image->height;
    int w = image->width;
    for (i = 0; i < model->nface; ++i) {
        Vec3 *v0 = &(model->vertices[model->faces[i][0]]);
        Vec3 *v1 = &(model->vertices[model->faces[i][3]]);
        Vec3 *v2 = &(model->vertices[model->faces[i][6]]);

        line(((*v0)[0] + 1) * w / 2,(1 - (*v0)[1]) * h / 2,
             ((*v1)[0] + 1) * w / 2,(1 - (*v1)[1]) * h / 2,
             image, tgaRGB(255, 255, 255));

        line(((*v1)[0] + 1) * w / 2,(1 - (*v1)[1]) * h / 2,
             ((*v2)[0] + 1) * w / 2,(1 - (*v2)[1]) * h / 2,
             image, tgaRGB(255, 255, 255));

        line(((*v2)[0] + 1) * w / 2,(1 - (*v2)[1]) * h / 2,
             ((*v0)[0] + 1) * w / 2,(1 - (*v0)[1]) * h / 2,
             image, tgaRGB(255, 255, 255));
    }
}

void rasterize(tgaImage *image, Model *model, int depth)
{
    assert(image);
    assert(model);
    int i;
    int h = image->height;
    int w = image->width;

    int *zbuffer = (int *)malloc(h * w * sizeof(int));
    for (i = 0; i < h * w; ++i) {
        zbuffer[i] = INT_MIN;
    }
    Vec3 light_dir = {0, 0, -1};

    for (i = 0; i < model->nface; ++i) {
        int j;
        Vec3 *world_coords[3];
        Vec3i screen_coords[3];
        for (j = 0; j < 3; ++j) {
            world_coords[j] = &(model->vertices[model->faces[i][3*j]]);
            screen_coords[j][0] = ((*world_coords[j])[0] + 1) * w / 2;
            screen_coords[j][1] = (1 - (*world_coords[j])[1]) * h / 2;
            screen_coords[j][2] = ((*world_coords[j])[2] + 1) * depth / 2;
        }

        Vec3 v01;
        sub_vec3(&v01, world_coords[1], world_coords[0]);
        Vec3 v02;
        sub_vec3(&v02, world_coords[2], world_coords[0]);
        Vec3 normale;
        vec_prod(&normale, &v02, &v01);
        normalize(&normale);

        double intensity = dot_prod(&normale, &light_dir);
        if (intensity > 0) {
            triangle(&screen_coords[0], &screen_coords[1], &screen_coords[2],
                     image,
                     tgaRGB(255 * intensity, 255 * intensity, 255 * intensity),
                     zbuffer);        
        }
    }

    tgaImage * zdump = tgaNewImage(h, w, RGB);
    int j;
    for (i = 0; i < w; ++i) {
        for (j = 0; j < h; ++j) {
            unsigned char color = UCHAR_MAX * (double)zbuffer[i + j*w]/(INT_MAX - INT_MIN);
            tgaSetPixel(zdump, i, j, tgaRGB(color, color, color));
        }
    }
    tgaSaveToFile(zdump, "zbuffer.tga");
    tgaFreeImage(zdump);
    free(zbuffer);
}

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <objfile> <outfile>\n", argv[0]);
        return -1;
    }

    Model *model = loadFromObj(argv[1]);
    if (!model) {
        perror("loadFromObj");
        return -1;
    }
    printf("model with %d vertices, %d faces loaded\n", model->nvert, model->nface);
    int rv = 0;

    int height = 800;
    int width = 800;

    tgaImage * image = tgaNewImage(height, width, RGB);

    // meshgrid(image, model);
    rasterize(image, model, /* depth = */ 255);

    if (-1 == tgaSaveToFile(image, argv[2])) {
        perror("tgaSateToFile");
        rv = -1;
    }
    tgaFreeImage(image);
    freeModel(model);

    return rv;
}

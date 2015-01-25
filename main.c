#include <stdio.h>
#include "tga.h"
#include "draw.h"
#include "model.h"

#include <assert.h>

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

    meshgrid(image, model);

    if (-1 == tgaSaveToFile(image, argv[2])) {
        perror("tgaSateToFile");
        rv = -1;
    }
    tgaFreeImage(image);
    freeModel(model);

    return rv;
}

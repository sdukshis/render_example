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

void identity(Mat4 *m)
{
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            setM(m, i, j, i == j ? 1.0 : 0.0);
        }
    }
}

void projection(Mat4 *m, double c)
{
    identity(m);
    setM(m, 3, 2, -1.0/c);
}

void viewport(Mat4 *m, unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height,
                       unsigned int depth)
{
    identity(m);
    setM(m, 0, 0, width / 2.0);
    setM(m, 1, 1, height / 2.0);
    setM(m, 2, 2, depth / 2.0);

    setM(m, 0, 3, x + width / 2.0);
    setM(m, 1, 3, -y + height / 2.0);
    setM(m, 2, 3, depth / 2.0);
}

void lookat(Mat4 *m, Vec3 *eye, Vec3 *center, Vec3 *up)
{
    Vec3 x, y, z;
    sub_vec3(&z, eye, center);
    normalize(&z);
    cross_prod(&x, up, &z);
    normalize(&x);
    cross_prod(&y, &z, &x);
    normalize(&y);

    identity(m);
    int i = 0;
    for (i = 0; i < 3; ++i) {
        setM(m, 0, i, x[i]);
        setM(m, 1, i, y[i]);
        setM(m, 2, i, z[i]);
        setM(m, i, 3, -(*center)[i]);
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
    Vec3 light_dir = {0, 0, -1.0};
    Vec3 eye = {1, 1, 3};
    Vec3 center = {0, 0, 0};
    Vec3 camera = {0, 0, 3.0};
    Vec3 up = {0, 1, 0};

    Mat4 Projection;
    Mat4 ViewPort;
    Mat4 ModelView;
    Mat4 Transform;
    projection(&Projection, camera[2]);
    // identity(&Projection);
    lookat(&ModelView, &eye, &center, &up);
    // identity(&ModelView);
    // projection(&ViewPort, 0, 0, width, height, depth);
    mulMM(&Transform, &ModelView, &Projection);

    for (i = 0; i < model->nface; ++i) {
        int j;
        Vec3 *vertex_coords;
        Vec4 coords;
        Vec4 proj_coords;
        Vec3i screen_coords[3];
        Vec3 world_coords[3];
        Vec3 uv[3];
        Vec3 intensity;
        for (j = 0; j < 3; ++j) {
            vertex_coords = getVertex(model, i, j);
            Vec3to4(&coords, vertex_coords);
            mulMV(&proj_coords, &Transform, &coords);
            Vec4to3(&world_coords[j], &proj_coords);
            screen_coords[j][0] = (world_coords[j][0] + 1) * w / 2;
            screen_coords[j][1] = (1 - world_coords[j][1]) * h / 2;
            screen_coords[j][2] = (world_coords[j][2] + 1) * depth / 2;
            cpy_vec3(&uv[j], getDiffuseUV(model, i, j));
            // TODO: remove norm
            Vec3 norm;
            Vec3 *n = getNorm(model, i, j);
            int k;
            for (k = 0; k < 3; ++k) {
                norm[k] = -(*n)[k];
            }
            // normalize(&norm);
            intensity[j] = dot_prod(&norm, &light_dir);
        }

        Vec3 v01;
        sub_vec3(&v01, &world_coords[1], &world_coords[0]);
        Vec3 v02;
        sub_vec3(&v02, &world_coords[2], &world_coords[0]);
        Vec3 normale;
        cross_prod(&normale, &v02, &v01);
        normalize(&normale);

        // double intensity = dot_prod(&normale, &light_dir);
        triangle(screen_coords,
                 uv,
                 &intensity,
                 image,
                 zbuffer,
                 model);
    }

    tgaImage * zdump = tgaNewImage(h, w, GRAYSCALE);
    int j;
    for (i = 0; i < w; ++i) {
        for (j = 0; j < h; ++j) {
            unsigned char color = UCHAR_MAX * (double)zbuffer[i + j*w]/(INT_MAX - INT_MIN);
            tgaSetPixel(zdump, i, j, color);
        }
    }
    tgaSaveToFile(zdump, "zbuffer.tga");
    tgaFreeImage(zdump);
    free(zbuffer);
}

int main(int argc, char const *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <objfile> <texture> <outfile>\n", argv[0]);
        return -1;
    }

    Model *model = loadFromObj(argv[1]);
    if (!model) {
        perror("loadFromObj");
        return -1;
    }
    if (!loadDiffuseMap(model, argv[2])) {
        perror("loadDiffuseMap");
        freeModel(model);
        return -1;
    }
    printf("model with %d vertices, %d faces loaded\n", model->nvert, model->nface);
    int rv = 0;

    int height = 800;
    int width = 800;

    tgaImage * image = tgaNewImage(height, width, RGB);

    // meshgrid(image, model);
    rasterize(image, model, /* depth = */ 255);

    if (-1 == tgaSaveToFile(image, argv[3])) {
        perror("tgaSateToFile");
        rv = -1;
    }
    tgaFreeImage(image);
    freeModel(model);

    return rv;
}

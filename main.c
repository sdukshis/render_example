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

        line(((*v0)[0] + 1) * w / 2,(1 + (*v0)[1]) * h / 2,
             ((*v1)[0] + 1) * w / 2,(1 + (*v1)[1]) * h / 2,
             image, tgaRGB(255, 255, 255));

        line(((*v1)[0] + 1) * w / 2,(1 + (*v1)[1]) * h / 2,
             ((*v2)[0] + 1) * w / 2,(1 + (*v2)[1]) * h / 2,
             image, tgaRGB(255, 255, 255));

        line(((*v2)[0] + 1) * w / 2,(1 + (*v2)[1]) * h / 2,
             ((*v0)[0] + 1) * w / 2,(1 + (*v0)[1]) * h / 2,
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

Vec3 light_dir = {0.0, 0.0, 1.0};
Vec3 eye = {1, 1, 3};
Vec3 center = {0, 0, 0};
Vec3 camera = {0, 0, 3.0};
Vec3 up = {0, 1, 0};

Mat4 Projection;
Mat4 ViewPort;
Mat4 ModelView;
Mat4 Transform;

typedef struct gouraud_shader_t {
    Model *model;
    Vec3 varying_intensity;
    Vec3 varying_uv[3];
} gouraud_shader;

void gouraud_vertex_shader(Vec3 *sc, int face, int vert, void *state)
{
    gouraud_shader *sh = (gouraud_shader *)state;
    sh->varying_intensity[vert] = MAX(0.0, dot_prod(getNorm(sh->model, face, vert),
                                                    &light_dir));
    cpy_vec3(&(sh->varying_uv[vert]), getDiffuseUV(sh->model, face, vert));
    
    Vec3 *vertex_coords = getVertex(sh->model, face, vert);
    Vec4 coords;
    Vec4 world_coords;
    Vec3to4(&coords, vertex_coords);
    mulMV(&world_coords, &Transform, &coords);
    Vec4to3(sc, &world_coords);
}

int gouraud_fragment_shader(tgaColor *color, Vec3 *bar, void *state)
{
    gouraud_shader *sh = (gouraud_shader *)state;
    double intensity = MIN(1.0 ,dot_prod(&(sh->varying_intensity), bar));
    Vec3 *uv = sh->varying_uv;
    Vec3 uvx = {uv[0][0], uv[1][0], uv[2][0]};
    Vec3 uvy = {uv[0][1], uv[1][1], uv[2][1]};
    Vec3 uv_coords = {dot_prod(&uvx, bar),
                      dot_prod(&uvy, bar),
                      0};
    tgaColor color_uv = getDiffuseColor(sh->model, &uv_coords);
    *color = tgaRGB(Red(color_uv) * intensity,
                    Green(color_uv) * intensity,
                    Blue(color_uv) * intensity);
    return 1;
}

void rasterize(tgaImage *image, Model *model,
               vertex_shader vs, fragment_shader fs,
               void *shader)
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

    for (i = 0; i < model->nface; ++i) {
        int j;
        Vec3 screen_coords[3];
        for (j = 0; j < 3; ++j) {
            (*vs)(&screen_coords[j], i, j, shader);
        }

        triangle(screen_coords,
                 image,
                 fs,
                 zbuffer,
                 shader
                 );
    }

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
    int depth = 255;

    projection(&Projection, camera[2]);
    lookat(&ModelView, &eye, &center, &up);
    viewport(&ViewPort, 0, 0, width, height, depth);
    Mat4 MP;
    mulMM(&MP, &ModelView, &Projection);
    mulMM(&Transform, &ViewPort, &MP);
    
    tgaImage * image = tgaNewImage(height, width, RGB);

    gouraud_shader shader;
    shader.model = model;
    // meshgrid(image, model);
    rasterize(image, model,
              gouraud_vertex_shader,
              gouraud_fragment_shader,
              (void*)&shader);

    tgaFlipVertically(image);
    if (-1 == tgaSaveToFile(image, argv[3])) {
        perror("tgaSateToFile");
        rv = -1;
    }
    tgaFreeImage(image);
    freeModel(model);

    return rv;
}

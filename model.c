#include "model.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

Model * loadFromObj(const char *filename)
{
    assert(filename);

    FILE *fd = fopen(filename, "r");
    if (!fd) {
        return NULL;
    }

    Model *model = (Model *)malloc(sizeof(Model));
    model->nvert = 0;
    model->ntext = 0;
    model->nnorm = 0;
    model->nface = 0;

    size_t vertcap = 1;
    size_t textcap = 1;
    size_t normcap = 1;
    size_t facecap = 1;

    model->vertices = (Vec3 *)malloc(vertcap * sizeof(Vec3));
    model->textures = (Vec3 *)malloc(textcap * sizeof(Vec3));
    model->normals = (Vec3 *)malloc(normcap * sizeof(Vec3));
    model->faces = (Face *)malloc(facecap * sizeof(Face));


    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fd)) > 0) {
        if (!strncmp(line, "vn", 2)) {
            if (model->nnorm >= normcap) { // realloc
                normcap *= 2;
                model->normals = (Vec3 *)realloc(model->normals, normcap * sizeof(Vec3));
                assert(model->normals);
            }
            Vec3 *vn = &model->normals[model->nnorm];
            assert(3 == sscanf(line + 2, "%lg %lg %lg\n", &(*vn)[0], &(*vn)[1], &(*vn)[2]));
            model->nnorm += 1;
        } else if (!strncmp(line, "vt", 2)) {
            if (model->ntext >= textcap) { // realloc
                textcap *= 2;
                model->textures = (Vec3 *)realloc(model->textures, textcap * sizeof(Vec3));
                assert(model->textures);
            }
            Vec3 *vt = &model->textures[model->ntext];
            assert(3 == sscanf(line + 2, "%lg %lg %lg\n", &(*vt)[0], &(*vt)[1], &(*vt)[2]));
            model->ntext += 1;
        } else if (!strncmp(line, "v", 1)) {
            if (model->nvert >= vertcap) { // realloc
                vertcap *= 2;
                model->vertices = (Vec3 *)realloc(model->vertices, vertcap * sizeof(Vec3));
                assert(model->vertices);
            }
            Vec3 *v = &model->vertices[model->nvert];
            assert(3 == sscanf(line + 1, "%lg %lg %lg\n", &(*v)[0], &(*v)[1], &(*v)[2]));
            model->nvert += 1;
        } else if (!strncmp(line, "f", 1)) {
            if (model->nface >= facecap) { // realloc
                facecap *= 2;
                model->faces = (Face *)realloc(model->faces, facecap * sizeof(Face));
                assert(model->faces);
            }
            Face *f = &model->faces[model->nface];
            assert(9 == sscanf(line + 1, "%u/%u/%u %u/%u/%u %u/%u/%u", &(*f)[0], &(*f)[1], &(*f)[2],
                                                                       &(*f)[3], &(*f)[4], &(*f)[5],
                                                                       &(*f)[6], &(*f)[7], &(*f)[8]));
            assert((*f)[0] <= model->nvert &&
                   (*f)[1] <= model->ntext &&
                   (*f)[2] <= model->nnorm);
            assert((*f)[3] <= model->nvert &&
                   (*f)[4] <= model->ntext &&
                   (*f)[5] <= model->nnorm);
            assert((*f)[6] <= model->nvert &&
                   (*f)[7] <= model->ntext &&
                   (*f)[8] <= model->nnorm);
            int i;
            for (i = 0; i < sizeof(Face)/sizeof(unsigned int); ++i) {
                (*f)[i] -= 1;
            }
            model->nface += 1;
        } else if (!strncmp(line, "#", 1) ||
                   !strncmp(line, "\n", 1)) {
            // skip comments and empty lines
        } else {
            fprintf(stderr, "Warning! Unsupported obj format: %s", line);
        }
    }

    if (line) {
        free(line);
    }
    fclose(fd);
    return model;
}

void freeModel(Model *model)
{
    assert(model);

    if (model->vertices) 
        free(model->vertices);
    if (model->textures)
        free(model->textures);
    if (model->normals)
        free(model->normals);
    if (model->faces)
        free(model->faces);
    free(model);
}

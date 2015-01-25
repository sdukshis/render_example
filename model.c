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
    model->nface = 0;

    size_t vertcap = 1;
    size_t facecap = 1;

    model->vertices = (Vec3 *)malloc(vertcap * sizeof(Vec3));
    model->faces = (Face *)malloc(facecap * sizeof(Face));


    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fd)) > 0) {
        if (!strncmp(line, "vn", 2)) {
            // skip normalas for now
        } else if (!strncmp(line, "vt", 2)) {
            // skip texture mapping for now
        } else if (!strncmp(line, "v", 1)) {
            if (model->nvert >= vertcap) { // realloc
                vertcap *= 2;
                model->vertices = (Vec3 *)realloc(model->vertices, vertcap * sizeof(Vec3));
                assert(model->vertices);
            }
            Vec3 *v = &model->vertices[model->nvert];
            int rv = sscanf(line + 1, "%lg %lg %lg\n", &(*v)[0], &(*v)[1], &(*v)[2]);
            if (rv != 3) {
                fprintf(stderr, "Error parsing line: %s", line);
                assert(0);
            }
            model->nvert += 1;
        } else if (!strncmp(line, "f", 1)) {
            if (model->nface >= facecap) { // realloc
                facecap *= 2;
                model->faces = (Face *)realloc(model->faces, facecap * sizeof(Face));
                assert(model->faces);
            }
            Face *f = &model->faces[model->nface];
            unsigned int ignore;
            assert(9 == sscanf(line + 1, "%u/%u/%u %u/%u/%u %u/%u/%u", &(*f)[0], &ignore, &ignore,
                                                                     &(*f)[1], &ignore, &ignore,
                                                                     &(*f)[2], &ignore, &ignore));
            assert((*f)[0] <= model->nvert &&
                   (*f)[1] <= model->nvert &&
                   (*f)[2] <= model->nvert);
            (*f)[0] -= 1;
            (*f)[1] -= 1;
            (*f)[2] -= 1;
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
    if (model->faces)
        free(model->faces);
    free(model);
}

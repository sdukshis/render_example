#ifndef MODEL_H_
#define MODEL_H_

#include "geometry.h"
#include "tga.h"

typedef unsigned int Face[9];

typedef struct Model {
    unsigned int nvert; // number of vertices
    unsigned int ntext; // number of texture coords
    unsigned int nnorm; // number of normals
    unsigned int nface; // number of faces
    Vec3 *vertices;
    Vec3 *textures;
    Vec3 *normals;
    Face *faces;
    tgaImage *diffuse_map;
    tgaImage *normal_map;
    tgaImage *specular_map;
} Model;

Model * loadFromObj(const char *filename);

int loadDiffuseMap(Model *model, const char *filename);
int loadNormalMap(Model *model, const char *filename);
int loadSpecularMap(Model *model, const char *filename);

void freeModel(Model *);

#endif // MODEL_H_

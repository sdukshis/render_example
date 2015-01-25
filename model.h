#ifndef MODEL_H_
#define MODEL_H_

typedef double Vec3[3];
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
} Model;

Model * loadFromObj(const char *filename);

void freeModel(Model *);

#endif // MODEL_H_

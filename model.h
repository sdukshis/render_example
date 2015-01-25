#ifndef MODEL_H_
#define MODEL_H_

typedef double Vec3[3];
typedef double Vec2[2];
typedef unsigned int Face[3];

typedef struct Model {
    unsigned int nvert; // number of vertices
    unsigned int nface; // number of faces
    Vec3 *vertices;
    Face *faces;
} Model;

Model * loadFromObj(const char *filename);

void freeModel(Model *);

#endif // MODEL_H_

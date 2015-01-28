#include "geometry.h"

#include <math.h>

double dot_prod(Vec3 *a, Vec3 *b)
{
    return (*a)[0]*(*b)[0] + (*a)[1]*(*b)[1] + (*a)[2]*(*b)[2];
}

void add_vec3(Vec3 *c, Vec3 *a, Vec3 *b)
{
    (*c)[0] = (*a)[0] + (*b)[0];
    (*c)[1] = (*a)[1] + (*b)[1];
    (*c)[2] = (*a)[2] + (*b)[2];
}

void sub_vec3(Vec3 *c, Vec3 *a, Vec3 *b)
{
    (*c)[0] = (*a)[0] - (*b)[0];
    (*c)[1] = (*a)[1] - (*b)[1];
    (*c)[2] = (*a)[2] - (*b)[2];
}

void cross_prod(Vec3 *c, Vec3 *a, Vec3 *b)
{
    (*c)[0] = (*a)[1]*(*b)[2] - (*a)[2]*(*b)[1];
    (*c)[1] = (*a)[2]*(*b)[0] - (*a)[0]*(*b)[2];
    (*c)[2] = (*a)[0]*(*b)[1] - (*a)[1]*(*b)[0];
}

void normalize(Vec3 *v)
{
    double len = sqrt(dot_prod(v, v));
    if (len == 0.0)
        return;

    (*v)[0] /= len;
    (*v)[1] /= len;
    (*v)[2] /= len;
}

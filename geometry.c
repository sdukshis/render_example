#include "geometry.h"

#include <math.h>
#include <assert.h>

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

void Vec3to4(Vec4 *dst, Vec3 *src)
{
    assert(dst);
    assert(src);
    (*dst)[0] = (*src)[0];
    (*dst)[1] = (*src)[1];
    (*dst)[2] = (*src)[2];
    (*dst)[3] = 1.0;
}

void Vec4to3(Vec3 *dst, Vec4 *src)
{
    assert(dst);
    assert(src);
    assert((*src)[3]); // infinity point
    (*dst)[0] = (*src)[0] / (*src)[3];
    (*dst)[1] = (*src)[1] / (*src)[3];
    (*dst)[2] = (*src)[2] / (*src)[3];
}

void mulMM(Mat4 *c, Mat4 *a, Mat4 *b)
{
    assert(c);
    assert(a);
    assert(b);
    int i, j, k;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            double r = 0.0;
            for (k = 0; k < 4; ++k) {
                r += getM(a, i, k) * getM(b, k, j);
            }
            setM(c, i, j, r);
        }
    }
}

void mulMV(Vec4 *c, Mat4 *a, Vec4 *b)
{
    assert(c);
    assert(a);
    assert(b);
    int i, j;
    for (i = 0; i < 4; ++i) {
        double r = 0.0;
        for (j = 0; j < 4; ++j) {
            r += getM(a, i, j) * (*b)[j];
        }
        (*c)[i] = r;
    }
}

void setM(Mat4 *m, unsigned int i, unsigned int j, double val)
{
    assert(m);
    assert(i < 4);
    assert(j < 4);
    (*m)[j + 4 * i] = val;
}

double getM(Mat4 *m, unsigned int i, unsigned int j)
{
    assert(m);
    assert(i < 4);
    assert(j < 4);
    return (*m)[j + 4 * i];
}

#include "geometry.h"

#include <math.h>
#include <assert.h>
#include <string.h>

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

void cpy_vec3(Vec3 *a, Vec3 *b)
{
    (*a)[0] = (*b)[0];
    (*a)[1] = (*b)[1];
    (*a)[2] = (*b)[2];
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

void LU(Mat4 *A, unsigned int *P, int *signum)
{
    int i, j, k;
    *signum = 1;
    int N = 4;
    for (i = 0; i < N; ++i) {
        P[i] = i;
    }

    for (j = 0; j < N -1 ; ++j) {
        double max = fabs(getM(A, j, j));
        int i_pivot = j;

        for (i = j + 1; i < N; ++i) {
            double ajj = fabs(getM(A, i, j));
            if (ajj > max) {
                max = ajj;
                i_pivot = i;
            }
        }

        if (i_pivot != j) {
            for (i = 0; i < N; ++i) {
                double tmp = getM(A, j, i);
                setM(A, j, i, getM(A, i_pivot, i));
                setM(A, i_pivot, i, tmp);
            }

            int tmpi = P[j];
            P[j] = P[i_pivot];
            P[i_pivot] = tmpi;
        }

        double ajj = getM(A, j, j);
        if (ajj != 0.0) {
            for (i = j + 1; i < N; ++i) {
                setM(A, i, j, getM(A, i, j)/ajj);
                double aij = getM(A, i, j);
                for (k = j + 1; k < N; ++k) {
                    double aik = getM(A, i, k);
                    double ajk = getM(A, j, k);
                    setM(A, i, k, aik - aij * ajk);
                }
            }
        }
    }
}

void LU_solve(Mat4 *LU, Vec4 *x)
{
    int N = 4;
    int i, j;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < i; ++j) {
            (*x)[i] -= getM(LU, i, j) * (*x)[j];
        }
    }
    (*x)[N - 1] /= getM(LU, N - 1, N - 1);

    for (i = N - 2; i >= 0; --i) {
        for (j = N -1; j > i; --j) {
            (*x)[i] -= getM(LU, i, j)*(*x)[j];
        }
        (*x)[i] /=getM(LU, i, i);
    }
}

void shr(Vec4 *b)
{
    int i;
    for (i = 3; i > 0; --i) {
        (*b)[i] = (*b)[i - 1];
    }
    (*b)[0] = 0.0;
}

int inverse(Mat4 *invm, Mat4 *m)
{
    Mat4 lu;
    memcpy(lu, *m, sizeof(Mat4));
    unsigned int P[4] = {0, 0, 0, 0};
    int signum = 0;
    LU(&lu, P, &signum);

    Vec4 b = {1.0, 0.0, 0.0, 0.0};
    Vec4 bn;
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            bn[j] = b[P[j]];
        }
        LU_solve(&lu, &bn);
        for (j = 0; j < 4; ++j) {
            setM(invm, j, i, bn[j]);
        }

        shr(&b);
    }
    return 1;
}


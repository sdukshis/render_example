#include <stdio.h>
#include <float.h>
#include <assert.h>
#include <math.h>

#include "geometry.h"

void assert_eq_mat4(Mat4 *a, Mat4 *b)
{
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            if (fabs(getM(a, i, j) - getM(b, i, j)) > 1e-6) {
                fprintf(stderr, "Expect %lf, actual %lf\n", getM(a, i, j),
                                                            getM(b, i, j));
                assert(0);
            }
        }
    }
}

void print_mat(Mat4 *a)
{
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            printf("%6.3g ", getM(a, i, j));
        }
        printf("\n");
    }
}
void test_lu() 
{
    printf("Run test_lu...");
    Mat4 A = {1.0, 2.0, 3.0, 4.0,
              5.0, 6.0, 7.0, 8.0,
              9.0, 1.0, 2.0, 3.0,
              1.0, 2.0, 6.0, 4.0
              };

    unsigned int P[4] = {0, 0, 0, 0};
    int signum = 0;
    LU(&A, P, &signum);

    Mat4 lu = {9.0,        1.0,        2.0,        3.0,
               0.55555556, 5.44444444, 5.88888889, 6.33333333,
               0.11111111, 0.34693878, 3.73469388, 1.46938776,
               0.11111111, 0.34693878, 0.19672131, 1.18032787
              };

    assert_eq_mat4(&lu, &A);
    printf("[Ok]\n");
}

void test_inv()
{
    Mat4 A = {1.0, 2.0, 3.0, 4.0,
              5.0, 6.0, 7.0, 8.0,
              9.0, 1.0, 2.0, 3.0,
              1.0, 2.0, 6.0, 4.0
              };
    Mat4 invA;

    inverse(&invA, &A);

    Mat4 expect = {
-1.38888889e-01,   2.77777778e-02,   1.11111111e-01,
         -3.46944695e-18,
-6.25000000e-01,   4.58333333e-01,  -1.66666667e-01,
         -1.66666667e-01,
-3.33333333e-01,   0.00000000e+00,   0.00000000e+00,
          3.33333333e-01,
8.47222222e-01,  -2.36111111e-01,   5.55555556e-02,
         -1.66666667e-01
    };

    printf("Run test_inv...");
    assert_eq_mat4(&expect, &invA);
    printf("[Ok]\n");

}

int main()
{
    test_lu();
    test_inv();
    return 0;
}

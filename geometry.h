#ifndef GEOMETRY_H_
#define GEOMETRY_H_

typedef double Vec3[3];
typedef double Vec4[4];
typedef double Mat4[16];

void add_vec3(Vec3 *c, Vec3 *a, Vec3 *b);

void sub_vec3(Vec3 *c, Vec3 *a, Vec3 *b);

double dot_prod(Vec3 *a, Vec3 *b);

void cross_prod(Vec3 *c, Vec3 *a, Vec3 *b);

void normalize(Vec3 *v);

void Vec3to4(Vec4 *, Vec3 *);

void Vec4to3(Vec3 *, Vec4 *);

void mulMM(Mat4 *c, Mat4 *a, Mat4 *b);

void mulMV(Vec4 *c, Mat4 *a, Vec4 *b);

void setM(Mat4 *m, unsigned int i, unsigned int j, double val);

double getM(Mat4 *m, unsigned int i, unsigned int j);

#endif  // GEOMETRY_H_

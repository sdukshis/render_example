#ifndef GEOMETRY_H_
#define GEOMETRY_H_

typedef double Vec3[3];

void add_vec3(Vec3 *c, Vec3 *a, Vec3 *b);

void sub_vec3(Vec3 *c, Vec3 *a, Vec3 *b);

double dot_prod(Vec3 *a, Vec3 *b);

void cross_prod(Vec3 *c, Vec3 *a, Vec3 *b);

void normalize(Vec3 *v);

#endif  // GEOMETRY_H_

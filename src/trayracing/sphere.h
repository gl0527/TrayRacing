#ifndef TRAYRACING_SPHERE_H
#define TRAYRACING_SPHERE_H

#include "vec3.h"
#include "ray.h"
#include "material.h"
#include "hit.h"

typedef struct Sphere {
    Vec3 center;
    float radius;
    Material *material;
} Sphere;

static Hit intersect(Sphere sphere, Ray ray)
{
    Hit hit;
    hit.t = -1.0f;

    Vec3 const dist = sub(ray.origin, sphere.center);
    float const b = dot(dist, ray.direction) * 2;
    float const c = lengthSqr(dist) - sphere.radius * sphere.radius;

    float const disc = b * b - 4 * c;
    if (disc < 0) {
        return hit;
    }

    float const sqrt_disc = sqrtf(disc);
    float const t1 = -b + sqrt_disc;
    float const t2 = -b - sqrt_disc;
    if (t1 < PRECISION) {
        return hit;
    }

    hit.t = t2 > PRECISION ? t2 * 0.5f : t1 * 0.5f;
    hit.position = add(ray.origin, mulf(hit.t, ray.direction));
    hit.normal = mulf(1.0f / sphere.radius, sub(hit.position, sphere.center));
    hit.material = sphere.material;

    return hit;
}

#endif

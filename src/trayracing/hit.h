#ifndef TRAYRACING_HIT_H
#define TRAYRACING_HIT_H

#include "vec3.h"
#include "material.h"

typedef struct Hit {
    float t;
    Vec3 position;
    Vec3 normal;
    Material const *material;
} Hit;

#endif

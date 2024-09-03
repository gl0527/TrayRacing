#ifndef TRAYRACING_LIGHT_H
#define TRAYRACING_LIGHT_H

#include "vec3.h"

typedef struct Light {
    Vec3 direction;
    Vec3 exitance;
} Light;

#endif

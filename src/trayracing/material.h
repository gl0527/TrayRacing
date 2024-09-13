#ifndef TRAYRACING_MATERIAL_H
#define TRAYRACING_MATERIAL_H

#include "vec3.h"

typedef struct Material {
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;
} Material;

static Vec3 shade(Material const *const material, Vec3 normal, Vec3 toEye, Vec3 toLight, Vec3 inRadiance)
{
    Vec3 outRadiance = {0.0f, 0.0f, 0.0f};
    float const NdotL = dot(normal, toLight);
    if (NdotL < 0)
    {
        return outRadiance;
    }
    outRadiance = mulf(NdotL, mul(inRadiance, material->diffuse));
    Vec3 const halfway = norm(add(toEye, toLight));
    float const NdotH = dot(normal, halfway);
    if (NdotH < 0)
    {
        return outRadiance;
    }

    return add(outRadiance, mulf(powf(NdotH, material->shininess), mul(inRadiance, material->specular)));
}

#endif

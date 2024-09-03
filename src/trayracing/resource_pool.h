#ifndef TRAYRACING_RESOURCE_H
#define TRAYRACING_RESOURCE_H

#include "material.h"

#include <stdint.h>

typedef struct ResourcePool {
    enum {MAX_MATERIAL_COUNT = 10} sizes;
    uint8_t currentMaterialCount;
    Material materials[MAX_MATERIAL_COUNT];
} ResourcePool;

static void set(ResourcePool *const pResourcePool)
{
    pResourcePool->currentMaterialCount = 0;
}

static void addMaterial(ResourcePool *const pResourcePool, Material material)
{
    if (pResourcePool->currentMaterialCount < MAX_MATERIAL_COUNT) {
        pResourcePool->materials[pResourcePool->currentMaterialCount++] = material;
    }
}

#endif

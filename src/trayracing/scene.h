#ifndef TRAYRACING_SCENE_H
#define TRAYRACING_SCENE_H

#include "sphere.h"
#include "light.h"
#include "camera.h"
#include "hit.h"
#include "material.h"

#include <stdint.h>

typedef struct Scene {
    enum {MAX_SPHERE_COUNT = 10, MAX_LIGHT_COUNT = 4} sizes;
    uint8_t currentSphereCount;
    uint8_t currentLightCount;
    Sphere spheres[MAX_SPHERE_COUNT];
    Light lights[MAX_LIGHT_COUNT];
    Camera camera;
    Vec3 ambientLight;
} Scene;

static Scene create(Vec3 eye, Vec3 up, Vec3 lookat, float fov, Vec3 La)
{
    Camera cam;
    SetUp(&cam, eye, lookat, up, fov);

    Scene scene;

    scene.currentSphereCount = 0;
    scene.currentLightCount = 0;
    scene.camera = cam;
    scene.ambientLight = La;

    return scene;
}

static void addSphere(Scene *const scene, Sphere sphere)
{
    if (scene->currentSphereCount < MAX_SPHERE_COUNT) {
        scene->spheres[scene->currentSphereCount++] = sphere;
    }
}

static void addLight(Scene *const scene, Light light)
{
    if (scene->currentLightCount < MAX_LIGHT_COUNT) {
        scene->lights[scene->currentLightCount++] = light;
    }
}

static Hit firstIntersect(Scene const *const scene, Ray ray)
{
    Hit bestHit;
    bestHit.t = -1.0f;
    for (uint8_t i = 0; i < scene->currentSphereCount; ++i)
    {
        Hit hit = intersect(scene->spheres[i], ray);
        if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
        {
            bestHit = hit;
        }
    }

    return bestHit;
}

static Vec3 trace(Scene const *const scene, Ray ray)
{
    Hit hit = firstIntersect(scene, ray);
    if (hit.t < 0) {
        return scene->ambientLight;
    }

    Vec3 outRadiance = mul(hit.material->ambient, scene->ambientLight);

    for (uint8_t i = 0; i < scene->currentLightCount; ++i)
    {
        outRadiance = add(outRadiance, shade(hit.material, hit.normal, inv(ray.direction), scene->lights[i].direction, scene->lights[i].exitance));
    }

    return outRadiance;
}

static void render(Scene const *const scene, Vec3 *const image, uint32_t imageWidth, uint32_t imageHeight)
{
    for (uint32_t y = 0; y < imageHeight; ++y)
    {
        for (uint32_t x = 0; x < imageWidth; ++x)
        {
            image[y * imageWidth + x] = trace(scene, GetRay(&(scene->camera), x, y, imageWidth, imageHeight));
        }
    }
}

#endif

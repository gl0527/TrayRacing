#ifndef TRAYRACING_CAMERA_H
#define TRAYRACING_CAMERA_H

#include "ray.h"

typedef struct Camera {
    Vec3 eye;
    Vec3 lookat;
    Vec3 right;
    Vec3 up;
} Camera;

static void SetUp(Camera *const camera, Vec3 eye, Vec3 lookat, Vec3 up, float fov)
{
    camera->eye = eye;
    camera->lookat = lookat;
    Vec3 const w = sub(eye, camera->lookat);
    float const windowSize = length(w) * tanf(fov * 0.5f);
    camera->right = mulf(windowSize, norm(cross(up, w)));
    camera->up = mulf(windowSize, norm(cross(w, camera->right)));
}

static Ray GetRay(Camera const *const camera, int x, int y, int screenWidth, int screenHeight)
{
    Vec3 const dir = sub(add(add(camera->lookat, mulf((2.0f * (x + 0.5f) / screenWidth - 1), camera->right)), mulf((2.0f * (y + 0.5f) / screenHeight - 1), camera->up)), camera->eye);
    Ray const ray = {.origin = camera->eye, .direction = norm(dir)};

    return ray;
}

#endif

#ifndef TRAYRACING_H
#define TRAYRACING_H

#include <stdint.h>

#ifndef TRAYRACING_DECL
#ifdef TRAYRACING_STATIC
#define TRAYRACING_DECL static
#else
#define TRAYRACING_DECL extern
#endif
#endif

#ifndef PRECISION
#define PRECISION 1e-7f
#endif

#ifndef M_PIf
#define M_PIf 3.141593f
#endif

#ifndef DEG2RAD
#define DEG2RAD (M_PIf/180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f/M_PIf)
#endif

#define BIT(n) (1ULL << (n))

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

typedef struct Light {
    Vec3 direction;
    Vec3 exitance;
} Light;

typedef struct Ray {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct Camera {
    Vec3 eye;
    Vec3 lookat;
    Vec3 right;
    Vec3 up;
} Camera;

typedef enum MaterialType
{
    MT_ROUGH,
    MT_SMOOTH
} MaterialType;

typedef enum SmoothMaterialAttribute
{
    SMTA_REFLECTIVE = BIT(0),
    SMTA_REFRACTIVE = BIT(1)
} SmoothMaterialAttribute;

typedef struct Material
{
    MaterialType type;
    Vec3 ambient;
    union {
        struct {
            Vec3 diffuse;
            Vec3 specular;
            float shininess;
        };
        struct {
            Vec3 minReflectance;
            Vec3 refrIdx;
            Vec3 absorpCoeff;
            uint8_t attributeMask;
        };
    };
} Material;

typedef struct Hit {
    float t;
    Vec3 position;
    Vec3 normal;
    Material const *material;
} Hit;

typedef struct Sphere {
    Vec3 center;
    float radius;
    Material const *material;
} Sphere;

typedef struct ResourcePool {
    enum {MAX_MATERIAL_COUNT = 10} sizes;
    uint8_t currentMaterialCount;
    Material materials[MAX_MATERIAL_COUNT];
} ResourcePool;

typedef struct Scene {
    enum {MAX_SPHERE_COUNT = 128, MAX_LIGHT_COUNT = 4} sizes;
    uint8_t currentSphereCount;
    uint8_t currentLightCount;
    Sphere spheres[MAX_SPHERE_COUNT];
    Light lights[MAX_LIGHT_COUNT];
    Camera camera;
    Vec3 ambientLight;
} Scene;

#ifdef __cplusplus
extern "C" {
#endif

TRAYRACING_DECL Vec3 inv(Vec3 a);
TRAYRACING_DECL Vec3 add(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 sub(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 mulf(float f, Vec3 a);
TRAYRACING_DECL Vec3 mul(Vec3 a, Vec3 b);
TRAYRACING_DECL float dot(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 cross(Vec3 a, Vec3 b);
TRAYRACING_DECL float lengthSqr(Vec3 a);
TRAYRACING_DECL float length(Vec3 a);
TRAYRACING_DECL Vec3 norm(Vec3 a);
TRAYRACING_DECL float dist(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 reflect(Vec3 n, Vec3 v);
TRAYRACING_DECL Vec3 refract(Vec3 n, Vec3 i, Vec3 refrIdx);

TRAYRACING_DECL void SetUp(Camera *const camera, Vec3 eye, Vec3 lookat, Vec3 up, float fov);
TRAYRACING_DECL Ray GetRay(Camera const *const camera, uint32_t x, uint32_t y, uint32_t screenWidth, uint32_t screenHeight);

TRAYRACING_DECL void createRoughMaterial(Material *const material, Vec3 ambient, Vec3 diffuse, Vec3 specular, float shininess);
TRAYRACING_DECL void createSmoothMaterial(Material *const material, Vec3 ambient, Vec3 refrIdx, Vec3 absorption, uint8_t attributes);
TRAYRACING_DECL Vec3 shade(Material const *const material, Vec3 normal, Vec3 toEye, Vec3 toLight, Vec3 inRadiance);

TRAYRACING_DECL Hit intersect(Sphere const *const sphere, Ray const *const ray);

TRAYRACING_DECL void set(ResourcePool *const pResourcePool);
TRAYRACING_DECL void addMaterial(ResourcePool *const pResourcePool, Material material);

TRAYRACING_DECL Scene create(Vec3 eye, Vec3 up, Vec3 lookat, float fov, Vec3 La);
TRAYRACING_DECL void addSphere(Scene *const scene, Sphere sphere);
TRAYRACING_DECL void addLight(Scene *const scene, Light light);
TRAYRACING_DECL Hit firstIntersect(Scene const *const scene, Ray const *const ray);
TRAYRACING_DECL Vec3 trace(Scene const *const scene, Ray const *const ray, uint8_t depth);
TRAYRACING_DECL void render(Scene const *const scene, Vec3 *const image, uint32_t imageWidth, uint32_t imageHeight);

#ifdef __cplusplus
}
#endif

#endif // TRAYRACING_H

#ifdef TRAYRACING_IMPLEMENTATION

#define _USE_MATH_DEFINES
#include <math.h>

Vec3 inv(Vec3 a)
{
    return (Vec3){-a.x, -a.y, -a.z};
}

Vec3 add(Vec3 a, Vec3 b)
{
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 mulf(float f, Vec3 a)
{
    return (Vec3){f * a.x, f * a.y, f * a.z };
}

Vec3 mul(Vec3 a, Vec3 b)
{
    return (Vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

float dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(Vec3 a, Vec3 b)
{
    return (Vec3){  a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x};
}

float lengthSqr(Vec3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float length(Vec3 a)
{
    return sqrtf(lengthSqr(a));
}

Vec3 norm(Vec3 a)
{
    float const len = length(a);

    return (len > PRECISION) ? mulf(1.0f / len, a) : a;
}

float dist(Vec3 a, Vec3 b)
{
    return length(sub(b, a));
}

Vec3 reflect(Vec3 n, Vec3 v) 
{
    return sub(v, mulf(2.0f,  mulf(dot(n, v),  n)));
}

Vec3 refract(Vec3 n, Vec3 i, Vec3 refrIdx)
{
    float cosa = dot(n, i);
    if (cosa > 0) {
        cosa = -cosa;
        n = inv(n);
        refrIdx.x = 1.0f / refrIdx.x;
        refrIdx.y = 1.0f / refrIdx.y;
        refrIdx.z = 1.0f / refrIdx.z;
    }
    float const num = 1.0f - cosa * cosa;
    float const discX = 1.0f - num / (refrIdx.x * refrIdx.x);
    float const discY = 1.0f - num / (refrIdx.y * refrIdx.y);
    float const discZ = 1.0f - num / (refrIdx.z * refrIdx.z);
    if (discX < 0.0f || discY < 0.0f || discZ < 0.0f)
    {
        return reflect(n, i);
    }

    return add(add(
                add(mulf(1.0f / refrIdx.x, i), mulf(cosa / refrIdx.x - sqrtf(discX), n)),
                add(mulf(1.0f / refrIdx.y, i), mulf(cosa / refrIdx.y - sqrtf(discY), n))),
                add(mulf(1.0f / refrIdx.z, i), mulf(cosa / refrIdx.z - sqrtf(discZ), n)));
}

void SetUp(Camera *const camera, Vec3 eye, Vec3 lookat, Vec3 up, float fov)
{
    camera->eye = eye;
    camera->lookat = lookat;
    Vec3 const w = sub(eye, camera->lookat);
    float const windowSize = length(w) * tanf(fov * 0.5f);
    camera->right = mulf(windowSize, norm(cross(up, w)));
    camera->up = mulf(windowSize, norm(cross(w, camera->right)));
}

Ray GetRay(Camera const *const camera, uint32_t x, uint32_t y, uint32_t screenWidth, uint32_t screenHeight)
{
    Vec3 const dir = sub(add(add(camera->lookat, mulf((2.0f * (x + 0.5f) / screenWidth - 1), camera->right)), mulf((2.0f * (y + 0.5f) / screenHeight - 1), camera->up)), camera->eye);
    return (Ray){.origin = camera->eye, .direction = norm(dir)};
}

void createRoughMaterial(Material *const material, Vec3 ambient, Vec3 diffuse, Vec3 specular, float shininess)
{
    material->type = MT_ROUGH;
    material->ambient = ambient;
    material->diffuse = diffuse;
    material->specular = specular;
    material->shininess = shininess;
}

void createSmoothMaterial(Material *const material, Vec3 ambient, Vec3 refrIdx, Vec3 absorpCoeff, uint8_t attributeMask)
{
    material->type = MT_SMOOTH;
    material->ambient = ambient;
    material->refrIdx = refrIdx;
    material->absorpCoeff = absorpCoeff;
    material->attributeMask = attributeMask;

    Vec3 const white = {1.0f, 1.0f, 1.0f};
    Vec3 const refrIdxP1 = add(refrIdx, white);
    Vec3 const refrIdxM1 = sub(refrIdx, white);
    Vec3 const absorpCoeff2 = mul(material->absorpCoeff, material->absorpCoeff);
    Vec3 const num = add(mul(refrIdxM1,  refrIdxM1), absorpCoeff2);
    Vec3 const denom = add(mul(refrIdxP1,  refrIdxP1), absorpCoeff2);

    material->minReflectance = (Vec3){num.x / denom.x, num.y / denom.y, num.z / denom.z};
}

Vec3 shade(Material const *const material, Vec3 normal, Vec3 toEye, Vec3 toLight, Vec3 inRadiance)
{
    switch (material->type)
    {
        case MT_ROUGH:
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
        case MT_SMOOTH:
            {
                float const cosa = fabsf(dot(normal, toEye));
                Vec3 const white = {1.0f, 1.0f, 1.0f};

                return add(material->minReflectance, mulf(powf(1.0f - cosa, 5), sub(white, material->minReflectance)));
            }
    }
}

Hit intersect(Sphere const *const sphere, Ray const *const ray)
{
    Hit hit;
    hit.t = -1.0f;

    Vec3 const dist = sub(ray->origin, sphere->center);
    float const b = dot(dist, ray->direction) * 2;
    float const c = lengthSqr(dist) - sphere->radius * sphere->radius;

    float const disc = b * b - 4 * c;
    if (disc < 0) {
        return hit;
    }

    float const sqrt_disc = sqrtf(disc);
    float const t1 = -b + sqrt_disc;
    if (t1 > PRECISION) {
        float const t2 = -b - sqrt_disc;
        hit.t = t2 > PRECISION ? t2 * 0.5f : t1 * 0.5f;
        hit.position = add(ray->origin, mulf(hit.t, ray->direction));
        hit.normal = mulf(1.0f / sphere->radius, sub(hit.position, sphere->center));
        hit.material = sphere->material;
    }

    return hit;
}

void set(ResourcePool *const pResourcePool)
{
    pResourcePool->currentMaterialCount = 0;
}

void addMaterial(ResourcePool *const pResourcePool, Material material)
{
    if (pResourcePool->currentMaterialCount < MAX_MATERIAL_COUNT) {
        pResourcePool->materials[pResourcePool->currentMaterialCount++] = material;
    }
}

Scene create(Vec3 eye, Vec3 up, Vec3 lookat, float fov, Vec3 La)
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

void addSphere(Scene *const scene, Sphere sphere)
{
    if (scene->currentSphereCount < MAX_SPHERE_COUNT) {
        scene->spheres[scene->currentSphereCount++] = sphere;
    }
}

void addLight(Scene *const scene, Light light)
{
    if (scene->currentLightCount < MAX_LIGHT_COUNT) {
        scene->lights[scene->currentLightCount++] = light;
    }
}

Hit firstIntersect(Scene const *const scene, Ray const *const ray)
{
    Hit bestHit;
    bestHit.t = -1.0f;
    for (uint8_t i = 0; i < scene->currentSphereCount; ++i)
    {
        Hit const hit = intersect(&(scene->spheres[i]), ray);
        if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
        {
            bestHit = hit;
        }
    }

    return bestHit;
}

Vec3 trace(Scene const *const scene, Ray const *const ray, uint8_t depth)
{
    if (depth > 3)
    {
        return scene->ambientLight;
    }

    Hit const hit = firstIntersect(scene, ray);
    if (hit.t < 0)
    {
        return scene->ambientLight;
    }

    Vec3 outRadiance = mul(hit.material->ambient, scene->ambientLight);
    Vec3 const viewDir = inv(ray->direction);

    switch(hit.material->type)
    {
        case MT_ROUGH:
            {
                for (uint8_t i = 0; i < scene->currentLightCount; ++i)
                {
                    Vec3 const toLight = norm(inv(scene->lights[i].direction));
                    Ray const shadowRay = {
                        .origin = add(hit.position, mulf(1e-3f, toLight)),
                        .direction = toLight
                    };
                    Hit const shadowHit = firstIntersect(scene, &shadowRay);
                    if (shadowHit.t < 0)
                    {
                        outRadiance = add(outRadiance, shade(hit.material, hit.normal, viewDir, toLight, scene->lights[i].exitance));
                    }
                }
            }
            break;
        case MT_SMOOTH:
            {
                Vec3 const reflectance = shade(hit.material, hit.normal, viewDir, (Vec3){0.0f ,0.0f ,0.0f}, (Vec3){0.0f, 0.0f, 0.0f});

                if (hit.material->attributeMask & SMTA_REFLECTIVE)
                {
                    Vec3 const reflectedDirection = norm(reflect(hit.normal, ray->direction));
                    Ray const reflectedRay = {
                        .origin = add(hit.position, mulf(1e-3f, reflectedDirection)),
                        .direction = reflectedDirection
                    };
                    outRadiance = add(outRadiance, mul(reflectance, trace(scene, &reflectedRay, depth + 1)));
                }
                if (hit.material->attributeMask & SMTA_REFRACTIVE)
                {
                    Vec3 const refractedDirection = norm(refract(hit.normal, ray->direction, hit.material->refrIdx));
                    Ray const refractedRay = {
                        .origin = add(hit.position, mulf(1e-3f, refractedDirection)),
                        .direction = refractedDirection
                    };
                    Vec3 const white = {1.0f, 1.0f, 1.0f};
                    outRadiance = add(outRadiance, mul(sub(white, reflectance), trace(scene, &refractedRay, depth + 1)));
                }
            }
    }

    return outRadiance;
}

void render(Scene const *const scene, Vec3 *const image, uint32_t imageWidth, uint32_t imageHeight)
{
    for (uint32_t y = 0; y < imageHeight; ++y)
    {
        for (uint32_t x = 0; x < imageWidth; ++x)
        {
            Ray const ray = GetRay(&(scene->camera), x, y, imageWidth, imageHeight);
            image[y * imageWidth + x] = trace(scene, &ray, 0);
        }
    }
}

#endif // TRAYRACING_IMPLEMENTATION

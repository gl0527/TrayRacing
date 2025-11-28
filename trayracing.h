#ifndef TRAYRACING_H
#define TRAYRACING_H

#include <stdint.h>
#include <stdio.h>

#ifndef TRAYRACING_DECL
#ifdef TRAYRACING_STATIC
#define TRAYRACING_DECL static
#else
#define TRAYRACING_DECL extern
#endif
#endif

#define BIT(n) (1ULL << (n))

#ifndef FRAME_WIDTH
#define FRAME_WIDTH 600
#endif

#ifndef FRAME_HEIGHT
#define FRAME_HEIGHT 600
#endif

typedef union Vec2 {
    float v[2];

    struct { float x, y; };
    struct { float r, g; };
    struct { float s, t; };
} Vec2;

typedef union Vec3 {
    float v[3];

    struct { float x, y, z; };
    struct { float r, g, b; };
    struct { float s, t, u; };

    struct { Vec2 xy; float _z; };
    struct { Vec2 rg; float _b; };
    struct { Vec2 st; float _u; };
    struct { float _x; Vec2 yz; };
    struct { float _r; Vec2 gb; };
    struct { float _s; Vec2 tu; };
} Vec3;

typedef struct Light {
    Vec3 direction;
    Vec3 exitance;
} Light;

typedef struct Camera {
    Vec3 eye;
    Vec3 lookat;
    Vec3 right;
    Vec3 up;
} Camera;

typedef enum MaterialType
{
    MT_ROUGH = BIT(0),
    MT_REFLECTIVE = BIT(1),
    MT_REFRACTIVE = BIT(2),
} MaterialType;

typedef struct Material
{
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;

    Vec3 minReflectance;
    Vec3 refrIdx;
    Vec3 absorpCoeff;

    uint8_t flags;
} Material;

typedef struct Sphere {
    Vec3 center;
    float radius;
    Material const *material;
} Sphere;

typedef enum Values {
    MAX_MATERIAL_COUNT = 10,
    MAX_SPHERE_COUNT = 128,
    MAX_LIGHT_COUNT = 4
} Values;

typedef struct ResourcePool {
    uint8_t currentMaterialCount;
    Material materials[MAX_MATERIAL_COUNT];
} ResourcePool;

typedef struct Scene {
    uint8_t currentSphereCount;
    uint8_t currentLightCount;
    Sphere spheres[MAX_SPHERE_COUNT];
    Light lights[MAX_LIGHT_COUNT];
    Camera camera;
    Vec3 ambientLight;
} Scene;

typedef struct Frame {
    Vec3 data[FRAME_WIDTH * FRAME_HEIGHT];
} Frame;

#ifdef __cplusplus
extern "C" {
#endif

TRAYRACING_DECL Vec2 vec2_inv(Vec2 a);
TRAYRACING_DECL Vec2 vec2_add(Vec2 a, Vec2 b);
TRAYRACING_DECL Vec2 vec2_sub(Vec2 a, Vec2 b);
TRAYRACING_DECL Vec2 vec2_scale(float f, Vec2 a);
TRAYRACING_DECL Vec2 vec2_mul(Vec2 a, Vec2 b);
TRAYRACING_DECL float vec2_dot(Vec2 a, Vec2 b);
TRAYRACING_DECL float vec2_length_sqr(Vec2 a);
TRAYRACING_DECL float vec2_length(Vec2 a);
TRAYRACING_DECL Vec2 vec2_norm(Vec2 a);
TRAYRACING_DECL float vec2_dist(Vec2 a, Vec2 b);

TRAYRACING_DECL Vec3 vec3_inv(Vec3 a);
TRAYRACING_DECL Vec3 vec3_add(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 vec3_sub(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 vec3_scale(float f, Vec3 a);
TRAYRACING_DECL Vec3 vec3_mul(Vec3 a, Vec3 b);
TRAYRACING_DECL float vec3_dot(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 vec3_cross(Vec3 a, Vec3 b);
TRAYRACING_DECL float vec3_length_sqr(Vec3 a);
TRAYRACING_DECL float vec3_length(Vec3 a);
TRAYRACING_DECL Vec3 vec3_norm(Vec3 a);
TRAYRACING_DECL float vec3_dist(Vec3 a, Vec3 b);
TRAYRACING_DECL Vec3 vec3_reflect(Vec3 n, Vec3 v);
TRAYRACING_DECL Vec3 vec3_refract(Vec3 n, Vec3 i, Vec3 refrIdx);

TRAYRACING_DECL Camera camera_create(Vec3 eye, Vec3 lookat, Vec3 up, float fov);

TRAYRACING_DECL Material material_create(Vec3 ambient, Vec3 diffuse, Vec3 specular, float shininess, Vec3 refrIdx, Vec3 absorption, uint8_t flags);

TRAYRACING_DECL ResourcePool resourcepool_create(void);
TRAYRACING_DECL void resourcepool_add_material(ResourcePool *const pResourcePool, Material material);

TRAYRACING_DECL void frame_save_to_file(Frame const *const frame);

TRAYRACING_DECL Scene scene_create(Camera cam, Vec3 La);
TRAYRACING_DECL void scene_add_sphere(Scene *const scene, Sphere sphere);
TRAYRACING_DECL void scene_add_light(Scene *const scene, Light light);
TRAYRACING_DECL float scene_render(Scene const *const scene, Frame *const frame);

#ifdef __cplusplus
}
#endif

#endif // TRAYRACING_H

#ifdef TRAYRACING_IMPLEMENTATION

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdlib.h>

#ifndef PRECISION
#define PRECISION 1e-4f
#endif

#ifndef M_PIf
#define M_PIf 3.141593f
#endif

#ifdef __cplusplus
#define LITERAL(x) x
#else
#define LITERAL(x) (x)
#endif

typedef struct Ray {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct Hit {
    float t;
    Vec3 position;
    Vec3 normal;
    Material const *material;
} Hit;

static inline float rand_float(float lowerBound, float upperBound)
{
    return (upperBound - lowerBound) * ((float)rand() / RAND_MAX) + lowerBound;
}

static inline int rand_int(int lowerBound, int upperBound)
{
    return rand() % (upperBound - lowerBound + 1) + lowerBound;
}

static inline float clamp(float v, float lowerBound, float upperBound)
{
    return v < lowerBound ? lowerBound : v > upperBound ? upperBound : v;
}

static inline float deg2rad(float f)
{
    return f * (M_PIf / 180.0f);
}

static inline float rad2deg(float f)
{
    return f * (180.0f / M_PIf);
}

static inline Vec2 vec2_zero(void)
{
    return LITERAL(Vec2){.x = 0.0f, .y = 0.0f};
}

static inline Vec2 vec2_one(void)
{
    return LITERAL(Vec2){.x = 1.0f, .y = 1.0f};
}

static inline Vec2 vec2_unit_x(void)
{
    return LITERAL(Vec2){.x = 1.0f, .y = 0.0f};
}

static inline Vec2 vec2_unit_y(void)
{
    return LITERAL(Vec2){.x = 0.0f, .y = 1.0f};
}

static inline Vec2 vec2_negative_unit_x(void)
{
    return LITERAL(Vec2){.x = -1.0f, .y = 0.0f};
}

static inline Vec2 vec2_negative_unit_y(void)
{
    return LITERAL(Vec2){.x = 0.0f, .y = -1.0f};
}

Vec2 vec2_inv(Vec2 a)
{
    return LITERAL(Vec2){.x = -a.x, .y = -a.y};
}

Vec2 vec2_add(Vec2 a, Vec2 b)
{
    return LITERAL(Vec2){.x = a.x + b.x, .y = a.y + b.y};
}

Vec2 vec2_sub(Vec2 a, Vec2 b)
{
    return LITERAL(Vec2){.x = a.x - b.x, .y = a.y - b.y};
}

Vec2 vec2_scale(float f, Vec2 a)
{
    return LITERAL(Vec2){.x = f * a.x, .y = f * a.y};
}

Vec2 vec2_mul(Vec2 a, Vec2 b)
{
    return LITERAL(Vec2){.x = a.x * b.x, .y = a.y * b.y};
}

float vec2_dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

float vec2_length_sqr(Vec2 a)
{
    return a.x * a.x + a.y * a.y;
}

float vec2_length(Vec2 a)
{
    return sqrtf(vec2_length_sqr(a));
}

Vec2 vec2_norm(Vec2 a)
{
    float const len = vec2_length(a);

    return (len > PRECISION) ? vec2_scale(1.0f / len, a) : vec2_zero();
}

float vec2_dist(Vec2 a, Vec2 b)
{
    return vec2_length(vec2_sub(b, a));
}

static inline Vec3 vec3_zero(void)
{
    return LITERAL(Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
}

static inline Vec3 vec3_one(void)
{
    return LITERAL(Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f};
}

static inline Vec3 vec3_unit_x(void)
{
    return LITERAL(Vec3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
}

static inline Vec3 vec3_unit_y(void)
{
    return LITERAL(Vec3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
}

static inline Vec3 vec3_unit_z(void)
{
    return LITERAL(Vec3){.x = 0.0f, .y = 0.0f, .z = 1.0f};
}

static inline Vec3 vec3_negative_unit_x(void)
{
    return LITERAL(Vec3){.x = -1.0f, .y = 0.0f, .z = 0.0f};
}

static inline Vec3 vec3_negative_unit_y(void)
{
    return LITERAL(Vec3){.x = 0.0f, .y = -1.0f, .z = 0.0f};
}

static inline Vec3 vec3_negative_unit_z(void)
{
    return LITERAL(Vec3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
}

Vec3 vec3_inv(Vec3 a)
{
    return LITERAL(Vec3){.x = -a.x, .y = -a.y, .z = -a.z};
}

Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return LITERAL(Vec3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return LITERAL(Vec3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

Vec3 vec3_scale(float f, Vec3 a)
{
    return LITERAL(Vec3){.x = f * a.x, .y = f * a.y, .z = f * a.z };
}

Vec3 vec3_mul(Vec3 a, Vec3 b)
{
    return LITERAL(Vec3){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

float vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return LITERAL(Vec3){  .x = a.y * b.z - a.z * b.y,
                    .y = a.z * b.x - a.x * b.z,
                    .z = a.x * b.y - a.y * b.x};
}

float vec3_length_sqr(Vec3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float vec3_length(Vec3 a)
{
    return sqrtf(vec3_length_sqr(a));
}

Vec3 vec3_norm(Vec3 a)
{
    float const len = vec3_length(a);

    return (len > PRECISION) ? vec3_scale(1.0f / len, a) : vec3_zero();
}

float vec3_dist(Vec3 a, Vec3 b)
{
    return vec3_length(vec3_sub(b, a));
}

Vec3 vec3_reflect(Vec3 n, Vec3 v)
{
    return vec3_sub(v, vec3_scale(2.0f,  vec3_scale(vec3_dot(n, v),  n)));
}

Vec3 vec3_refract(Vec3 n, Vec3 i, Vec3 refrIdx)
{
    float cosa = vec3_dot(n, i);
    if (cosa > 0) {
        cosa = -cosa;
        refrIdx.x = 1.0f / refrIdx.x;
        refrIdx.y = 1.0f / refrIdx.y;
        refrIdx.z = 1.0f / refrIdx.z;
    }
    float const num = 1.0f - cosa * cosa;

    float const vec3_invRefrIdxX = 1.0f / refrIdx.x;
    float const vec3_invRefrIdxY = 1.0f / refrIdx.y;
    float const vec3_invRefrIdxZ = 1.0f / refrIdx.z;

    float const discX = 1.0f - num * vec3_invRefrIdxX * vec3_invRefrIdxX;
    float const discY = 1.0f - num * vec3_invRefrIdxY * vec3_invRefrIdxY;
    float const discZ = 1.0f - num * vec3_invRefrIdxZ * vec3_invRefrIdxZ;

    if (discX < 0.0f || discY < 0.0f || discZ < 0.0f)
    {
        return vec3_reflect(n, i);
    }

    return vec3_add(vec3_add(
                vec3_add(vec3_scale(vec3_invRefrIdxX, i), vec3_scale(cosa * vec3_invRefrIdxX - sqrtf(discX), n)),
                vec3_add(vec3_scale(vec3_invRefrIdxY, i), vec3_scale(cosa * vec3_invRefrIdxY - sqrtf(discY), n))),
                vec3_add(vec3_scale(vec3_invRefrIdxZ, i), vec3_scale(cosa * vec3_invRefrIdxZ - sqrtf(discZ), n)));
}

Camera camera_create(Vec3 eye, Vec3 lookat, Vec3 up, float fov)
{
    Camera camera;

    camera.eye = eye;
    camera.lookat = lookat;
    Vec3 const w = vec3_sub(eye, camera.lookat);
    float const windowSize = vec3_length(w) * tanf(fov * 0.5f);
    camera.right = vec3_scale(windowSize, vec3_norm(vec3_cross(up, w)));
    camera.up = vec3_scale(windowSize, vec3_norm(vec3_cross(w, camera.right)));

    return camera;
}

static Ray camera_get_ray(Camera const *const camera, uint32_t x, uint32_t y, uint32_t screenWidth, uint32_t screenHeight)
{
    Vec3 const dir = vec3_sub(vec3_add(vec3_add(camera->lookat, vec3_scale((2.0f * (x + 0.5f) / screenWidth - 1), camera->right)), vec3_scale((2.0f * (y + 0.5f) / screenHeight - 1), camera->up)), camera->eye);
    return LITERAL(Ray){camera->eye, vec3_norm(dir)};
}

Material material_create(Vec3 ambient, Vec3 diffuse, Vec3 specular, float shininess, Vec3 refrIdx, Vec3 absorption, uint8_t flags)
{
    Material material;

    material.flags = flags;

    if (flags & MT_ROUGH) {
        material.ambient = ambient;
        material.diffuse = diffuse;
        material.specular = specular;
        material.shininess = shininess;
    }

    if (flags & (MT_REFLECTIVE | MT_REFRACTIVE)) {
        material.refrIdx = refrIdx;
        material.absorpCoeff = absorption;

        Vec3 const refrIdxP1 = vec3_add(refrIdx, vec3_one());
        Vec3 const refrIdxM1 = vec3_sub(refrIdx, vec3_one());
        Vec3 const absorpCoeff2 = vec3_mul(material.absorpCoeff, material.absorpCoeff);
        Vec3 const num = vec3_add(vec3_mul(refrIdxM1,  refrIdxM1), absorpCoeff2);
        Vec3 const denom = vec3_add(vec3_mul(refrIdxP1,  refrIdxP1), absorpCoeff2);

        material.minReflectance = LITERAL(Vec3){.r = num.r / denom.r, .g = num.g / denom.g, .b = num.b / denom.b};
    }

    return material;
}

static Vec3 material_shade(Material const *const material, Vec3 normal, Vec3 toEye, Vec3 toLight, Vec3 inRadiance)
{
    if (material->flags & MT_ROUGH)
    {
        Vec3 outRadiance = vec3_zero();
        float const NdotL = vec3_dot(normal, toLight);
        if (NdotL < 0)
        {
            return outRadiance;
        }
        outRadiance = vec3_scale(NdotL, vec3_mul(inRadiance, material->diffuse));
        Vec3 const halfway = vec3_norm(vec3_add(toEye, toLight));
        float const NdotH = vec3_dot(normal, halfway);
        if (NdotH < 0)
        {
            return outRadiance;
        }

        return vec3_add(outRadiance, vec3_scale(powf(NdotH, material->shininess), vec3_mul(inRadiance, material->specular)));
    }
    if (material->flags & (MT_REFLECTIVE | MT_REFRACTIVE))
    {
        float const cosa = fabsf(vec3_dot(normal, toEye));

        return vec3_add(material->minReflectance, vec3_scale(powf(1.0f - cosa, 5), vec3_sub(vec3_one(), material->minReflectance)));
    }

    return vec3_zero();
}

static Material material_emerald(void)
{
    Vec3 const diffuse = {.r = 0.07568f, .g = 0.61424f, .b = 0.07568f};
    Vec3 const specular = {.r = 0.633f, .g = 0.727811f, .b = 0.633f};
    Vec3 const ambient = {.r = 0.0215f, .g = 0.1745f, .b = 0.02145f};
    float const shininess = 76.8f;

    return material_create(ambient, diffuse, specular, shininess, vec3_zero(), vec3_zero(), MT_ROUGH);
}

static Material material_gold(void)
{
    Vec3 eta = {.r = 0.17f, .g = 0.35f, .b = 1.5f};
    Vec3 kappa = {.r = 3.1f, .g = 2.7f, .b = 1.9f};

    return material_create(vec3_zero(), vec3_zero(), vec3_zero(), 0.0f, eta, kappa, MT_REFLECTIVE);
}

static Material material_glass(void)
{
    Vec3 eta = {.r = 1.5f, .g = 1.5f, .b = 1.5f};
    Vec3 kappa = {.r = 0.0f, .g = 0.0f, .b = 0.0f};

    return material_create(vec3_zero(), vec3_zero(), vec3_zero(), 0.0f, eta, kappa, MT_REFLECTIVE | MT_REFRACTIVE);
}

static Material material_silver(void)
{
    Vec3 eta = {.r = 0.14f, .g = 0.16f, .b = 0.13f};
    Vec3 kappa = {.r = 4.1f, .g = 2.3f, .b = 3.1f};

    return material_create(vec3_zero(), vec3_zero(), vec3_zero(), 0.0f, eta, kappa, MT_REFLECTIVE);
}

static Material material_diamond(void)
{
    Vec3 eta = {.r = 2.4f, .g = 2.4f, .b = 2.4f};
    Vec3 kappa = {.r = 0.0f, .g = 0.0f, .b = 0.0f};

    return material_create(vec3_zero(), vec3_zero(), vec3_zero(), 0.0f, eta, kappa, MT_REFLECTIVE | MT_REFRACTIVE);
}

static Material material_copper(void)
{
    Vec3 eta = {.r = 0.2f, .g = 1.1f, .b = 1.2f};
    Vec3 kappa = {.r = 3.6f, .g = 2.6f, .b = 2.3f};

    return material_create(vec3_zero(), vec3_zero(), vec3_zero(), 0.0f, eta, kappa, MT_REFLECTIVE);
}

static Hit sphere_intersect(Sphere const *const sphere, Ray const *const ray)
{
    Vec3 const dist = vec3_sub(ray->origin, sphere->center);
    float const b = 2.0f * vec3_dot(dist, ray->direction);
    float const c = vec3_length_sqr(dist) - sphere->radius * sphere->radius;
    float const disc = b * b - 4.0f * c;

    Hit hit;
    hit.t = -1.0f;

    if (disc < 0.0f) {
        return hit;
    }
    float const sqrt_disc = sqrtf(disc);

    float const t1 = -0.5f * (b - sqrt_disc);
    if (t1 < 0.0f) {
        return hit;
    }
    float const t2 = -0.5f * (b + sqrt_disc);

    hit.t = (t2 > 0.0f) ? t2 : t1;
    hit.position = vec3_add(ray->origin, vec3_scale(hit.t, ray->direction));
    hit.normal = vec3_scale(1.0f / sphere->radius, vec3_sub(hit.position, sphere->center));
    hit.material = sphere->material;

    return hit;
}

ResourcePool resourcepool_create(void)
{
    ResourcePool resourcePool;

    resourcePool.currentMaterialCount = 0;

    return resourcePool;
}

void resourcepool_add_material(ResourcePool *const pResourcePool, Material material)
{
    if (pResourcePool->currentMaterialCount < MAX_MATERIAL_COUNT) {
        pResourcePool->materials[pResourcePool->currentMaterialCount++] = material;
    }
}

void frame_save_to_file(Frame const *const frame)
{
    // Assemble output file name.
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char fname[64];
    snprintf(fname, sizeof(fname), "screenshot_%d%02d%02dT%02d%02d%02d.ppm",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);

    // Open file.
    FILE* file = fopen(fname, "w");

    // Write meta data into file.
    fprintf(file, "P6\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);

    // Write pixel data into file, from the top left pixel to the bottom right pixel.
    for (int32_t y = FRAME_HEIGHT - 1; y >= 0; --y)
    {
        for (int32_t x = 0; x < FRAME_WIDTH; ++x)
        {
            Vec3 const *const pixel = &(frame->data[y * FRAME_WIDTH + x]);

            uint8_t const r = clamp(pixel->r, 0.0f, 1.0f) * 255;
            uint8_t const g = clamp(pixel->g, 0.0f, 1.0f) * 255;
            uint8_t const b = clamp(pixel->b, 0.0f, 1.0f) * 255;

            uint8_t data[3] = { r, g, b };

            fwrite(data, 1, sizeof(data), file);
        }
    }

    // Close file.
    fclose(file);

    printf("Screenshot is saved as \'%s\'.\n", fname);
}

Scene scene_create(Camera cam, Vec3 La)
{
    Scene scene;

    scene.currentSphereCount = 0;
    scene.currentLightCount = 0;
    scene.camera = cam;
    scene.ambientLight = La;

    return scene;
}

void scene_add_sphere(Scene *const scene, Sphere sphere)
{
    if (scene->currentSphereCount < MAX_SPHERE_COUNT) {
        scene->spheres[scene->currentSphereCount++] = sphere;
    }
}

void scene_add_light(Scene *const scene, Light light)
{
    if (scene->currentLightCount < MAX_LIGHT_COUNT) {
        scene->lights[scene->currentLightCount++] = light;
    }
}

static Hit scene_raycast(Scene const *const scene, Ray const *const ray)
{
    Hit bestHit;
    bestHit.t = -1.0f;
    for (uint8_t i = 0; i < scene->currentSphereCount; ++i)
    {
        Hit const hit = sphere_intersect(&(scene->spheres[i]), ray);
        if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
        {
            bestHit = hit;
        }
    }

    if (vec3_dot(ray->direction, bestHit.normal) > 0) {
        bestHit.normal = vec3_inv(bestHit.normal);
    }

    return bestHit;
}

static Vec3 scene_raytrace(Scene const *const scene, Ray const *const ray, uint8_t depth)
{
    if (depth > 5)
    {
        return scene->ambientLight;
    }

    Hit const hit = scene_raycast(scene, ray);
    if (hit.t < 0)
    {
        return scene->ambientLight;
    }

    Vec3 outRadiance = vec3_zero();
    Vec3 const viewDir = vec3_inv(ray->direction);

    if (hit.material->flags & MT_ROUGH)
    {
        outRadiance = vec3_add(outRadiance, vec3_mul(hit.material->ambient, scene->ambientLight));
        for (uint8_t i = 0; i < scene->currentLightCount; ++i)
        {
            Vec3 const toLight = vec3_norm(vec3_inv(scene->lights[i].direction));
            Ray const shadowRay = {vec3_add(hit.position, vec3_scale(PRECISION, hit.normal)), toLight};
            Hit const shadowHit = scene_raycast(scene, &shadowRay);
            if (shadowHit.t < 0)
            {
                outRadiance = vec3_add(outRadiance, material_shade(hit.material, hit.normal, viewDir, toLight, scene->lights[i].exitance));
            }
        }
    }
    if (hit.material->flags & (MT_REFLECTIVE | MT_REFRACTIVE))
    {
        Vec3 const reflectance = material_shade(hit.material, hit.normal, viewDir, vec3_zero(), vec3_zero());

        if (hit.material->flags & MT_REFLECTIVE)
        {
            Vec3 const reflectedDirection = vec3_norm(vec3_reflect(hit.normal, ray->direction));
            Ray const reflectedRay = {vec3_add(hit.position, vec3_scale(PRECISION, hit.normal)), reflectedDirection};
            outRadiance = vec3_add(outRadiance, vec3_mul(reflectance, scene_raytrace(scene, &reflectedRay, depth + 1)));
        }
        if (hit.material->flags & MT_REFRACTIVE)
        {
            Vec3 const refractedDirection = vec3_norm(vec3_refract(hit.normal, ray->direction, hit.material->refrIdx));
            Ray const refractedRay = {vec3_sub(hit.position, vec3_scale(PRECISION, hit.normal)), refractedDirection};
            outRadiance = vec3_add(outRadiance, vec3_mul(vec3_sub(vec3_one(), reflectance), scene_raytrace(scene, &refractedRay, depth + 1)));
        }
    }

    return outRadiance;
}

float scene_render(Scene const *const scene, Frame *const frame)
{
    clock_t const start = clock();

    for (uint32_t y = 0; y < FRAME_HEIGHT; ++y)
    {
        for (uint32_t x = 0; x < FRAME_WIDTH; ++x)
        {
            Ray const ray = camera_get_ray(&(scene->camera), x, y, FRAME_WIDTH, FRAME_HEIGHT);
            frame->data[y * FRAME_WIDTH + x] = scene_raytrace(scene, &ray, 0);
        }
    }

    // Returns the frame time in seconds.
    return (float)(clock() - start) / CLOCKS_PER_SEC;
}

#endif // TRAYRACING_IMPLEMENTATION

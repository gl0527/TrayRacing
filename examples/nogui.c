#define TRAYRACING_IMPLEMENTATION
#include "trayracing/trayracing.h"

ResourcePool resourcePool;
Frame frame;
Scene scene;

static void Init(void)
{
    srand(time(NULL));

    resourcePool = resourcepool_create();

    resourcepool_add_material(&resourcePool, material_emerald());
    resourcepool_add_material(&resourcePool, material_gold());
    resourcepool_add_material(&resourcePool, material_glass());
    resourcepool_add_material(&resourcePool, material_silver());
    resourcepool_add_material(&resourcePool, material_diamond());
    resourcepool_add_material(&resourcePool, material_copper());

    Vec3 eye = {.x = 0.0f, .y = 2.0f, .z = 4.0f};
    Vec3 up = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
    Vec3 lookat = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    float fov = deg2rad(60.0f);
    Vec3 ambient = {.x = 0.5f, .y = 0.6f, .z = 0.8f};

    Camera camera = camera_create(eye, lookat, up, fov);
    scene = scene_create(camera, ambient);

    Vec3 lightDir = {.x = -1.0f, .y = -1.0f, .z = -1.0f};
    Light light = {vec3_norm(lightDir), {.r = 0.8f, .g = 0.8f, .b = 0.8f}};
    scene_add_light(&scene, light);

    for (int i = 0; i < 49; ++i)
    {
        Vec3 center = {.x = rand_float(-1.0f, 1.0f), .y = rand_float(-1.0f, 1.0f), .z = rand_float(-1.0f, 1.0f)};
        float radius = rand_float(0.2f, 0.3f);
        int const materialIndex = rand_int(0, resourcePool.currentMaterialCount - 1);
        Sphere sphere = {center, radius, &(resourcePool.materials[materialIndex])};
        scene_add_sphere(&scene, sphere);
    }

    Vec3 center = {.x = 0.0f, .y = -102.0f, .z = 0.0f};
    float radius = 100.0f;
    Sphere sphere = {center, radius, &(resourcePool.materials[0])};
    scene_add_sphere(&scene, sphere);
}

static void Destroy(void)
{
}

static void Update(float dt)
{
}

static bool Draw(void)
{
    scene_render(&scene, &frame);
    frame_render_frametime(&frame);

    return frame_save_to_file(&frame);
}

int main(int argc, char **argv)
{
    Init();

    int result = EXIT_SUCCESS;

    while(1)
    {
        Update(0.0f);
        if (!Draw())
        {
            result = EXIT_FAILURE;
            break;
        }
    }

    Destroy();

    return result;
}

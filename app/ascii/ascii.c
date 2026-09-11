#define FRAME_WIDTH 120
#define FRAME_HEIGHT 60

#define TRAYRACING_IMPLEMENTATION
#include "trayracing/trayracing.h"

#include <signal.h>

ResourcePool resourcePool;
Frame frame;
Scene scene;

volatile sig_atomic_t keep_running = 1;

static void handle_sigint(int sig)
{
    // Show cursor
    printf("\033[?25h");
    fflush(stdout);

    keep_running = 0;
}

static void Init(void)
{
    resourcePool = resourcepool_create();

    resourcepool_add_material(&resourcePool, material_emerald());
    resourcepool_add_material(&resourcePool, material_gold());
    resourcepool_add_material(&resourcePool, material_glass());
    resourcepool_add_material(&resourcePool, material_silver());
    resourcepool_add_material(&resourcePool, material_diamond());
    resourcepool_add_material(&resourcePool, material_copper());

    Vec3 eye = {.x = 0.0f, .y = 2.0f, .z = 5.0f};
    Vec3 up = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
    Vec3 lookat = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    float fov = deg2rad(60.0f);
    Vec3 ambient = {.r = 0.7f, .g = 0.7f, .b = 0.8f};

    Camera camera = camera_create(eye, lookat, up, fov);
    scene = scene_create(camera, ambient);

    Vec3 lightDir = {.x = -1.0f, .y = -1.0f, .z = -1.0f};
    Light light = {vec3_norm(lightDir), {.r = 0.8f, .g = 0.8f, .b = 0.8f}};
    scene_add_light(&scene, light);

    for (int i = 0; i < 11; ++i)
    {
        Vec3 center = {.z = 3.0f * i / 5.0f - 3.0f, .y = 0.0f, .x = ((i % 2 == 0) ? -0.1f : 0.1f)};
        Sphere sphere = {center, 0.29f, &(resourcePool.materials[i % 6])};
        scene_add_sphere(&scene, sphere);
    }

    Vec3 center = {.x = 0.0f, .y = -100.3f, .z = 0.0f};
    Sphere sphere = {center, 100.0f, &(resourcePool.materials[0])};
    scene_add_sphere(&scene, sphere);
}

static void Destroy(void)
{
}

static void Update(float dt)
{
}

static void Draw(void)
{
    scene_render(&scene, &frame);
    frame_save_to_stdout(&frame);
}

int main(int argc, char **argv)
{
    // Register the signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, handle_sigint);

    Init();

    // Clear screen
    printf("\033[2J");

    // Hide cursor
    printf("\033[?25l");

    while(keep_running)
    {
        Update(0.0f);
        Draw();
    }

    Destroy();

    return 0;
}


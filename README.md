# TrayRacing

TrayRacing is a small, dependency-light ray tracer written in C. It renders scenes made of spheres and directional lights, with rough, reflective, and refractive materials. The project includes a reusable single-header ray-tracing library plus three example applications: terminal ASCII output, PPM image output without a GUI, and a legacy OpenGL/GLUT viewer.

## Features

- Single-header library: `include/trayracing/trayracing.h`
- 2D and 3D vector math
- Sphere intersection and closest-hit ray casting
- Phong-Blinn shading for rough materials
- Directional lighting and hard shadows
- Recursive reflection and refraction with Fresnel reflectance
- Four samples per pixel for basic anti-aliasing
- PPM image output
- Terminal ASCII rendering
- Built-in line and vector-stroke text rendering
- Example materials including emerald, gold, glass, silver, diamond, and copper

## Repository layout

```text
include/trayracing/
└── trayracing.h       Single-header ray-tracing library

app/
├── ascii/             Terminal renderer
├── nogui/             Headless renderer that writes PPM screenshots
└── legacy_opengl/     OpenGL/GLUT windowed renderer

Makefile               Top-level build targets
common.mk              Shared compiler, linker, and output settings
```

## Requirements

### All applications

- GNU Make
- GCC or another compiler compatible with the Makefiles
- A C environment with the standard math and time libraries

### `legacy_opengl`

The OpenGL example additionally requires development packages for:

- OpenGL (`libGL`)
- GLU (`libGLU`)
- GLUT (`libglut`)
- GLEW (`libGLEW`)

The exact package names depend on your operating system.

## Build

Build optimized versions of all three applications:

```sh
make release
```

Build debug versions:

```sh
make debug
```

Build both configurations and create the screenshot directory:

```sh
make
```

Remove generated binaries, object files, assembly listings, and screenshots:

```sh
make clean
```

Build an individual application with the top-level targets:

```sh
make ascii
make nogui
make legacy_opengl
```

The resulting binaries are written to `bin/`:

```text
bin/ascii
bin/nogui
bin/legacy_opengl
```

Debug builds use the `_d` suffix, for example `bin/ascii_d`.

## Running the examples

### ASCII renderer

The ASCII example renders a 120×60 scene directly in the terminal and continues until interrupted:

```sh
./bin/ascii
```

Press `Ctrl+C` to stop it. The renderer uses terminal escape sequences to clear the screen and hide/show the cursor.

### Headless PPM renderer

The no-GUI example renders a randomly generated scene and saves each frame as a binary PPM image:

```sh
mkdir -p screenshot
./bin/nogui
```

Images are written to `screenshot/` with timestamped names such as:

```text
screenshot_YYYYMMDDTHHMMSS_NNN.ppm
```

The program continues rendering until the screenshot counter reaches its built-in limit or the process is stopped.

### OpenGL renderer

Start the GLUT windowed example with:

```sh
./bin/legacy_opengl
```

The scene and light move continuously. Press the space bar to save the current frame as a PPM image in `screenshot/`.

## Using the library

`trayracing.h` is an amalgamated header. Define `TRAYRACING_IMPLEMENTATION` in exactly one translation unit before including it:

```c
#define TRAYRACING_IMPLEMENTATION
#include "trayracing/trayracing.h"
```

Other translation units should include the header without the implementation define:

```c
#include "trayracing/trayracing.h"
```

A minimal scene setup looks like this:

```c
#define TRAYRACING_IMPLEMENTATION
#include "trayracing/trayracing.h"

int main(void)
{
    ResourcePool resources = resourcepool_create();
    resourcepool_add_material(&resources, material_emerald());

    Vec3 eye = {.x = 0.0f, .y = 2.0f, .z = 4.0f};
    Vec3 lookat = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    Vec3 up = {.x = 0.0f, .y = 1.0f, .z = 0.0f};

    Camera camera = camera_create(eye, lookat, up, deg2rad(60.0f));
    Scene scene = scene_create(camera, (Vec3){.r = 0.7f, .g = 0.7f, .b = 0.8f});

    scene_add_light(&scene, (Light){
        .direction = vec3_norm((Vec3){.x = -1.0f, .y = -1.0f, .z = -1.0f}),
        .exitance = (Vec3){.r = 0.8f, .g = 0.8f, .b = 0.8f}
    });

    scene_add_sphere(&scene, (Sphere){
        .center = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
        .radius = 1.0f,
        .material = &resources.materials[0]
    });

    Frame frame;
    scene_render(&scene, &frame);
    frame_save_to_file(&frame);

    return 0;
}
```

Compile a client with the include directory and the math library. `SCREENSHOTS_FOLDER` must be defined when using `frame_save_to_file`; it is expected to include a trailing slash:

```sh
gcc -Iinclude example.c -lm -DSCREENSHOTS_FOLDER=\"screenshot/\" -o example
```

## Configuration

The following macros can be overridden before including the header:

- `FRAME_WIDTH` — frame width; defaults to `600`
- `FRAME_HEIGHT` — frame height; defaults to `600`
- `PRECISION` — ray-origin offset and numerical tolerance; defaults to `1e-4f`
- `TRAYRACING_STATIC` — gives declarations static linkage
- `TRAYRACING_DECL` — customizes declaration linkage
- `SCREENSHOTS_FOLDER` — output directory prefix used by `frame_save_to_file`

The built-in scene limits are:

- 10 materials
- 128 spheres
- 4 lights
- 4 samples per pixel
- Maximum recursive ray depth of 5

## Public API areas

- **Vector math:** `vec2_*` and `vec3_*`
- **Camera:** `camera_create`
- **Materials:** `material_create` and the built-in material helpers
- **Resources:** `resourcepool_create`, `resourcepool_add_material`
- **Scenes:** `scene_create`, `scene_add_sphere`, `scene_add_light`, `scene_render`
- **Frames:** `frame_save_to_file`, `frame_save_to_stdout`, `frame_render_frametime`
- **Overlays:** `line_render`, `text_render`

## Notes

- The renderer is intentionally simple and uses a linear scan over all spheres for each ray; it is best suited to experimentation and learning rather than large scenes.
- `frame_save_to_file` writes binary PPM (`P6`) files, which can be opened by most image viewers or converted with common image tools.
- The OpenGL application uses OpenGL to display the CPU-rendered pixel buffer; the ray tracing itself is implemented in the C header library.

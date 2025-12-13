#define TRAYRACING_IMPLEMENTATION
#include "trayracing/trayracing.h"

#include <time.h>

#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define SCREENWIDTH 600
#define SCREENHEIGHT 600

Frame frame;

#define malloc(x)
#define calloc(x)
#define realloc(x)

#define UNUSED(x) (void)(x)

ResourcePool resourcePool;
Scene scene;

uint8_t tick = 0;

void onInitialization(void) {
    srand(time(NULL));
	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);

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

    for (int i = 0; i < 20; ++i)
    {
        Vec3 center = {.x = rand_float(-1.0f, 1.0f), .y = rand_float(-1.0f, 1.0f), .z = rand_float(-1.0f, 1.0f)};
        float radius = rand_float(0.2f, 0.4f);
        int const materialIndex = rand_int(0, resourcePool.currentMaterialCount - 1);
        Sphere sphere = {center, radius, &(resourcePool.materials[materialIndex])};
        scene_add_sphere(&scene, sphere);
    }

    Vec3 center = {.x = 0.0f, .y = -102.0f, .z = 0.0f};
    float radius = 100.0f;
    Sphere sphere = {center, radius, &(resourcePool.materials[rand_int(0, 0)])};
    scene_add_sphere(&scene, sphere);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay(void) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    float const frameTime = scene_render(&scene, &frame);
    if (tick != 0) {
        printf("Frame time = %.2fms\n", 1000 * frameTime);
    }

    glDrawPixels(FRAME_WIDTH, FRAME_HEIGHT, GL_RGB, GL_FLOAT, frame.data);
	
    glutSwapBuffers();     				// Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    UNUSED(key);
    UNUSED(x);
    UNUSED(y);
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
    UNUSED(x);
    UNUSED(y);

    if (key == 32)
    {
        frame_save_to_file(&frame);
    }
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    UNUSED(button);
    UNUSED(state);
    UNUSED(x);
    UNUSED(y);
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{
    UNUSED(x);
    UNUSED(y);
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle(void) {
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;		// program inditasa ota eltelt ido

    tick = (time - (int)time < 1e-1f) ? 1 : 0;

    Vec3 const newDir = {.x = cosf(0.5f * time), .y = -1.0f, .z = sinf(0.5f * time)};
    scene.lights[0].direction = vec3_norm(vec3_add(newDir, scene.lights[0].direction));

    Vec3 eye = {.x = 3.5f * cosf(0.25f * time), .y = scene.camera.eye.y, .z = 3.5f * sinf(0.25f * time)};
    Vec3 up = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
    Vec3 lookat = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    float fov = deg2rad(60.0f);

    scene.camera = camera_create(eye, lookat, up, fov);

    glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Trayracing");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

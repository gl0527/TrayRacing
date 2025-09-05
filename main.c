#define TRAYRACING_IMPLEMENTATION
#include "trayracing.h"

#include <stdlib.h>
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

#ifndef RAND_FLOAT
#define RAND_FLOAT(min,max) (((max)-(min)) * (float)rand() / RAND_MAX + (min))
#endif

#ifndef RAND_INT
#define RAND_INT(min,max) (rand() % ((max) - (min) + 1) + (min))
#endif

#define UNUSED(x) (void)(x)

ResourcePool resourcePool;
Scene scene;

uint8_t tick = 0;

void onInitialization(void) {
    srand(time(NULL));
	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);

    Vec3 nullVec = {.r = 0.0f, .g = 0.0f, .b = 0.0f};

    Vec3 kdEmerald = {.r = 0.07568f, .g = 0.61424f, .b = 0.07568f};
    Vec3 ksEmerald = {.r = 0.633f, .g = 0.727811f, .b = 0.633f};
    Vec3 kaEmerald = {.r = 0.0215f, .g = 0.1745f, .b = 0.02145f};
    Material materialEmerald;
    createMaterial(&materialEmerald, kaEmerald, kdEmerald, ksEmerald, 76.8f, nullVec, nullVec, MT_ROUGH);
    
    Vec3 kdCopper = {.r = 0.7038f, .g = 0.27048f, .b = 0.0828f};
    Vec3 ksCopper = {.r = 0.256777f, .g = 0.137622f, .b = 0.086014f};
    Vec3 kaCopper = {.r = 0.19125f, .g = 0.0735f, .b = 0.0225f};
    Material materialCopper;
    createMaterial(&materialCopper, kaCopper, kdCopper, ksCopper, 12.8f, nullVec, nullVec, MT_ROUGH);

    Vec3 goldN = {.r = 0.17f, .g = 0.35f, .b = 1.5f};
    Vec3 goldK = {.r = 3.1f, .g = 2.7f, .b = 1.9f};
    Material gold;
    createMaterial(&gold, nullVec, nullVec, nullVec, 0.0f, goldN, goldK, MT_REFLECTIVE);

    Vec3 glassN = {.r = 1.5f, .g = 1.5f, .b = 1.5f};
    Vec3 glassK = {.r = 0.0f, .g = 0.0f, .b = 0.0f};
    Material glass;
    createMaterial(&glass, nullVec, nullVec, nullVec, 0.0f, glassN, glassK, MT_REFLECTIVE | MT_REFRACTIVE);

    Vec3 silverN = {.r = 0.14f, .g = 0.16f, .b = 0.13f};
    Vec3 silverK = {.r = 4.1f, .g = 2.3f, .b = 3.1f};
    Material silver;
    createMaterial(&silver, nullVec, nullVec, nullVec, 0.0f, silverN, silverK, MT_REFLECTIVE);

    set(&resourcePool);

    addMaterial(&resourcePool, materialEmerald);
    addMaterial(&resourcePool, materialCopper);
    addMaterial(&resourcePool, gold);
    addMaterial(&resourcePool, glass);
    addMaterial(&resourcePool, silver);

    Vec3 eye = {.x = 0.0f, .y = 2.0f, .z = 4.0f};
    Vec3 up = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
    Vec3 lookat = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    float fov = 60 * DEG2RAD;
    Vec3 ambient = {.x = 0.5f, .y = 0.6f, .z = 0.8f};

    scene = create(eye, up, lookat, fov, ambient);

    Vec3 lightDir = {.x = -1.0f, .y = -1.0f, .z = -1.0f};
    Light light = {norm(lightDir), {.r = 0.8f, .g = 0.8f, .b = 0.8f}};
    addLight(&scene, light);

    for (int i = 0; i < 20; ++i)
    {
        Vec3 center = {.x = RAND_FLOAT(-1.0f, 1.0f), .y = RAND_FLOAT(-1.0f, 1.0f), .z = RAND_FLOAT(-1.0f, 1.0f)};
        float radius = RAND_FLOAT(0.2f, 0.4f);
        Sphere sphere = {center, radius, &(resourcePool.materials[RAND_INT(0, 4)])};
        addSphere(&scene, sphere);
    }

    Vec3 center = {.x = 0.0f, .y = -2002.0f, .z = 0.0f};
    float radius = 2000.0f;
    Sphere sphere = {center, radius, &(resourcePool.materials[RAND_INT(0, 0)])};
    addSphere(&scene, sphere);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay(void) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    float const frameTime = render(&scene, &frame);
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
    UNUSED(key);
    UNUSED(x);
    UNUSED(y);
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
    scene.lights[0].direction = norm(add(newDir, scene.lights[0].direction));

    Vec3 eye = {.x = 3.5f * cosf(0.25f * time), .y = scene.camera.eye.y, .z = 3.5f * sinf(0.25f * time)};
    Vec3 up = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
    Vec3 lookat = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    float fov = 60 * DEG2RAD;

    SetUp(&(scene.camera), eye, lookat, up, fov);

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

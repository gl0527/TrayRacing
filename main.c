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

    Vec3 nullVec = {0.0f, 0.0f, 0.0f};

    Vec3 kdEmerald = {0.07568f, 0.61424f, 0.07568f};
    Vec3 ksEmerald = {0.633f, 0.727811f, 0.633f};
    Vec3 kaEmerald = {0.0215f, 0.1745f, 0.02145f};
    Material materialEmerald;
    createMaterial(&materialEmerald, kaEmerald, kdEmerald, ksEmerald, 76.8f, nullVec, nullVec, MT_ROUGH);
    
    Vec3 kdCopper = {0.7038f, 0.27048f, 0.0828f};
    Vec3 ksCopper = {0.256777f, 0.137622f, 0.086014f};
    Vec3 kaCopper = {0.19125f, 0.0735f, 0.0225f};
    Material materialCopper;
    createMaterial(&materialCopper, kaCopper, kdCopper, ksCopper, 12.8f, nullVec, nullVec, MT_ROUGH);

    Vec3 goldN = {0.17f, 0.35f, 1.5f};
    Vec3 goldK = {3.1f, 2.7f, 1.9f};
    Material gold;
    createMaterial(&gold, nullVec, nullVec, nullVec, 0.0f, goldN, goldK, MT_REFLECTIVE);

    Vec3 glassN = {1.5f, 1.5f, 1.5f};
    Vec3 glassK = {0.0f, 0.0f, 0.0f};
    Material glass;
    createMaterial(&glass, nullVec, nullVec, nullVec, 0.0f, glassN, glassK, MT_REFLECTIVE | MT_REFRACTIVE);

    Vec3 silverN = {0.14f, 0.16f, 0.13f};
    Vec3 silverK = {4.1f, 2.3f, 3.1f};
    Material silver;
    createMaterial(&silver, nullVec, nullVec, nullVec, 0.0f, silverN, silverK, MT_REFLECTIVE);

    set(&resourcePool);

    addMaterial(&resourcePool, materialEmerald);
    addMaterial(&resourcePool, materialCopper);
    addMaterial(&resourcePool, gold);
    addMaterial(&resourcePool, glass);
    addMaterial(&resourcePool, silver);

    Vec3 eye = {0.0f, 2.0f, 4.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    Vec3 lookat = {0.0f, 0.0f, 0.0f};
    float fov = 60 * DEG2RAD;
    Vec3 ambient = {0.5f, 0.6f, 0.8f};

    scene = create(eye, up, lookat, fov, ambient);

    Vec3 lightDir = {-1.0f, -1.0f, -1.0f};
    Light light = {norm(lightDir), {0.8f, 0.8f, 0.8f}};
    addLight(&scene, light);

    for (int i = 0; i < 20; ++i)
    {
        Vec3 center = {RAND_FLOAT(-1.0f, 1.0f), RAND_FLOAT(-1.0f, 1.0f), RAND_FLOAT(-1.0f, 1.0f)};
        float radius = RAND_FLOAT(0.2f, 0.4f);
        Sphere sphere = {center, radius, &(resourcePool.materials[RAND_INT(0, 4)])};
        addSphere(&scene, sphere);
    }

    Vec3 center = {0.0f, -2002.0f, 0.0f};
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

    Vec3 const newDir = {cosf(0.5f * time), -1.0f, sinf(0.5f * time)};
    scene.lights[0].direction = norm(add(newDir, scene.lights[0].direction));

    Vec3 eye = {3.5f * cosf(0.25f * time), scene.camera.eye.y, 3.5f * sinf(0.25f * time)};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    Vec3 lookat = {0.0f, 0.0f, 0.0f};
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

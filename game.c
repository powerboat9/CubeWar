#include "SDL.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

#define SHIP_MAX_WIDTH 256
#define SHIP_MAX_LENGTH 512
#define SHIP_MAX_HEIGHT 256

struct worldPosition {
    double x;
    double y;
    double z;
    float pitch;
    float yaw;
    float roll;
};

/*typedef partIDType short;

struct part {
    partIDType partTypes[8][8][8];
    unsigned char damageCounts[8][8][8];
};

struct ship {
    struct part shipParts[SHIP_MAX_HEIGHT][SHIP_MAX_LENGTH][SHIP_MAX_WIDTH];
};

struct shipList {
    struct ship *ships;
    unsigned long memSize;
    unsigned long size;
};*/

void initGL(unsigned int width, unsigned int height) {
    float ratio = ((float) width) / height;
    glShadeModel(GL_SMOOTH);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1.5, 50);
    /*glClearDepth(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);*/
}

void makeCube(float sizeX, float sizeY, float sizeZ, float r, float g, float b) {
    sizeX = sizeX / 2;
    sizeY = sizeY / 2;
    sizeZ = sizeZ / 2;
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    /* top */
    glColor3f(1, 0, 0);
    glVertex3f(sizeX, sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, sizeY, sizeZ);
    glVertex3f(sizeX, sizeY, sizeZ);
    /* bottom */
    glColor3f(0, 1, 0);
    glVertex3f(sizeX, 0-sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, 0-sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, 0-sizeY, sizeZ);
    glVertex3f(sizeX, 0-sizeY, sizeZ);
    /* x=1 */
    glColor3f(1, 1, 0);
    glVertex3f(sizeX, sizeY, 0-sizeZ);
    glVertex3f(sizeX, sizeY, sizeZ);
    glVertex3f(sizeX, 0-sizeY, sizeZ);
    glVertex3f(sizeX, 0-sizeY, 0-sizeZ);
    /* x=-1 */
    glColor3f(0, 0, 1);
    glVertex3f(0-sizeX, sizeY, sizeZ);
    glVertex3f(0-sizeX, sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, 0-sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, 0-sizeY, sizeZ);
    /* z=1 */
    glColor3f(1, 0, 1);
    glVertex3f(sizeX, sizeY, sizeZ);
    glVertex3f(0-sizeX, sizeY, sizeZ);
    glVertex3f(0-sizeX, 0-sizeY, sizeZ);
    glVertex3f(sizeX, 0-sizeY, sizeZ);
    /* z=-1 */
    glColor3f(0, 1, 1);
    glVertex3f(sizeX, 0-sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, 0-sizeY, 0-sizeZ);
    glVertex3f(0-sizeX, sizeY, 0-sizeZ);
    glVertex3f(sizeX, sizeY, 0-sizeZ);
    glEnd();
}

void displayGameworld(SDL_Window *window, struct worldPosition view/*struct shipList *ships*/) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(1.5, 0, -7);
    glTranslated(0-view.x, 0-view.y, 0-view.z);
    glRotatef(view.pitch, -1, 0, 0);
    glRotatef(view.yaw, 0, -1, 0);
    glRotatef(view.roll, 0, 0, -1);
    makeCube(2, 2, 2, 1, 1, 1);
    glPopMatrix();
    SDL_GL_SwapWindow(window);
}

SDL_Event event;

struct pressedKeys {
    int isForward:1;
    int isBackward:1;
    int isPitchUp:1;
    int isPitchDown:1;
    int isRollClock:1;
    int isRollCounterClock:1;
    int isYawLeft:1;
    int isYawRight:1;
} presses;

void updateKey(Uint8 code, unsigned char isOn) {
    isOn = isOn ? 1 : 0;
    switch (code) {
        case 'w':
            presses.isForward = isOn;
            break;
        case 'd':
            presses.isBackward = isOn;
            break;
        case 'q':
            presses.isRollCounterClock = isOn;
            break;
        case 'e':
            presses.isRollClock = isOn;
            break;
        case 'i':
            presses.isPitchUp = isOn;
            break;
        case 'k':
            presses.isPitchDown = isOn;
            break;
        case 'j':
            presses.isYawLeft = isOn;
            break;
        case 'l':
            presses.isYawRight = isOn;
            break;
    }
}

int acceptEvents() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return -1;
            case SDL_KEYDOWN:
                updateKey(event.key.keysym.scancode, 1);
                break;
            case SDL_KEYUP:
                updateKey(event.key.keysym.scancode, 0);
                break;
            default:
                break;
        }
    }
}

void move(struct worldPosition *pos) {
    double lookX = cos(2 * M_PI * pos->yaw / 360);
    double lookZ = sin(2 * M_PI * pos->yaw / 360);
    double lookY = cos(2 * M_PI * pos->pitch / 360);
    double length = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
    lookX = lookX / length;
    lookY = lookY / length;
    lookZ = lookZ / length;
    if (presses.isForward ^ presses.isBackward) {
        if (presses.isForward) {
            pos->x += lookX;
            pos->y += lookY;
            pos->z += lookZ;
        } else {
            pos->x -= lookX;
            pos->y -= lookY;
            pos->z -= lookZ;
        }
    }
}

int main(int argc, char **args) {
    printf("Initializing SDL2...\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    printf("Creating window...\n");
    SDL_Window *window = SDL_CreateWindow("CubeWar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "SDL Window Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_GLContext glcon = SDL_GL_CreateContext(window);
    initGL(640, 480);
    struct worldPosition camera;
    camera.x = 0;
    camera.y = 0;
    camera.z = 0;
    camera.pitch = 0;
    camera.yaw = 0;
    camera.roll = 0;
    while (1) {
        displayGameworld(window, camera);
        if (acceptEvents() < 0) break;
        move(&camera);
        SDL_Delay(2000);
    }
    SDL_GL_DeleteContext(glcon);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

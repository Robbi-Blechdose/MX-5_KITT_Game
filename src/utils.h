#ifndef _UTILS_H
#define _UTILS_H

#include "GL/gl.h"
#include <math.h>
#include <SDL.h>
#include "includes/tobjparse.h"

#define EPSILON 0.0001f

#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)
//#define DEG_TO_RAD(deg) (deg * M_PI / 180.0f)

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;

float lerpf(float a, float b, float f);
int8_t lerpInt(int8_t a, int8_t b, int8_t f);

SDL_Surface* loadPNG(const char* path);
model loadModel(const char* path);
GLuint createModelDisplayList(model* model);
GLuint loadModelList(const char* path);
GLuint loadRGBTexture(unsigned char* path);

typedef enum {
    U,
    D,
    L,
    R,
    A,
    B,
    X,
    Y,
    M,
    N,
    Q,
    K,
    S
} Key;

void handleKeys(SDL_Event* event);
uint8_t keyPressed(Key key);

int intersectTriangle(Vector3f* origin, Vector3f* direction, Vector3f* vert0, Vector3f* vert1, Vector3f* vert2,
                      float* t, float* u, float* v);

#endif
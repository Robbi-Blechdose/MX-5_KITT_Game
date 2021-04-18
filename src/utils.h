#ifndef _UTILS_H
#define _UTILS_H

#include <sparrow3d.h>
#include <SDL.h>

typedef struct {
    Sint32 x;
    Sint32 y;
    Sint32 z;
} Vector;

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;

void updateKeys();
//char keyDown(uint8_t code);
char keyReleased(uint8_t code);

Sint32 lerpf(float a, float b, float f);
Sint32 lerpFixed(Sint32 a, Sint32 b, Sint32 f);
int8_t lerpInt(int8_t a, int8_t b, int8_t f);

void fixedVec3toVec3f(Vector3f* result, Vector* a);
void fixedToVec3f(Vector3f* result, Sint32 x, Sint32 y, Sint32 z);

int intersectTriangle(Vector3f* origin, Vector3f* direction, Vector3f* vert0, Vector3f* vert1, Vector3f* vert2,
                      float* t, float* u, float* v);

#endif
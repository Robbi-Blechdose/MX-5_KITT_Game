#ifndef _UTILS_H
#define _UTILS_H

#include <sparrow3d.h>
#include <SDL.h>

typedef struct {
    Sint32 x;
    Sint32 y;
    Sint32 z;
} Vector;

void updateKeys();
//char keyDown(uint8_t code);
char keyReleased(uint8_t code);

Sint32 lerpFixed(Sint32 a, Sint32 b, Sint32 f);
int8_t lerpInt(int8_t a, int8_t b, int8_t f);

#endif
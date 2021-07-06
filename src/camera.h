#ifndef _CAMERA_H
#define _CAMERA_H

#include <SDL.h>

#include "car.h"

void initCamera();
void drawCameraPos();
void calcCameraPos(Car* car, uint16_t ticks);
void toggleCameraDirection();

#endif
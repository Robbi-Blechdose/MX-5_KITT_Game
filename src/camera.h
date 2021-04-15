#ifndef _CAMERA_H
#define _CAMERA_H

#include <sparrow3d.h>
#include <SDL.h>

#include "car.h"

void initCamera();
void drawCameraPos();
void calcCameraPos(Car* car, Uint32 steps);

#endif
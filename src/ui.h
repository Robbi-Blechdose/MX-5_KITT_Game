#ifndef _UI_H
#define _UI_H

#include <sparrow3d.h>
#include <SDL.h>

#include "car.h"

void initGameUI();
void deleteGameUI();
void drawGameUI(Car *car);

void drawFPS(spFontPointer font);

#endif
#ifndef _UI_H
#define _UI_H

#include <sparrow3d.h>
#include <SDL.h>

#include "car.h"

void initMenu();
void initGameUI();

void deleteMenu();
void deleteGameUI();

int calcMenu();
void drawMenu();
void drawGameUI(Car *car);

void drawFPS(spFontPointer font);

#endif
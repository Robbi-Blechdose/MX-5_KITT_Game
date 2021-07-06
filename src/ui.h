#ifndef _UI_H
#define _UI_H

#include <SDL.h>

#include "car.h"

void initMenu();
void initGameUI();

void deleteMenu();
void deleteGameUI();

int calcMenu();
void drawMenu();
void drawGameUI(SDL_Surface* screen, Car *car);

void drawFPS(uint16_t fps);

#endif
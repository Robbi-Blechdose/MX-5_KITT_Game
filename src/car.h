#ifndef _CAR_H
#define _CAR_H

#include <sparrow3d.h>
#include <SDL_image.h>

#include "utils.h"
#include "map.h"

#define POPUPS_DOWN 0
#define POPUPS_UP 1
#define POPUPS_MDOWN 2
#define POPUPS_MUP 3
#define POPUP_POS_UP 0.0f
#define POPUP_POS_DOWN 0.75f
#define POPUP_SPEED 0.005f

#define REDLINE 7200

typedef struct {
    SDL_Surface *bodyTexture;
    SDL_Surface *bodyTextureBraking;
    spModelPointer bodyMesh;
    SDL_Surface *popupTexture;
    spModelPointer popupMesh;
    SDL_Surface *wheelTexture;
    spModelPointer wheelMesh;

    Vector3f position;
    Vector3f rotation;
    
    int16_t revs;
    int8_t gear;

    float speed;
    int16_t speedKPH;

    int8_t steering;

    uint8_t popupState;
    float popupPos;
} Car;

void initCar(Car* car);
void deleteCar(Car* car);

void drawCar(Car* car);
void calcCar(Car* car, Map* map, Uint32 steps);

void accelerate(Car* car, int8_t dir, Uint32 steps);
void steer(Car* car, int8_t dir, Uint32 steps);
void shiftUp(Car* car);
void shiftDown(Car* car);

void togglePopups(Car* car);

#endif
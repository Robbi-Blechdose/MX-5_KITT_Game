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

#define NUM_SKIDS 64

typedef struct {
    SDL_Surface *bodyTexture;
    SDL_Surface *bodyTextureBraking;
    spModelPointer bodyMesh;
    SDL_Surface *popupTexture;
    spModelPointer popupMesh;
    SDL_Surface *wheelTexture;
    spModelPointer wheelMesh;

    SDL_Surface *skidTexture;
    spModelPointer skidMesh;
    Vector3f skidPositions[NUM_SKIDS];
    uint8_t skidIndex;
    uint8_t skidTimer;

    Vector3f position;
    Vector3f rotation;

    Vector3f wheelPositions[4];

    uint8_t onGround;

    int16_t turboBoostTimer;
    int16_t turboBoostCooldown;
    
    int16_t revs;
    int8_t gear;

    float speed;
    int16_t speedKPH;

    int8_t steering;

    uint8_t popupState;
    float popupPos;
} Car;

void initCar(Car* car, Vector3f* pos, float Yrot);
void deleteCar(Car* car);

void drawCar(Car* car);
void calcCar(Car* car, spModelPointer mapMesh, Uint32 steps);

void accelerate(Car* car, int8_t dir, Uint32 steps);
void steer(Car* car, int8_t dir, Uint32 steps);
void shiftUp(Car* car);
void shiftDown(Car* car);

void turboBoost(Car* car);

void togglePopups(Car* car);

#endif
#ifndef _CAR_H
#define _CAR_H

#include <SDL_image.h>

#include "utils.h"
#include "map.h"

#define NUM_SKIDS 64

typedef struct {
    SDL_Surface *skidTexture;
    //spModelPointer skidMesh;
    Vector3f positions[NUM_SKIDS];
    uint8_t index;
    uint8_t timer;
} Skids;

#define POPUPS_NONE 0
#define POPUPS_DOWN 1
#define POPUPS_UP 2
#define POPUPS_MDOWN 3
#define POPUPS_MUP 4
#define POPUP_POS_UP 0.0f
#define POPUP_POS_DOWN 0.75f
#define POPUP_SPEED 0.005f

#define REDLINE 7200

#define TYPE_MX_5   0
#define TYPE_SHELBY 1

typedef struct {
    GLuint bodyTexture;
    GLuint bodyTextureBraking;
    GLuint bodyMesh;

    GLuint popupTexture;
    GLuint popupMesh;
    
    GLuint wheelTexture;
    GLuint wheelMesh;

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

    uint8_t braking;

    Skids skids;
} Car;

void initCar(Car* car, uint8_t type, Vector3f* pos, float yRot);
void deleteCar(Car* car);

void drawCar(Car* car);
void calcCar(Car* car, model* mapMesh, uint16_t ticks);

void accelerate(Car* car, int8_t dir, uint16_t ticks);
void steer(Car* car, int8_t dir, uint16_t ticks);
void shiftUp(Car* car);
void shiftDown(Car* car);

void turboBoost(Car* car);

void togglePopups(Car* car);

#endif
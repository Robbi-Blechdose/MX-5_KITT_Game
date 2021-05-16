#ifndef _MISSION_H
#define _MISSION_H

#include <sparrow3d.h>
#include <SDL.h>
#include "utils.h"
#include "map.h"
#include "car.h"

typedef struct {
    Map map;
    Vector3f playerStartPos;
    float playerStartYRot;
} Mission;


void loadMission(Mission* mission, uint8_t index);
void initCurrentMission(Mission* mission, Car* playerCar);

#endif
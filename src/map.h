#ifndef _MAP_H
#define _MAP_H

#include <sparrow3d.h>
#include <SDL.h>

#include "utils.h"

typedef struct {
    char* terrainMesh;
    char* terrainTexture;
    char* buildingMesh;
    char* buildingTexture;
    uint8_t numTrees;
    uint8_t numRocks;
    uint8_t numSigns;
    //Trees and rocks only save the difference in position
    Vector trees[16];
    Vector rocks[16];
    //Signs save absolute position and rotation
    Vector signs[16];
    Sint32 signRot[16];

    //FIXME: Temporary hack so the cars have access to the mesh for raycasting
    spModelPointer mapMesh;
} Map;

void initMap(Map* map);
void deleteMap();
void drawMap(Map* map);

#endif
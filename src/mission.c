#include <math.h>

#include "mission.h"
#include "resources.h"

/**
 * Somewhat crude method for loading mission data
 **/
void loadMission(Mission* mission, uint8_t index)
{
    switch(index)
    {
        case 0:
        {
            mission->map.terrainMesh = MAP01_MESH;
            mission->map.terrainTexture = MAP01_TEXTURE;

            mission->map.numTrees = 5;
            mission->map.trees[0] = (Vector) {.x = spIntToFixed(-20), .y = 0, .z = spIntToFixed(-35)};              //-20, 0, -35
            mission->map.trees[1] = (Vector) {.x = spIntToFixed(-10), .y = 0, .z = spIntToFixed(-1)};               //-30, 0, -36
            mission->map.trees[2] = (Vector) {.x = spIntToFixed(-10), .y = 0, .z = spIntToFixed(3)};                //-40, 0, -33
            mission->map.trees[3] = (Vector) {.x = spIntToFixed(70), .y = spIntToFixed(5), .z = spIntToFixed(73)};  // 30, 5,  40
            mission->map.trees[4] = (Vector) {.x = spIntToFixed(2), .y = spIntToFixed(-5), .z = spIntToFixed(-27)}; // 32, 0,  13

            mission->map.numRocks = 1;
            mission->map.rocks[0] = (Vector) {.x = spIntToFixed(-29), .y = 0, .z = spIntToFixed(-15)}; //-29, 0, -15

            mission->map.numSigns = 6;
            //Group in first corner
            mission->map.signs[0] = (Vector) {.x = spFloatToFixed(-38), .y = 0, .z = spFloatToFixed(-32)};
            mission->map.signs[1] = (Vector) {.x = spFloatToFixed(-41), .y = 0, .z = spFloatToFixed(-29)};
            mission->map.signs[2] = (Vector) {.x = spFloatToFixed(-43.5f), .y = 0, .z = spFloatToFixed(-26.5f)};
            //Group in corner behind ramps
            mission->map.signs[3] = (Vector) {.x = spFloatToFixed(6), .y = 0, .z = spFloatToFixed(24)};
            mission->map.signs[4] = (Vector) {.x = spFloatToFixed(9), .y = 0, .z = spFloatToFixed(21)};
            mission->map.signs[5] = (Vector) {.x = spFloatToFixed(12), .y = 0, .z = spFloatToFixed(18)};
            //Y rotation for signs
            //Group in first corner
            mission->map.signRot[0] = spFloatToFixed(-3.926f);
            mission->map.signRot[1] = spFloatToFixed(-3.926f);
            mission->map.signRot[2] = spFloatToFixed(-3.926f);
            //Group in corner behind ramps
            mission->map.signRot[3] = spFloatToFixed(-0.567f);
            mission->map.signRot[4] = spFloatToFixed(-0.785f);
            mission->map.signRot[5] = spFloatToFixed(-1.003f);

            //Player start position
            mission->playerStartPos.x = 0;
            mission->playerStartPos.y = 1.0f;
            mission->playerStartPos.z = -29.0f;
            mission->playerStartYRot = -M_PI / 2;

            break;
        }
    }
}

void initCurrentMission(Mission* mission, Car* playerCar)
{
    initMap(&mission->map);
    initCar(playerCar, TYPE_MX_5, &mission->playerStartPos, mission->playerStartYRot);

    initCar(&mission->enemies[0], TYPE_SHELBY, &mission->playerStartPos, mission->playerStartYRot);
}
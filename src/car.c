#include <math.h>

#include "car.h"
#include "resources.h"
#include "utils.h"

int8_t accelerationTable[6] = {
    100, 120, 80, 60, 30, 20
};

#define BRAKING 140
#define IDLE_BRAKING 10

int16_t speedTable[6] = {
    -7200, 7200, 7200 / 2, 7200 / 3, 7200 / 4, 7200 / 5
};

void initCar(Car* car)
{
    //Load body texture and mesh
	car->bodyTexture = spLoadSurface(MX_5_TEXTURE);
    car->bodyTextureBraking = spLoadSurface(MX_5_TEXTURE_BRAKING);
	car->bodyMesh = spMeshLoadObj(MX_5_MESH, car->bodyTexture, 65535);
    //Load popup texture and mesh
	car->popupTexture = spLoadSurface(POPUP_TEXTURE);
	car->popupMesh = spMeshLoadObj(POPUP_MESH, car->popupTexture, 65535);
    //Load wheel texture and mesh
    car->wheelTexture = spLoadSurface(WHEEL_TEXTURE);
	car->wheelMesh = spMeshLoadObj(WHEEL_MESH, car->wheelTexture, 65535);
    //Set initial position + rotation
    car->position.x = 0;
    car->position.y = 1.0f;
    car->position.z = -29.0f;
    car->rotation.y = spFixedToFloat(spDiv(-SP_PI, spIntToFixed(2)));
    //Set initial wheel height
    int i;
    for(i = 0; i < 4; i++)
    {
        car->wheelPositions[i].y = 1.0f;
    }
    //Set to gear 1
    car->gear = 1;
    //Set popups to up
    car->popupState = POPUPS_UP;
}

void deleteCar(Car* car)
{
	spDeleteSurface(car->bodyTexture);
	spMeshDelete(car->bodyMesh);
	spDeleteSurface(car->popupTexture);
	spMeshDelete(car->popupMesh);
	spDeleteSurface(car->wheelTexture);
	spMeshDelete(car->wheelMesh);
}

void drawCar(Car* car)
{
    //Set car position + rotation
	spTranslate(spFloatToFixed(car->position.x),
                spFloatToFixed(car->position.y),
                spFloatToFixed(car->position.z));
	spRotateX(spFloatToFixed(car->rotation.x));
	spRotateY(spFloatToFixed(car->rotation.y));
	spRotateZ(spFloatToFixed(car->rotation.z));
    //Draw car
	spMesh3D(car->bodyMesh, 2);
    //Save model view
    spPushModelView();

    //Set popup position + rotation
    spTranslate(0, spFloatToFixed(0.03f), spFloatToFixed(1.19f));
    spRotateX(spFloatToFixed(car->popupPos));
    //Draw popups
    spMesh3D(car->popupMesh, 2);

    //Get model view back
    spPopModelView();
    //Set wheel position + rotation, then draw
    //Front left 0.57 -0.33 0.96
    spTranslate(spFloatToFixed(0.57f), spFloatToFixed(-0.33f), spFloatToFixed(0.96f));
    spRotateY(spIntToFixed(car->steering * 45));
    spMesh3D(car->wheelMesh, 2);
    //Revert angle
    spRotateY(spIntToFixed(car->steering * -45));
    //Front right -0.57 -0.33 0.96
    spTranslate(spFloatToFixed(-0.57f * 2), 0, 0);
    spRotateY(spIntToFixed(car->steering * 45));
    spMesh3D(car->wheelMesh, 2);
    //Revert angle
    spRotateY(spIntToFixed(car->steering * -45));
    //Rear right -0.57 -0.33 -0.96
    spTranslate(0, 0, spFloatToFixed(-0.96f * 2));
    spMesh3D(car->wheelMesh, 2);
    //Rear left 0.57 -0.33 -0.96
    spTranslate(spFloatToFixed(0.57f * 2), 0, 0);
    spMesh3D(car->wheelMesh, 2);
}

/**
 * Calculates the x and z positions of the wheels
 * The y position is set via raycasts and used to calculate the x and z rotation of the car
 **/
void calcWheelPositions(Car* car)
{
    //Set base position
    int i;
    for(i = 0; i < 4; i++)
    {
        car->wheelPositions[i].x = car->position.x;
        car->wheelPositions[i].z = car->position.z;
    }

    float sinY = sin(car->rotation.y);
    float cosY = cos(car->rotation.y);
    //Front left 0.57 -0.33 0.96
    car->wheelPositions[0].x += 0.57f * cosY + 0.96f * sinY;
    car->wheelPositions[0].z += 0.57f * -sinY + 0.96f * cosY;
    //Front right -0.57 -0.33 0.96
    car->wheelPositions[1].x += -0.57f * cosY + 0.96f * sinY;
    car->wheelPositions[1].z += -0.57f * -sinY + 0.96f * cosY;
    //Rear right -0.57 -0.33 -0.96
    car->wheelPositions[2].x += -0.57f * cosY + -0.96f * sinY;
    car->wheelPositions[2].z += -0.57f * -sinY + -0.96f * cosY;
    //Rear left 0.57 -0.33 -0.96
    car->wheelPositions[3].x += 0.57f * cosY + -0.96f * sinY;
    car->wheelPositions[3].z += 0.57f * -sinY + -0.96f * cosY;
}

#define DISTANCE 2.0f

/**
 * Check which triangles have at least one vertex in the radius of "car position + 3.0f"
 * Then run Möller-Trumbore for each wheel
 **/
void calcRaycast(Car* car, Map* map)
{
    Vector3f* position = &car->position;
    spModelPointer mapMesh = map->mapMesh;

    calcWheelPositions(car);

    int i, j;
    for(i = 0; i < mapMesh->texTriangleCount; i++)
	{
        for(j = 0; j < 3; j++)
        {
            if(spFixedToFloat(mapMesh->texPoint[mapMesh->texTriangle[i].point[j]].x) > (position->x - DISTANCE) &&
               spFixedToFloat(mapMesh->texPoint[mapMesh->texTriangle[i].point[j]].x) < (position->x + DISTANCE) &&
               spFixedToFloat(mapMesh->texPoint[mapMesh->texTriangle[i].point[j]].z) > (position->z - DISTANCE) &&
               spFixedToFloat(mapMesh->texPoint[mapMesh->texTriangle[i].point[j]].z) < (position->z + DISTANCE))
            {
                Vector3f dir, vert0, vert1, vert2;
                dir.x = 0;
                dir.y = -1.0f;
                dir.z = 0;
                fixedToVec3f(&vert0, mapMesh->texPoint[mapMesh->texTriangle[i].point[0]].x,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[0]].y,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[0]].z);
                fixedToVec3f(&vert1, mapMesh->texPoint[mapMesh->texTriangle[i].point[1]].x,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[1]].y,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[1]].z);
                fixedToVec3f(&vert2, mapMesh->texPoint[mapMesh->texTriangle[i].point[2]].x,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[2]].y,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[2]].z);
                float t, u, v;
                if(intersectTriangle(&car->wheelPositions[0], &dir, &vert0, &vert1, &vert2, &t, &u, &v))
                {
                    float diff = (1.0f - t);
                    //Super simple limit for drop speed
                    //Would have to be improved with steps variable
                    if(diff < -0.01f)
                    {
                        diff = -0.01f;
                    }
                    position->y += diff;
                    car->wheelPositions[0].y += diff;
                    return;
                }
            }
        }
    }
}

void calcCar(Car* car, Map* map, Uint32 steps)
{
    calcRaycast(car, map);

    car->speed = (((float) car->revs) / speedTable[car->gear]) * (steps / 750.0f);
    /**
    car->speed = spMul(spDiv(spIntToFixed(car->revs), spIntToFixed(speedTable[car->gear])),
                       spDiv(spIntToFixed(steps), spIntToFixed(750)));
    **/
    car->speedKPH = spFixedToInt(spMul(spDiv(spIntToFixed(car->revs), spIntToFixed(speedTable[car->gear])),
                                       spIntToFixed(40)));

    //Calculate x and z position
    car->position.x += car->speed * sinf(car->rotation.y);
    car->position.z += car->speed * cosf(car->rotation.y);

    //Handle popups
    if(car->popupState == POPUPS_MDOWN)
    {
        if(car->popupPos < POPUP_POS_DOWN)
        {
            car->popupPos += POPUP_SPEED * steps;
        }
        else
        {
            car->popupState = POPUPS_DOWN;
            car->popupPos = POPUP_POS_DOWN;
        }
    }
    else if(car->popupState == POPUPS_MUP)
    {
        if(car->popupPos > POPUP_POS_UP)
        {
            car->popupPos -= POPUP_SPEED * steps;
        }
        else
        {
            car->popupState = POPUPS_UP;
            car->popupPos = POPUP_POS_UP;
        }
    }
}

void accelerate(Car* car, int8_t dir, Uint32 steps)
{
    //Only start the car in first or reverse
    if(car->revs == 0 && car->gear > 1)
    {
        return;
    }

    //Accelerate, brake or idle brake
    if(dir > 0)
    {
        car->revs += (accelerationTable[car->gear] * steps) / 20;
        car->bodyMesh->texture = car->bodyTexture;
    }
    else if(dir < 0)
    {
        car->revs -= (BRAKING * steps) / 20;
        car->bodyMesh->texture = car->bodyTextureBraking;
    }
    else
    {
        car->revs -= (IDLE_BRAKING * steps) / 20;
        car->bodyMesh->texture = car->bodyTexture;
    }

    if(car->revs < 0)
    {
        car->revs = 0;
    }
    else if(car->revs > REDLINE)
    {
        car->revs = REDLINE;
    }
}

void steer(Car* car, int8_t dir, Uint32 steps)
{
    if(car->speedKPH != 0)
    {
        car->rotation.y += dir * (steps / 1000.0f);
        car->rotation.z = lerpf(car->rotation.z, dir * 0.07f, steps * 0.01f);
        //car->rotation.y += spMul(spIntToFixed(dir), spDiv(spIntToFixed(steps), spIntToFixed(1000)));
        //car->rotation.z = lerpFixed(car->rotation.z, spFloatToFixed(dir * 0.07f), spFloatToFixed(steps * 0.01f));
    }
    car->steering = dir;
}

void shiftUp(Car* car)
{
    if(car->gear < 5)
    {
        //Reverse lockout
        if(car->gear == 0 && car->revs > 0)
        {
            return;
        }

        //Shift up
        car->gear++;
        //Did we shift from reverse to 1st?
        if(car->gear == 1)
        {
            car->revs = 0;
        }
        else
        {
            //Drop the revs (get ratio between gears and divide the revs by that)
            float ratio = ((float) speedTable[car->gear - 1]) / speedTable[car->gear];
            car->revs /= ratio;
        }
    }
}

void shiftDown(Car* car)
{
    if(car->gear > 0)
    {
        //Reverse lockout
        if(car->gear == 1 && car->revs > 0)
        {
            return;
        }

        //Shift down
        car->gear--;

        //Did we shift from 1st to reverse?
        if(car->gear == 0)
        {
            car->revs = 0;
        }
        else
        {
            //Up the revs
            //TODO: maybe damage the engine when over-revving?
            float ratio = ((float) speedTable[car->gear + 1]) / speedTable[car->gear];
            car->revs /= ratio;
        }
    }
}

void togglePopups(Car* car)
{
    if(car->popupState == POPUPS_DOWN || car->popupState == POPUPS_MDOWN)
    {
        car->popupState = POPUPS_MUP;
    }
    else
    {
        car->popupState = POPUPS_MDOWN;
    }
}
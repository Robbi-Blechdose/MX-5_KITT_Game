#include <math.h>

#include "car.h"
#include "resources.h"
#include "utils.h"

int8_t accelerationTable[6] = {
    100, 120, 80, 60, 30, 20
};

#define BRAKING 140
#define IDLE_BRAKING 10

float speedTable[6] = {
    -7200, 7200, 7200 / 2, 7200 / 3, 7200 / 4, 7200 / 5
};

void initCar(Car* car, Vector3f* pos, float Yrot)
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
    car->rotation.y = -M_PI / 2;
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

    //Load skid texture and mesh
    car->skidTexture = spLoadSurface(SKID_TEXTURE);
	car->skidMesh = spMeshLoadObj(SKID_MESH, car->skidTexture, 65535);
    car->skidIndex = 0;
    car->skidTimer = 0;
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
    spSetAlphaTest(1);
    int i;
    for(i = 0; i < NUM_SKIDS; i++)
    {
        spPushModelView();
        spTranslate(spFloatToFixed(car->skidPositions[i].x), spFloatToFixed(car->skidPositions[i].y - 0.9f), spFloatToFixed(car->skidPositions[i].z));
        spMesh3D(car->skidMesh, 2);
        spPopModelView();
    }
    spSetAlphaTest(0);

    //Set car position + rotation
	spTranslate(spFloatToFixed(car->position.x),
                spFloatToFixed(car->position.y),
                spFloatToFixed(car->position.z));
	spRotateY(spFloatToFixed(car->rotation.y));
	spRotateX(spFloatToFixed(car->rotation.x));
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

#define DISTANCE 3.0f

/**
 * Check which triangles have at least one vertex in the radius of "car position + 3.0f"
 * Then run M??ller-Trumbore for each wheel
 **/
void calcRaycast(Car* car, spModelPointer mapMesh, Uint32 steps)
{
    Vector3f* position = &car->position;

    calcWheelPositions(car);

    uint8_t done[4] = {0, 0, 0, 0};

    car->onGround = 0;

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
                float t, u, v;
                dir.x = 0;
                dir.y = -1.0f;
                dir.z = 0;
                //TODO: Precompute this on game start? We got more than enough RAM
                //      Note: Won't be necessary if we rewrite sparrow3d to use floats...
                fixedToVec3f(&vert0, mapMesh->texPoint[mapMesh->texTriangle[i].point[0]].x,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[0]].y,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[0]].z);
                fixedToVec3f(&vert1, mapMesh->texPoint[mapMesh->texTriangle[i].point[1]].x,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[1]].y,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[1]].z);
                fixedToVec3f(&vert2, mapMesh->texPoint[mapMesh->texTriangle[i].point[2]].x,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[2]].y,
                                     mapMesh->texPoint[mapMesh->texTriangle[i].point[2]].z);
                
                int k;
                for(k = 0; k < 4; k++)
                {
                    if(done[k])
                    {
                        continue;
                    }
                    if(intersectTriangle(&car->wheelPositions[k], &dir, &vert0, &vert1, &vert2, &t, &u, &v))
                    {
                        float diff = (1.0f - t);
                        //Super simple limit for drop speed
                        //TODO: Should be improved with steps variable
                        if(diff < -0.01f)
                        {
                            diff = -0.01f;
                        }
                        car->wheelPositions[k].y += diff;
                        done[k] = 1;

                        if(diff > -0.001f && diff < 0.005f)
                        {
                            car->onGround++;
                        }
                    }
                }
            }
        }
    }

    //Apply turbo boost
    if(car->turboBoostTimer)
    {
        //TODO: Use steps variable here?
        car->turboBoostTimer *= 0.96f;
        if(car->turboBoostTimer < 0)
        {
            car->turboBoostTimer = 0;
        }

        car->wheelPositions[0].y += (car->turboBoostTimer / 10000.0f);
        car->wheelPositions[1].y += (car->turboBoostTimer / 10000.0f);
        car->wheelPositions[2].y += (car->turboBoostTimer / 10000.0f);
        car->wheelPositions[3].y += (car->turboBoostTimer / 10000.0f);
    }

    car->position.y = (car->wheelPositions[0].y + car->wheelPositions[1].y +
                       car->wheelPositions[2].y + car->wheelPositions[3].y) / 4;
    
    float ldiff = car->wheelPositions[0].y - car->wheelPositions[3].y;
    float rdiff = car->wheelPositions[1].y - car->wheelPositions[2].y;

    car->rotation.x = -asinf((ldiff + rdiff) / (0.96f * 4));
    car->rotation.z = asinf(((car->wheelPositions[0].y - car->wheelPositions[1].y) +
                             (car->wheelPositions[3].y - car->wheelPositions[2].y)) / (0.57 * 4));
    

    //Reduce speed if we're going up too tall a slope - in a forward gear
    if(car->revs > 0 && car->speed > 0 && (ldiff > 1 || rdiff > 1))
    {
        car->revs -= 100;
        if(car->revs < 0)
        {
            car->revs = 0;
        }
    }
}

void calcCar(Car* car, spModelPointer mapMesh, Uint32 steps)
{
    calcRaycast(car, mapMesh, steps);

    float speed = (car->revs / speedTable[car->gear]);
    car->speed = speed * (steps / 750.0f) + (car->turboBoostTimer / 8000.0f);
    car->speedKPH = speed * 40; //TODO: Add turbo boost here...

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

    if(car->gear == 1 && car->onGround > 3)
    {
        car->skidTimer += steps;
        if(car->skidTimer > 100)
        {
            car->skidTimer = 0;
            car->skidPositions[car->skidIndex++] = car->wheelPositions[2];
            car->skidPositions[car->skidIndex++] = car->wheelPositions[3];
            if(car->skidIndex == NUM_SKIDS)
            {
                car->skidIndex = 0;
            }
        }
    }

    if(car->turboBoostCooldown > 0)
    {
        car->turboBoostCooldown -= steps;
    }
}

void accelerate(Car* car, int8_t dir, Uint32 steps)
{
    //Only start the car in first or reverse, and don't accelerate while in the air
    if((car->revs == 0 && car->gear > 1) || car->onGround < 4)
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
        //Forward gear
        if(car->gear > 0)
        {
            car->rotation.y += dir * (steps / 1000.0f);
        }
        //Reverse
        else
        {
            car->rotation.y -= dir * (steps / 1000.0f);
        }
        //car->rotation.z = lerpf(car->rotation.z, dir * 0.07f, steps * 0.01f);
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
            float ratio = speedTable[car->gear - 1] / speedTable[car->gear];
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
            float ratio = speedTable[car->gear + 1] / speedTable[car->gear];
            car->revs /= ratio;
        }
    }
}

void turboBoost(Car* car)
{
    if(car->turboBoostCooldown <= 0)
    {
        car->turboBoostTimer = 1000;
        car->turboBoostCooldown = 2000;
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
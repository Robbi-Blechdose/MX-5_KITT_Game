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

void initCar(Car* car, uint8_t type, Vector3f* pos, float yRot)
{
    switch(type)
    {
        case TYPE_MX_5:
        {
            //Load body texture and mesh
            car->bodyTexture = loadRGBTexture(MX_5_TEXTURE);
            car->bodyTextureBraking = loadRGBTexture(MX_5_TEXTURE_BRAKING);
            car->bodyMesh = loadModelList(MX_5_MESH);
            //Load popup texture and mesh
            car->popupTexture = loadRGBTexture(POPUP_TEXTURE);
            car->popupMesh = loadModelList(POPUP_MESH);
            //Set popups to up
            car->popupState = POPUPS_UP;
            break;
        }
        case TYPE_SHELBY:
        {
            //Load body texture and mesh
            car->bodyTexture = loadRGBTexture(SHELBY_TEXTURE);
            car->bodyTextureBraking = loadRGBTexture(SHELBY_TEXTURE); //TODO: Braking texture
            car->bodyMesh = loadModelList(SHELBY_MESH);
            //No popups
            car->popupState = POPUPS_NONE;
            break;
        }
    }

    //Load wheel texture and mesh
    car->wheelTexture = loadRGBTexture(WHEEL_TEXTURE);
    car->wheelMesh = loadModelList(WHEEL_MESH);
    //Set initial position + rotation
    car->position.x = pos->x;
    car->position.y = pos->y;
    car->position.z = pos->z;
    car->rotation.y = yRot;
    //Set initial wheel height
    int i;
    for(i = 0; i < 4; i++)
    {
        car->wheelPositions[i].y = 1.0f;
    }
    //Set to gear 1
    car->gear = 1;
    //Not braking
    car->braking = 0;

    /**
    //Load skid texture and mesh
    car->skids.skidTexture = loadPNG(SKID_TEXTURE);
	car->skids.skidMesh = spMeshLoadObj(SKID_MESH, car->skids.skidTexture, 65535);
    car->skids.index = 0;
    car->skids.timer = 0;
    **/
}

void deleteCar(Car* car)
{
    /**
	SDL_FreeSurface(car->bodyTexture);
	spMeshDelete(car->bodyMesh);
	SDL_FreeSurface(car->popupTexture);
	spMeshDelete(car->popupMesh);
	SDL_FreeSurface(car->wheelTexture);
	spMeshDelete(car->wheelMesh);
    **/

    //TODO: Delete skids
}

void drawCar(Car* car)
{
    glPushMatrix();
    /**
    glEnable(GL_ALPHA_TEST);
    int i;
    for(i = 0; i < NUM_SKIDS; i++)
    {
        glPushMatrix();
        glTranslatef(car->skids.positions[i].x, car->skids.positions[i].y - 0.9f, car->skids.positions[i].z);
        spMesh3D(car->skids.skidMesh, 2);
        glPopMatrix();
    }
    glDisable(GL_ALPHA_TEST);
    **/

    //Set car position + rotation
	glTranslatef(car->position.x, car->position.y, car->position.z);
    glRotatef(RAD_TO_DEG(car->rotation.y), 0, 1, 0);
    glRotatef(RAD_TO_DEG(car->rotation.x), 1, 0, 0);
    glRotatef(RAD_TO_DEG(car->rotation.z), 0, 0, 1);
    //Draw car
    if(car->braking)
    {
	    glBindTexture(GL_TEXTURE_2D, car->bodyTextureBraking);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, car->bodyTexture);
    }
	glCallList(car->bodyMesh);

    //Set popup position + rotation
    if(car->popupState)
    {
        glPushMatrix();
        glTranslatef(0, 0.03f, 1.19f);
        glRotatef(RAD_TO_DEG(car->popupPos), 1, 0, 0);
        //Draw popups
	    glBindTexture(GL_TEXTURE_2D, car->popupTexture);
        glCallList(car->popupMesh);
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, car->wheelTexture);
    //Set wheel position + rotation, then draw
    //Front left 0.57 -0.33 0.96
    glTranslatef(0.57f, -0.33f, 0.96f);
    glRotatef(car->steering * 45, 0, 1, 0);
    glCallList(car->wheelMesh);
    //Revert angle
    glRotatef(car->steering * -45, 0, 1, 0);
    //Front right -0.57 -0.33 0.96
    glTranslatef(-0.57f * 2, 0, 0);
    glRotatef(car->steering * 45, 0, 1, 0);
    glCallList(car->wheelMesh);
    //Revert angle
    glRotatef(car->steering * -45, 0, 1, 0);
    //Rear right -0.57 -0.33 -0.96
    glTranslatef(0, 0, -0.96f * 2);
    glCallList(car->wheelMesh);
    //Rear left 0.57 -0.33 -0.96
    glTranslatef(0.57f * 2, 0, 0);
    glCallList(car->wheelMesh);

    glPopMatrix();
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

    float sinY = sinf(car->rotation.y);
    float cosY = cosf(car->rotation.y);
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
 * Then run Möller-Trumbore for each wheel
 **/
void calcRaycast(Car* car, model* mapMesh, uint16_t ticks)
{
    Vector3f* position = &car->position;

    calcWheelPositions(car);

    uint8_t done[4] = {0, 0, 0, 0};

    car->onGround = 0;

    int i, j;
    for(i = 0; i < mapMesh->npoints; i += 3)
	{
        for(j = 0; j < 3; j++)
        {
            if(mapMesh->d[i + j].d[0] > (position->x - DISTANCE) && mapMesh->d[i + j].d[0] < (position->x + DISTANCE) &&
                mapMesh->d[i + j].d[2] > (position->z - DISTANCE) && mapMesh->d[i + j].d[2] < (position->z + DISTANCE))
            {
                Vector3f dir, vert0, vert1, vert2;
                float t, u, v;
                dir.x = 0;
                dir.y = -1.0f;
                dir.z = 0;
                vert0 = (Vector3f) {.x = mapMesh->d[i].d[0], .y = mapMesh->d[i].d[1], .z = mapMesh->d[i].d[2]};
                vert1 = (Vector3f) {.x = mapMesh->d[i + 1].d[0], .y = mapMesh->d[i + 1].d[1], .z = mapMesh->d[i + 1].d[2]};
                vert2 = (Vector3f) {.x = mapMesh->d[i + 2].d[0], .y = mapMesh->d[i + 2].d[1], .z = mapMesh->d[i + 2].d[2]};
                
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

void calcCar(Car* car, model* mapMesh, uint16_t ticks)
{
    calcRaycast(car, mapMesh, ticks);

    float speed = (car->revs / speedTable[car->gear]);
    car->speed = speed * (ticks / 750.0f) + (car->turboBoostTimer / 8000.0f);
    car->speedKPH = speed * 40 + (car->turboBoostTimer / 10);

    //Calculate x and z position
    car->position.x += car->speed * sinf(car->rotation.y);
    car->position.z += car->speed * cosf(car->rotation.y);

    //Handle popups
    if(car->popupState == POPUPS_MDOWN)
    {
        if(car->popupPos < POPUP_POS_DOWN)
        {
            car->popupPos += POPUP_SPEED * ticks;
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
            car->popupPos -= POPUP_SPEED * ticks;
        }
        else
        {
            car->popupState = POPUPS_UP;
            car->popupPos = POPUP_POS_UP;
        }
    }

    if(car->gear == 1 && car->onGround > 3)
    {
        car->skids.timer += ticks;
        if(car->skids.timer > 100)
        {
            car->skids.timer = 0;
            car->skids.positions[car->skids.index++] = car->wheelPositions[2];
            car->skids.positions[car->skids.index++] = car->wheelPositions[3];
            if(car->skids.index == NUM_SKIDS)
            {
                car->skids.index = 0;
            }
        }
    }

    if(car->turboBoostCooldown > 0)
    {
        car->turboBoostCooldown -= ticks;
    }
}

void accelerate(Car* car, int8_t dir, uint16_t ticks)
{
    //Only start the car in first or reverse, and don't accelerate while in the air
    if((car->revs == 0 && car->gear > 1)) // || car->onGround < 4)
    {
        return;
    }

    //Accelerate, brake or idle brake
    if(dir > 0)
    {
        car->revs += (accelerationTable[car->gear] * ticks) / 20;
        car->braking = 0;
    }
    else if(dir < 0)
    {
        car->revs -= (BRAKING * ticks) / 20;
        car->braking = 1;
    }
    else
    {
        car->revs -= (IDLE_BRAKING * ticks) / 20;
        car->braking = 0;
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

void steer(Car* car, int8_t dir, uint16_t ticks)
{
    if(car->speedKPH != 0)
    {
        //Forward gear
        if(car->gear > 0)
        {
            car->rotation.y += dir * (ticks / 1000.0f);
        }
        //Reverse
        else
        {
            car->rotation.y -= dir * (ticks / 1000.0f);
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
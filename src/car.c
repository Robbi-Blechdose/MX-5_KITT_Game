#include "car.h"
#include "resources.h"

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
    car->position.y = spFloatToFixed(1.0f);
    car->position.z = spFloatToFixed(-29.0f);
    car->rotation.y = spDiv(-SP_PI, spIntToFixed(2));
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
	spTranslate(car->position.x, car->position.y, car->position.z);
	spRotateX(car->rotation.x);
	spRotateY(car->rotation.y);
	spRotateZ(car->rotation.z);
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

void calcCar(Car* car, Uint32 steps)
{
    car->speed = spMul(spDiv(spIntToFixed(car->revs), spIntToFixed(speedTable[car->gear])),
                       spDiv(spIntToFixed(steps), spIntToFixed(750)));
    car->speedKPH = spFixedToInt(spMul(spDiv(spIntToFixed(car->revs), spIntToFixed(speedTable[car->gear])),
                                       spIntToFixed(40)));

    //Calculate x and z position
    car->position.x += spMul(car->speed, spSin(car->rotation.y));
    car->position.z += spMul(car->speed, spCos(car->rotation.y));

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
        car->rotation.y += spMul(spIntToFixed(dir), spDiv(spIntToFixed(steps), spIntToFixed(1000)));
        car->rotation.z = lerpFixed(car->rotation.z, spFloatToFixed(dir * 0.07f), spFloatToFixed(steps * 0.01f));
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
#include <math.h>

#include "camera.h"
#include "utils.h"

Vector3f cameraPos;
//x = pitch, y = yaw, z = roll
Vector3f cameraRot;
Vector3f cameraInputRot;

float distance = 4.2f;
Sint32 rotYOffset = 0;

void initCamera()
{
    cameraInputRot.x = 0.45f;
}

void drawCameraPos()
{
	spIdentity();
    spRotateX(spFloatToFixed(cameraRot.x));
    spRotateY(spFloatToFixed(cameraRot.y));
    spRotateZ(spFloatToFixed(cameraRot.z));
    spTranslate(spFloatToFixed(-cameraPos.x),
                spFloatToFixed(-cameraPos.y),
                spFloatToFixed(-cameraPos.z));
}

void calcCameraPos(Car *car, Uint32 steps)
{
    if(keyReleased(SP_BUTTON_SELECT))
    {
        rotYOffset = rotYOffset ? 0 : spFixedToFloat(SP_PI);
    }
    cameraInputRot.y = lerpf(cameraInputRot.y, (car->steering * -0.3f) + rotYOffset, steps * 0.02f);

    //Calculate horizontal distance from car
    float hDistance = distance * cos(cameraInputRot.x);
    //Calculate horizontal angle
    float hAngle = car->rotation.y + cameraInputRot.y;
    //Calculate x and z position
    cameraPos.x = car->position.x - (hDistance * sin(hAngle));
    cameraPos.z = car->position.z - (hDistance * cos(hAngle));
    //Calculate y position
    cameraPos.y = car->position.y + (distance * sin(cameraInputRot.x));

    //Copy pitch
    cameraRot.x = cameraInputRot.x;
    //Calculate yaw
    cameraRot.y = spFixedToFloat(SP_PI) - hAngle;
}
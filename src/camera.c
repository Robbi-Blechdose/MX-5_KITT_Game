#include "camera.h"
#include "utils.h"

Vector cameraPos;
//x = pitch, y = yaw, z = roll
Vector cameraRot;
Vector cameraInputRot;

Sint32 distance = spFloatToFixed(4.2f);
Sint32 rotYOffset = 0;

void initCamera()
{
    cameraInputRot.x = spFloatToFixed(0.45f);
}

void drawCameraPos()
{
	spIdentity();
    spRotateX(cameraRot.x);
    spRotateY(cameraRot.y);
    spRotateZ(cameraRot.z);
    spTranslate(-cameraPos.x, -cameraPos.y, -cameraPos.z);
}

void calcCameraPos(Car *car, Uint32 steps)
{
    if(keyReleased(SP_BUTTON_SELECT))
    {
        rotYOffset = rotYOffset ? 0 : SP_PI;
    }
    cameraInputRot.y = lerpFixed(cameraInputRot.y,
                                    spFloatToFixed(car->steering * -0.3f) + rotYOffset,
                                    spFloatToFixed(steps * 0.02f));

    //Calculate horizontal distance from car
    Sint32 hDistance = spMul(distance, spCos(cameraInputRot.x));
    //Calculate horizontal angle
    Sint32 hAngle = car->rotation.y + cameraInputRot.y;
    //Calculate x and z position
    cameraPos.x = car->position.x - spMul(hDistance, spSin(hAngle));
    cameraPos.z = car->position.z - spMul(hDistance, spCos(hAngle));
    //Calculate y position
    cameraPos.y = car->position.y + spMul(distance, spSin(cameraInputRot.x));

    //Copy pitch
    cameraRot.x = cameraInputRot.x;
    //Calculate yaw
    cameraRot.y = SP_PI - hAngle;
}
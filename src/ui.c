#include "ui.h"

SDL_Surface *mainUITexture;
SDL_Surface *popupsTexture;
SDL_Surface *gearsTexture;
SDL_Surface *numbersTexture;

uint8_t gearPos[6][2] = {
    {227, 227},
    {203, 203},
    {203, 227},
    {215, 203},
    {215, 227},
    {227, 203}
};

void initGameUI()
{
	mainUITexture = spLoadSurface("res/UI/MainUI.png");
    popupsTexture = spLoadSurface("res/UI/Popups.png");
    gearsTexture = spLoadSurface("res/UI/Gears.png");
    numbersTexture = spLoadSurface("res/UI/Numbers.png");
}

void deleteGameUI()
{
    spDeleteSurface(mainUITexture);
    spDeleteSurface(popupsTexture);
    spDeleteSurface(gearsTexture);
    spDeleteSurface(numbersTexture);
}

void drawGameUI(Car *car)
{
    spSetAlphaTest(1);
    spSetHorizontalOrigin(SP_LEFT);
    spSetVerticalOrigin(SP_TOP);
    //Main UI texture
    spBlitSurface(0, 200, 5, mainUITexture);

    //Popup state
    if(car->popupState == POPUPS_DOWN || car->popupState == POPUPS_MDOWN)
    {
        spBlitSurfacePart(115, 224, 0, popupsTexture, 10, 0, 10, 10);
    }
    else
    {
        spBlitSurfacePart(115, 206, 0, popupsTexture, 0, 0, 10, 10);
    }

    //Gears
    spBlitSurfacePart(gearPos[car->gear][0], gearPos[car->gear][1], 0, gearsTexture, 10 * car->gear, 0, 10, 10);

    //Revs
    int16_t rpmTemp = car->revs / 100;
    uint8_t digit2 = rpmTemp % 10;
    uint8_t digit1 = rpmTemp / 10;
    spBlitSurfacePart(53, 216, 0, numbersTexture, 12 * digit1, 0, 12, 16);
    spBlitSurfacePart(66, 216, 0, numbersTexture, 12 * digit2, 0, 12, 16);

    //Speed
    int16_t speedTemp = car->speedKPH < 0 ? -car->speedKPH : car->speedKPH;
    uint8_t digit3 = speedTemp % 10;
    speedTemp /= 10;
    digit2 = speedTemp % 10;
    digit1 = speedTemp / 10;
    spBlitSurfacePart(146, 216, 0, numbersTexture, 12 * digit1, 0, 12, 16);
    spBlitSurfacePart(159, 216, 0, numbersTexture, 12 * digit2, 0, 12, 16);
    spBlitSurfacePart(172, 216, 0, numbersTexture, 12 * digit3, 0, 12, 16);
}

void drawFPS(spFontPointer font)
{
    char buffer[8];
	sprintf(buffer, "FPS: %i", spGetFPS());
	spSetAlphaTest(1);
	spFontDraw(1, 1, 0, buffer, font);
	spFlip();
	spResetZBuffer();
}
#include "ui.h"
#include "utils.h"

SDL_Surface *menuTexture;

SDL_Surface *mainUITexture;
SDL_Surface *popupsTexture;
SDL_Surface *gearsTexture;
SDL_Surface *numbersTexture;
SDL_Surface *barTexture;

uint8_t gearPos[6][2] = {
    {227, 227},
    {203, 203},
    {203, 227},
    {215, 203},
    {215, 227},
    {227, 203}
};

void initMenu()
{
	//menuTexture = spLoadSurface("res/UI/MenuBG.png");
}

void initGameUI()
{
	mainUITexture = loadPNG("res/UI/MainUI.png");
    popupsTexture = loadPNG("res/UI/Popups.png");
    gearsTexture = loadPNG("res/UI/Gears.png");
    numbersTexture = loadPNG("res/UI/Numbers.png");
    barTexture = loadPNG("res/UI/TurboBoostBar.png");
}

void deleteMenu()
{
    SDL_FreeSurface(menuTexture);
}

void deleteGameUI()
{
    SDL_FreeSurface(mainUITexture);
    SDL_FreeSurface(popupsTexture);
    SDL_FreeSurface(gearsTexture);
    SDL_FreeSurface(numbersTexture);
    SDL_FreeSurface(barTexture);
}

int calcMenu()
{
    /**
    if(spGetInput()->button[SP_BUTTON_START])
    {
        return 1;
    }
    **/
}


void drawMenu()
{
    /**
    spClearTarget(0);
	spSetAlphaTest(0);

    spSetHorizontalOrigin(SP_LEFT);
    spSetVerticalOrigin(SP_TOP);

    //Draw menu
    spBlitSurface(0, 0, 5, menuTexture);

	spFlip();
	spResetZBuffer();
    **/
}

void drawGameUI(SDL_Surface* screen, Car *car)
{
    //Main UI texture
    SDL_Rect src = {.x = 0, .y = 0};
    SDL_Rect dst = {.x = 0, .y = 200};
    SDL_BlitSurface(mainUITexture, NULL, screen, &dst);

    //Popup state
    if(car->popupState == POPUPS_DOWN || car->popupState == POPUPS_MDOWN)
    {
        dst = (SDL_Rect) {.x = 115, .y = 224};
        src = (SDL_Rect) {.x = 10, .y = 0, .w = 10, .h = 10};
        SDL_BlitSurface(popupsTexture, &src, screen, &dst);
    }
    else
    {
        dst = (SDL_Rect) {.x = 115, .y = 206};
        src = (SDL_Rect) {.x = 0, .y = 0, .w = 10, .h = 10};
        SDL_BlitSurface(popupsTexture, &src, screen, &dst);
    }

    //Gears
    dst = (SDL_Rect) {.x = gearPos[car->gear][0], .y = gearPos[car->gear][1]};
    src = (SDL_Rect) {.x = 10 * car->gear, .y = 0, .w = 10, .h = 10};
    SDL_BlitSurface(gearsTexture, &src, screen, &dst);

    //Revs
    int16_t rpmTemp = car->revs / 100;
    uint8_t digit2 = rpmTemp % 10;
    uint8_t digit1 = rpmTemp / 10;

    dst = (SDL_Rect) {.x = 53, .y = 216};
    src = (SDL_Rect) {.x = 12 * digit1, .y = 0, .w = 12, .h = 16};
    SDL_BlitSurface(numbersTexture, &src, screen, &dst);
    dst = (SDL_Rect) {.x = 66, .y = 216};
    src = (SDL_Rect) {.x = 12 * digit2, .y = 0, .w = 12, .h = 16};
    SDL_BlitSurface(numbersTexture, &src, screen, &dst);

    //Speed
    int16_t speedTemp = car->speedKPH < 0 ? -car->speedKPH : car->speedKPH;
    uint8_t digit3 = speedTemp % 10;
    speedTemp /= 10;
    digit2 = speedTemp % 10;
    digit1 = speedTemp / 10;
    dst = (SDL_Rect) {.x = 146, .y = 216};
    src = (SDL_Rect) {.x = 12 * digit1, .y = 0, .w = 12, .h = 16};
    SDL_BlitSurface(numbersTexture, &src, screen, &dst);
    dst = (SDL_Rect) {.x = 159, .y = 216};
    src = (SDL_Rect) {.x = 12 * digit2, .y = 0, .w = 12, .h = 16};
    SDL_BlitSurface(numbersTexture, &src, screen, &dst);
    dst = (SDL_Rect) {.x = 172, .y = 216};
    src = (SDL_Rect) {.x = 12 * digit3, .y = 0, .w = 12, .h = 16};
    SDL_BlitSurface(numbersTexture, &src, screen, &dst);

    //Turbo boost
    int16_t turboBoostTemp = 15 - (car->turboBoostCooldown / 2000.0f) * 15;
    int i;
    for(i = 0; i < turboBoostTemp; i++)
    {
        dst = (SDL_Rect) {.x = 19, .y = 233 - (i * 2)};
        SDL_BlitSurface(barTexture, NULL, screen, &dst);
    }
}

void drawFPS(uint16_t fps)
{
    char buffer[9];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 1, 1, 0xFF);
}
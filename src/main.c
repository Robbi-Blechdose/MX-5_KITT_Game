#include <sparrow3d.h>
#include <SDL_image.h>
#include <math.h>
#include <string.h>
#include <strings.h>

#include "map.h"
#include "car.h"
#include "utils.h"
#include "resources.h"
#include "camera.h"
#include "ui.h"
#include "mission.h"

#define MIN_WAIT 0 //50fps

SDL_Surface *screen;
spFontPointer font;

Mission mission;
Car playerCar;

void resize(Uint16 w, Uint16 h)
{
	spSelectRenderTarget(spGetWindowSurface());
	//Setup of the new/resized window
	//spSetPerspective( 75.0, ( float )spGetWindowSurface()->w / ( float )spGetWindowSurface()->h, 1.0f, 100.0f );
    spSetPerspective(75, screen->w / screen->h, 1.5f, 80.0f);

	//Font Loading
	if(font)
		spFontDelete(font);
	font = spFontLoad("./font/Play-Bold.ttf", spFixedToInt(10 * spGetSizeFactor()));
	spFontSetShadeColor(0);
	spFontAdd(font, SP_FONT_GROUP_ASCII, 65535); //whole ASCII
	spFontAdd(font, "äüöÄÜÖßẞ", 65535); //German stuff (same like spFontAdd( font, SP_FONT_GROUP_GERMAN, 0 ); )
	spFontAddBorder(font, 0);
}

void drawFrame()
{
    spClearTarget(BACKGROUND_COLOR);
	spSetZSet(1);
	spSetZTest(1);
	spSetAlphaTest(0);

    drawCameraPos();

    drawMap(&mission.map);
    drawCar(&playerCar);
    //TODO: Figure out how to only draw the populated array entries...
    drawCar(&mission.enemies[0]);

    drawGameUI(&playerCar);
    drawFPS(font);
	spFlip();
	spResetZBuffer();
}

int calcFrame(Uint32 steps)
{
    if(spGetInput()->button[SP_BUTTON_QUIT])
    {
        return 1;
    }

    accelerate(&playerCar, -spGetInput()->axis[1], steps);
    steer(&playerCar, -spGetInput()->axis[0], steps);

    if(keyReleased(SP_BUTTON_R))
    {
        shiftUp(&playerCar);
    }
    else if(keyReleased(SP_BUTTON_L))
    {
        shiftDown(&playerCar);
    }

    if(keyReleased(SP_BUTTON_B))
    {
        turboBoost(&playerCar);
    }

    if(keyReleased(SP_BUTTON_A))
    {
        togglePopups(&playerCar);
    }

	calcCar(&playerCar, mission.map.mapMesh, steps);
    calcCameraPos(&playerCar, steps);

    updateKeys();

    //TODO: Figure out how to only calculate the populated array entries...
    accelerate(&mission.enemies[0], 1, steps);
    calcCar(&mission.enemies[0], mission.map.mapMesh, steps);

	return 0;
}

int main(int argc, char **argv)
{
    //Initialize Sparrow3d
	spSetDefaultWindowSize(240, 240);
	spInitCore();
	screen = spCreateDefaultWindow();
	resize(screen->w, screen->h);
	spSelectRenderTarget(screen);
	spUsePrecalculatedNormals(1);
	spSetAffineTextureHack(0);
    spEmulateBlendingWithPattern(2);
    spSetCulling(1);
	spSetLight(0);

    //Initialize SparrowSound
    spSoundInit();
    spSoundSetMusic(MUSIC);
    spSoundPlayMusic(0, -1);

    //Show menu
    initMenu();
	spLoop(drawMenu, calcMenu, MIN_WAIT, resize, NULL);
    deleteMenu();

    //Initialize game
    loadMission(&mission, 0);
    initCurrentMission(&mission, &playerCar);
    initGameUI();
    initCamera();

    //Run main loop
	spLoop(drawFrame, calcFrame, MIN_WAIT, resize, NULL);

    //Game cleanup
    deleteCar(&playerCar);
    deleteMap();
    deleteGameUI();

    //SparrowSound cleanup
    spSoundStopMusic(0);
    spSoundQuit();
    //Sparrow3d cleanup
	spFontDelete(font);
	spQuitCore();
	return 0;
}
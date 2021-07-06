#define CHAD_API_IMPL
#include "zbuffer.h"
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

#define WINX 240
#define WINY 240

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;
SDL_Event event;
uint16_t fps;

uint8_t running = 1;

Mission mission;
Car playerCar;

void drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    drawCameraPos();

    drawMap(&mission.map);
    drawCar(&playerCar);
    //TODO: Figure out how to only draw the populated array entries...
    drawCar(&mission.enemies[0]);

    drawFPS(fps);

    if(SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }
    ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
	if(SDL_MUSTLOCK(screen))
    {
		SDL_UnlockSurface(screen);
    }
    drawGameUI(screen, &playerCar);
	SDL_Flip(screen);
}

void handleInput(uint16_t ticks)
{
    if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_a:
            {
                togglePopups(&playerCar);
                break;
            }
            case SDLK_b:
            {
                turboBoost(&playerCar);
                break;
            }
            case SDLK_n:
            {
                shiftUp(&playerCar);
                break;
            }
            case SDLK_m:
            {
                shiftDown(&playerCar);
                break;
            }
            case SDLK_k:
            {
                toggleCameraDirection();
                break;
            }
            case SDLK_q:
            {
                running = 0;
                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if(event.type == SDL_QUIT)
    {
        running = 0;
    }
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(WINX, WINY, 16, SDL_SWSURFACE);
	SDL_ShowCursor(SDL_DISABLE);

    IMG_Init(IMG_INIT_PNG);

    //Initialize TinyGL
	frameBuffer = ZB_open(WINX, WINY, ZB_MODE_5R6G5B, 0);
	glInit(frameBuffer);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.4f, 0.8f, 1.0f, 0);
	glClearDepth(1.0f);
    glViewport(0, 0, WINX, WINY);
	glTextSize(GL_TEXT_SIZE8x8);
	glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mat4 matrix = perspective(70, (float)WINX / (float)WINY, 1, 512);
	glLoadMatrixf(matrix.d);

    //Initialize SparrowSound
    //spSoundInit();
    //spSoundSetMusic(MUSIC);
    //spSoundPlayMusic(0, -1);

    //Show menu
    //initMenu();
	//spLoop(drawMenu, calcMenu, MIN_WAIT, resize, NULL);
    //deleteMenu();

    //Initialize game
    loadMission(&mission, 0);
    initCurrentMission(&mission, &playerCar);
    initGameUI();
    initCamera();

    //Run main loop
	uint32_t tNow = SDL_GetTicks();
	uint32_t tLastFrame = tNow;
    uint16_t ticks = 0;
    while(running)
    {
		tNow = SDL_GetTicks();
        ticks = tNow - tLastFrame;
        
        while(SDL_PollEvent(&event))
        {
            handleKeys(&event);
            handleInput(ticks);
        }

        if(keyPressed(U))
        {
            accelerate(&playerCar, 1, ticks);
        }
        else if(keyPressed(D))
        {
            accelerate(&playerCar, -1, ticks);
        }
        else
        {
            accelerate(&playerCar, 0, ticks);
        }
        if(keyPressed(L))
        {
            steer(&playerCar, 1, ticks);
        }
        else if(keyPressed(R))
        {
            steer(&playerCar, -1, ticks);
        }
        else
        {
            steer(&playerCar, 0, ticks);
        }

        calcCar(&playerCar, mission.map.mapMesh, ticks);
        calcCameraPos(&playerCar, ticks);

        //TODO: Figure out how to only calculate the populated array entries...
        accelerate(&mission.enemies[0], 1, ticks);
        calcCar(&mission.enemies[0], mission.map.mapMesh, ticks);

        drawFrame();

        /**
        uint16_t targetFPS = 50;
		if((1000 / targetFPS) > (SDL_GetTicks() - tNow))
        {
			SDL_Delay((1000 / targetFPS) - (SDL_GetTicks() - tNow)); // Yay stable framerate!
		}**/
		fps = 1000.0f / (float)(tNow - tLastFrame);
		tLastFrame = tNow;
    }

    //Game cleanup
    deleteCar(&playerCar);
    deleteMap();
    deleteGameUI();

    //SparrowSound cleanup
    //spSoundStopMusic(0);
    //spSoundQuit();
    
    //TinyGL cleanup
	ZB_close(frameBuffer);
	glClose();

	return 0;
}
#ifndef _MAP_H
#define _MAP_H

#include <sparrow3d.h>
#include <SDL.h>

typedef struct {
    SDL_Surface *mapTexture;
    spModelPointer mapMesh;
    SDL_Surface *treeTexture;
    spModelPointer treeMesh;
    SDL_Surface *rockTexture;
    spModelPointer rockMesh;
    SDL_Surface *signTexture;
    spModelPointer signMesh;
} Map;

void initMap(Map* map);
void deleteMap(Map* map);

void drawMap(Map* map);

#endif
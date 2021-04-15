#include "map.h"
#include "utils.h"

SDL_Surface *mapTexture;
spModelPointer mapMesh;
SDL_Surface *treeTexture;
spModelPointer treeMesh;
SDL_Surface *rockTexture;
spModelPointer rockMesh;

#define NUM_TREES 4
//Note: We only save the DIFFERENCE in position here
Vector trees[NUM_TREES] = {
	//-20, 0, -35
	{.x = spIntToFixed(-20), .y = 0, .z = spIntToFixed(-35)},
	//-30, 0, -36
	{.x = spIntToFixed(-10), .y = 0, .z = spIntToFixed(-1)},
	//-40, 0, -33
	{.x = spIntToFixed(-10), .y = 0, .z = spIntToFixed(3)},
	//30, 5, 40
	{.x = spIntToFixed(70), .y = spIntToFixed(5), .z = spIntToFixed(73)}
};

#define NUM_ROCKS 1
//Note: We only save the DIFFERENCE in position here
Vector rocks[NUM_ROCKS] = {
	//-29, 0, -15
	{.x = spIntToFixed(-29), .y = 0, .z = spIntToFixed(-15)}
};

void initMap()
{
	mapTexture = spLoadSurface("res/tex/Map_01.png");
	mapMesh = spMeshLoadObj("res/obj/Map_01.obj", mapTexture, 65535);
	treeTexture = spLoadSurface("res/tex/Tree.png");
	treeMesh = spMeshLoadObj("res/obj/Tree.obj", treeTexture, 65535);
	rockTexture = spLoadSurface("res/tex/Rock_01.png");
	rockMesh = spMeshLoadObj("res/obj/Rock_01.obj", rockTexture, 65535);
}

void deleteMap()
{
	spDeleteSurface(mapTexture);
	spMeshDelete(mapMesh);
	spDeleteSurface(treeTexture);
	spMeshDelete(treeMesh);
	spDeleteSurface(rockTexture);
	spMeshDelete(rockMesh);
}

void drawMap()
{
	spPushModelView();

	spMesh3D(mapMesh, 2);

	spSetAlphaTest(1);
	spSetCulling(0);
	int i;
	for(i = 0; i < NUM_TREES; i++)
	{
		spTranslate(trees[i].x, trees[i].y, trees[i].z);
		spMesh3D(treeMesh, 1);
	}
	spSetAlphaTest(0);
	spSetCulling(1);

	spPopModelView();
	spPushModelView();
	for(i = 0; i < NUM_ROCKS; i++)
	{
		spTranslate(rocks[i].x, rocks[i].y, rocks[i].z);
		spMesh3D(rockMesh, 1);
	}

	spPopModelView();
}
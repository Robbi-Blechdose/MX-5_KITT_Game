#include "map.h"
#include "utils.h"
#include "resources.h"

#define NUM_TREES 5
//Note: We only save the DIFFERENCE in position here
Vector trees[NUM_TREES] = {
	//-20, 0, -35
	{.x = spIntToFixed(-20), .y = 0, .z = spIntToFixed(-35)},
	//-30, 0, -36
	{.x = spIntToFixed(-10), .y = 0, .z = spIntToFixed(-1)},
	//-40, 0, -33
	{.x = spIntToFixed(-10), .y = 0, .z = spIntToFixed(3)},
	//30, 5, 40
	{.x = spIntToFixed(70), .y = spIntToFixed(5), .z = spIntToFixed(73)},
	//32, 0, 13
	{.x = spIntToFixed(2), .y = spIntToFixed(-5), .z = spIntToFixed(-27)}
};

#define NUM_ROCKS 1
//Note: We only save the DIFFERENCE in position here
Vector rocks[NUM_ROCKS] = {
	//-29, 0, -15
	{.x = spIntToFixed(-29), .y = 0, .z = spIntToFixed(-15)}
};

#define NUM_SIGNS 6
Vector signs[NUM_SIGNS] = {
	//Group in first corner
	{.x = spFloatToFixed(-38), .y = 0, .z = spFloatToFixed(-32)},
	{.x = spFloatToFixed(-41), .y = 0, .z = spFloatToFixed(-29)},
	{.x = spFloatToFixed(-43.5f), .y = 0, .z = spFloatToFixed(-26.5f)},
	//Group in corner behind ramps
	{.x = spFloatToFixed(6), .y = 0, .z = spFloatToFixed(24)},
	{.x = spFloatToFixed(9), .y = 0, .z = spFloatToFixed(21)},
	{.x = spFloatToFixed(12), .y = 0, .z = spFloatToFixed(18)}
};

//Y rotation for signs
Sint32 signRot[NUM_SIGNS] = {
	//Group in first corner
	spFloatToFixed(-3.926f),
	spFloatToFixed(-3.926f),
	spFloatToFixed(-3.926f),
	//Group in corner behind ramps
	spFloatToFixed(-0.567f),
	spFloatToFixed(-0.785f),
	spFloatToFixed(-1.003f)
};

void initMap(Map* map)
{
	map->mapTexture = spLoadSurface(MAP01_TEXTURE);
	map->mapMesh = spMeshLoadObj(MAP01_MESH, map->mapTexture, 65535);
	map->treeTexture = spLoadSurface(TREE_TEXTURE);
	map->treeMesh = spMeshLoadObj(TREE_MESH, map->treeTexture, 65535);
	map->rockTexture = spLoadSurface(ROCK_TEXTURE);
	map->rockMesh = spMeshLoadObj(ROCK_MESH, map->rockTexture, 65535);
	map->signTexture = spLoadSurface(SIGN_TEXTURE);
	map->signMesh = spMeshLoadObj(SIGN_MESH, map->signTexture, 65535);
}

void deleteMap(Map* map)
{
	spDeleteSurface(map->mapTexture);
	spMeshDelete(map->mapMesh);
	spDeleteSurface(map->treeTexture);
	spMeshDelete(map->treeMesh);
	spDeleteSurface(map->rockTexture);
	spMeshDelete(map->rockMesh);
	spDeleteSurface(map->signTexture);
	spMeshDelete(map->signMesh);
}

void drawMap(Map* map)
{
	spMesh3D(map->mapMesh, 2);

	spPushModelView();
	int i;
	for(i = 0; i < NUM_ROCKS; i++)
	{
		spTranslate(rocks[i].x, rocks[i].y, rocks[i].z);
		spMesh3D(map->rockMesh, 1);
	}

	spPopModelView();
	spPushModelView();

	spSetAlphaTest(1);
	spSetCulling(0);

	for(i = 0; i < NUM_TREES; i++)
	{
		spTranslate(trees[i].x, trees[i].y, trees[i].z);
		spMesh3D(map->treeMesh, 1);
	}

	spPopModelView();

	for(i = 0; i < NUM_SIGNS; i++)
	{
		spPushModelView();
		spTranslate(signs[i].x, signs[i].y, signs[i].z);
		spRotateY(signRot[i]);
		spMesh3D(map->signMesh, 1);
		spPopModelView();
	}
	
	spSetAlphaTest(0);
	spSetCulling(1);
}
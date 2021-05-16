#include "map.h"
#include "utils.h"
#include "resources.h"
#include "mission.h"

SDL_Surface *mapTexture;
spModelPointer mapMesh;
SDL_Surface *treeTexture;
spModelPointer treeMesh;
SDL_Surface *rockTexture;
spModelPointer rockMesh;
SDL_Surface *signTexture;
spModelPointer signMesh;

void initMap(Map* map)
{
	mapTexture = spLoadSurface(map->terrainTexture);
	mapMesh = spMeshLoadObj(map->terrainMesh, mapTexture, 65535);
	treeTexture = spLoadSurface(TREE_TEXTURE);
	treeMesh = spMeshLoadObj(TREE_MESH, treeTexture, 65535);
	rockTexture = spLoadSurface(ROCK_TEXTURE);
	rockMesh = spMeshLoadObj(ROCK_MESH, rockTexture, 65535);
	signTexture = spLoadSurface(SIGN_TEXTURE);
	signMesh = spMeshLoadObj(SIGN_MESH, signTexture, 65535);

	map->mapMesh = mapMesh;
}

void deleteMap()
{
	spDeleteSurface(mapTexture);
	spMeshDelete(mapMesh);
	spDeleteSurface(treeTexture);
	spMeshDelete(treeMesh);
	spDeleteSurface(rockTexture);
	spMeshDelete(rockMesh);
	spDeleteSurface(signTexture);
	spMeshDelete(signMesh);
}

void drawMap(Map* map)
{
	spMesh3D(mapMesh, 2);

	spPushModelView();
	int i;
	for(i = 0; i < map->numRocks; i++)
	{
		spTranslate(map->rocks[i].x, map->rocks[i].y, map->rocks[i].z);
		spMesh3D(rockMesh, 1);
	}

	spPopModelView();
	spPushModelView();

	spSetAlphaTest(1);
	spSetCulling(0);

	for(i = 0; i < map->numTrees; i++)
	{
		spTranslate(map->trees[i].x, map->trees[i].y, map->trees[i].z);
		spMesh3D(treeMesh, 1);
	}

	spPopModelView();

	for(i = 0; i < map->numSigns; i++)
	{
		spPushModelView();
		spTranslate(map->signs[i].x, map->signs[i].y, map->signs[i].z);
		spRotateY(map->signRot[i]);
		spMesh3D(signMesh, 1);
		spPopModelView();
	}
	
	spSetAlphaTest(0);
	spSetCulling(1);
}
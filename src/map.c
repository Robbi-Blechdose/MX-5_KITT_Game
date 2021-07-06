#include "map.h"
#include "utils.h"
#include "resources.h"
#include "mission.h"

GLuint mapTexture;
GLuint mapMesh;
GLuint treeTexture;
GLuint treeMesh;
GLuint rockTexture;
GLuint rockMesh;
GLuint signTexture;
GLuint signMesh;

model temp;

void initMap(Map* map)
{
	mapTexture = loadRGBTexture(map->terrainTexture);
	//mapMesh = loadModelList(map->terrainMesh);
	temp = loadModel(map->terrainMesh);
	map->mapMesh = &temp;
	mapMesh = createModelDisplayList(&temp);

	treeTexture = loadRGBTexture(TREE_TEXTURE);
	treeMesh = loadModelList(TREE_MESH);
	rockTexture = loadRGBTexture(ROCK_TEXTURE);
	rockMesh = loadModelList(ROCK_MESH);
	signTexture = loadRGBTexture(SIGN_TEXTURE);
	signMesh = loadModelList(SIGN_MESH);
}

void deleteMap()
{
	/**
	SDL_FreeSurface(treeTexture);
	spMeshDelete(treeMesh);
	SDL_FreeSurface(rockTexture);
	spMeshDelete(rockMesh);
	SDL_FreeSurface(signTexture);
	spMeshDelete(signMesh);
	**/
}

void drawMap(Map* map)
{
	int i;

	glBindTexture(GL_TEXTURE_2D, mapTexture);
	glCallList(mapMesh);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, rockTexture);
	for(i = 0; i < map->numRocks; i++)
	{
		glTranslatef(map->rocks[i].x, map->rocks[i].y, map->rocks[i].z);
		glCallList(rockMesh);
	}
	glPopMatrix();

	glPushMatrix();
    glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, treeTexture);
	for(i = 0; i < map->numTrees; i++)
	{
		glTranslatef(map->trees[i].x, map->trees[i].y, map->trees[i].z);
		glCallList(treeMesh);
	}
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, signTexture);
	for(i = 0; i < map->numSigns; i++)
	{
		glPushMatrix();
		glTranslatef(map->signs[i].x, map->signs[i].y, map->signs[i].z);
		glRotatef(RAD_TO_DEG(map->signRot[i]), 0, 1, 0);
		glCallList(signMesh);
		glPopMatrix();
	}
	
    glEnable(GL_CULL_FACE);
}
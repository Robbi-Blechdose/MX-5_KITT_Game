#include <SDL_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"
#include "utils.h"

float lerpf(float a, float b, float f)
{
    return a + f * (b - a);
}

int8_t lerpInt(int8_t a, int8_t b, int8_t f)
{
    return a + f * (b - a);
}

void addVec3f(Vector3f* result, Vector3f* a, Vector3f* b)
{
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void subVec3f(Vector3f* result, Vector3f* a, Vector3f* b)
{
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

void crossVec3f(Vector3f* result, Vector3f* a, Vector3f* b)
{
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

float dotVec3f(Vector3f* a, Vector3f* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

SDL_Surface* loadPNG(const char* path)
{
    SDL_Surface* loaded = IMG_Load(path);
    if(loaded != NULL)
    {
        SDL_Surface* converted = SDL_DisplayFormatAlpha(loaded);
        SDL_FreeSurface(loaded);
        if(converted != NULL)
        {
            return converted;
        }
    }
    printf("PNG loading failed for \"%s\".\n", path);
    return NULL;
}

model loadModel(const char* path)
{
    objraw omodel;
	model m = initmodel();
	omodel = tobj_load(path);

	if(!omodel.positions)
    {
		printf("No positions in model \"%s\".\n", path);
    }
	m = tobj_tomodel(&omodel);
    freeobjraw(&omodel);
	//printf("Has %ld points.\n", m.npoints);
    return m;
}

GLuint createModelDisplayList(model* model)
{
	GLuint ret = 0;
	if(!model->d)
    {
		return 0;
    }
	ret = glGenLists(1);
	glNewList(ret, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for(uint i = 0; i < model->npoints; i++)
    {
        //Colors
		if(model->c)
        {
			glColor3f(model->c[i].d[0], model->c[i].d[1], model->c[i].d[2]);
		}
        //Texcoords
		if(model->t)
        {
			glTexCoord2f(model->t[i].d[0], model->t[i].d[1]);
        }
        //Normals
		if(model->n)
        {
			glNormal3f(model->n[i].d[0], model->n[i].d[1], model->n[i].d[2]);
        }
		glVertex3f(model->d[i].d[0], model->d[i].d[1], model->d[i].d[2]);
	}
	glEnd();
	glEndList();
	return ret;
}

GLuint loadModelList(const char* path)
{
	model temp = loadModel(path);
	GLuint ret = createModelDisplayList(&temp);
	freemodel(&temp);
    return ret;
}

GLuint loadRGBTexture(unsigned char* path)
{
    int sw = 0, sh = 0, sc = 0; // sc goes unused.
    unsigned char* sourceData = stbi_load(path, &sw, &sh, &sc, 3);
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, sw, sh, 0, GL_RGB, GL_UNSIGNED_BYTE, sourceData);
	free(sourceData);
	return t;
}

uint8_t keys[13];

void handleKeys(SDL_Event* event)
{
    if(event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
    {
        return;
    }

    uint8_t state = 0;
    if(event->type == SDL_KEYDOWN)
    {
        state = 1;
    }
    switch(event->key.keysym.sym)
    {
        case SDLK_u:
        {
            keys[U] = state;
            break;
        }
        case SDLK_d:
        {
            keys[D] = state;
            break;
        }
        case SDLK_l:
        {
            keys[L] = state;
            break;
        }
        case SDLK_r:
        {
            keys[R] = state;
            break;
        }
    }
}

uint8_t keyPressed(Key key)
{
    return keys[key];
}

//Möller-Trumbore algorithm
int intersectTriangle(Vector3f* origin, Vector3f* direction, Vector3f* vert0, Vector3f* vert1, Vector3f* vert2,
                      float* t, float* u, float* v)
{
    Vector3f edge1, edge2, tvec, pvec, qvec;
    float det, invDet;

    //Find vectors for two edges sharing vert0
    subVec3f(&edge1, vert1, vert0);
    subVec3f(&edge2, vert2, vert0);

    //Begin calculating determinant
    crossVec3f(&pvec, direction, &edge2);

    //If determinant is near 0, the ray is in the triangle plane
    det = dotVec3f(&edge1, &pvec);

    //Cull variant for extra speed
    if(det < EPSILON)
    {
        return 0;
    }

    //Calculate distance from vert0 to ray origin
    subVec3f(&tvec, origin, vert0);

    //Calculate u parameter and test bounds
    *u = dotVec3f(&tvec, &pvec);
    if(*u < 0.0f || *u > det)
    {
        return 0;
    }

    //Prepare to test v parameter
    crossVec3f(&qvec, &tvec, &edge1);

    //Calculate v parameter and test bounds
    *v = dotVec3f(direction, &qvec);
    if(*v < 0.0f || *u + *v > det)
    {
        return 0;
    }

    //Calculate t, scale parameters, ray intersects triangle
    *t = dotVec3f(&edge2, &qvec);
    invDet = 1.0f / det;
    *t *= invDet;
    *u *= invDet;
    *v *= invDet;

    return 1;
}
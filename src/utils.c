#include "utils.h"

char button[20];

void updateKeys()
{
    int i;
    for(i = 0; i < 20; i++)
    {
        button[i] = spGetInput()->button[i];
    }
}

//char keyDown(uint8_t code);

char keyReleased(uint8_t code)
{
    if(!spGetInput()->button[code] && button[code])
    {
        return 1;
    }
    return 0;
}


Sint32 lerpf(float a, float b, float f)
{
    return a + f * (b - a);
}

Sint32 lerpFixed(Sint32 a, Sint32 b, Sint32 f)
{
    return a + spMul(f, b - a);
}

int8_t lerpInt(int8_t a, int8_t b, int8_t f)
{
    return a + f * (b - a);
}

void fixedVec3toVec3f(Vector3f* result, Vector* a)
{
    result->x = spFixedToFloat(a->x);
    result->y = spFixedToFloat(a->y);
    result->z = spFixedToFloat(a->z);
}

void fixedToVec3f(Vector3f* result, Sint32 x, Sint32 y, Sint32 z)
{
    result->x = spFixedToFloat(x);
    result->y = spFixedToFloat(y);
    result->z = spFixedToFloat(z);
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

#define EPSILON 0.0001f

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
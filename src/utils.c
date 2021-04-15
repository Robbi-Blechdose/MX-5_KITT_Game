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

Sint32 lerpFixed(Sint32 a, Sint32 b, Sint32 f)
{
    return a + spMul(f, b - a);
}

int8_t lerpInt(int8_t a, int8_t b, int8_t f)
{
    return a + f * (b - a);
}
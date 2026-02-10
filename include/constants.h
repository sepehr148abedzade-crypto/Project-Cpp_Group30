#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "SDL2/SDL.h"

void Get_information(){
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0,&DM);
    const int width = DM.w;
    const int height = DM.h;
}

const double PI = 3.1415926;
const double e = 2.78;
#endif //CONSTANTS_H

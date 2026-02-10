#include "constants.h"

int Get_width(){
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0,&DM);
    const int width = DM.w;
    return width;
}

int Get_height(){
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0,&DM);
    const int height = DM.h;
    return height;
}


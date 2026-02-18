#include "constants.h"
#include "SDL2/SDL.h"

int Get_width(){
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0,&DM);
    return DM.w;
}

int Get_height(){
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0,&DM);
    return DM.h;
}



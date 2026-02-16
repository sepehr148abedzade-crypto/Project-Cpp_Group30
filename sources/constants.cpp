#include "constants.h"
#include "Entity.h"
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

SDL_Rect stage = {1040,95,485,350};
bool Limit_CharacterX (Character &sprite){
    if(sprite.x <= (double)stage.w / 2 - sprite.width / 2 && sprite.x >= -(double)stage.w / 2 + sprite.width / 2) return true;
    return false;
}

bool Limit_CharacterY(Character &sprite){
    if(sprite.y <= (double)stage.h / 2 - sprite.height / 2 && sprite.y >= -(double)stage.h / 2 + sprite.height / 2) return true;
    return false;
}
#include "constants.h"
#include "Entity.h"
#include "SDL2/SDL.h"
bool isFileMenuOpen = false;
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

void Update_Sprite_Dimensions(Character &sprite) {
    sprite.width = sprite.size * 500;
    sprite.height = sprite.size * 500;
}

bool Limit_CharacterX(Character &sprite) {
    double realWidth = sprite.size * 500.0;
    double halfW = realWidth / 2.0;
    double limit = (double)stage.w / 2.0;

    if (sprite.x <= limit - halfW && sprite.x >= -limit + halfW) return true;

    if (sprite.x > limit - halfW) sprite.x = limit - halfW;
    if (sprite.x < -limit + halfW) sprite.x = -limit + halfW;
    return false;
}

bool Limit_CharacterY(Character &sprite) {
    double realHeight = sprite.size * 500.0;
    double halfH = realHeight / 2.0;
    double limit = (double)stage.h / 2.0;

    if (sprite.y <= limit - halfH && sprite.y >= -limit + halfH) return true;

    if (sprite.y > limit - halfH) sprite.y = limit - halfH;
    if (sprite.y < -limit + halfH) sprite.y = -limit + halfH;
    return false;
}
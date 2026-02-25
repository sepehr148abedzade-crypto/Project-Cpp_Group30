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
    double limit = (double)stage.w / 2 - (sprite.size * 500) / 2;
    if (sprite.x > limit) {
        sprite.x = limit;
        return false;
    }
    if (sprite.x < -limit) {
        sprite.x = -limit;
        return false;
    }
    return true;
}

bool Limit_CharacterY(Character &sprite) {
    double limit = (double)stage.h / 2 - (sprite.size * 500) / 2;
    if (sprite.y > limit) {
        sprite.y = limit;
        return false;
    }
    if (sprite.y < -limit) {
        sprite.y = -limit;
        return false;
    }
    return true;
}
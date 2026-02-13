#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>

struct Button{
    SDL_Rect rect;
    SDL_Texture* Label_texture;
    SDL_Color first_color;
    SDL_Color second_color;
    bool is_mouse_on;
};
extern Button file_button;

void Init_Button();

#endif //ENTITY_H

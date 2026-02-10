#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include "SDL2/SDL.h"


struct Button{
    SDL_Rect rect;
    SDL_Color first_color;
    SDL_Color second_color;
    bool is_mouse_on;
};

struct Code_Button{
    int x=30, y, r=15;
    SDL_Color color;
    std::string label;
};

#endif //ENTITY_H

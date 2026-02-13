#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <string>

using namespace std;

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

struct Code_button{
    int x=30, y, r=15;
    SDL_Color color;
    string label;
    bool is_mouse_on;
    SDL_Texture* labelTexture;
};
extern Code_button categories[8];
void Init_Categories(SDL_Renderer* renderer, TTF_Font* font);

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

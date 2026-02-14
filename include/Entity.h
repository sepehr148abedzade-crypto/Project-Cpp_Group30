#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <string>

using namespace std;

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

struct Blocks{
    string id;
    SDL_Rect rect;
    SDL_Texture* image;
    string value;

    Blocks* next = nullptr;
};



struct Button{
    SDL_Rect rect={0,0,0,0};
    int radius ;
    SDL_Texture* Label_texture;
    SDL_Color first_color;
    SDL_Color second_color;
    bool is_mouse_on;
    bool is_mouse_click_on;
};
extern Button Load_button;
extern Button Top_button;
extern Button categories[8];

void Init_Button();
void Init_Load_button();
void Init_code_button(SDL_Renderer* renderer, TTF_Font* font);

#endif //ENTITY_H

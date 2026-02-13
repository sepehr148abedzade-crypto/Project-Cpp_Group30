#pragma once
#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include <SDL2/SDL.h>
#include <Entity.h>
#include <constants.h>
//#include <Game.h>

struct Button;
//SDL_Color CodeBar_colors[] = {
//        {76, 151, 255},
//        {153, 102, 255},
//        {214, 91, 229},
//        {255, 191, 0},
//        {255, 171, 25},
//        {76, 191, 230},
//        {89, 192, 89},
//        {255, 140, 26}
//};
extern SDL_Color white;


SDL_Color Hex_To_rgb(uint32_t hexcolor);
void Draw_BlueBar_Top(SDL_Renderer* renderer,int width,SDL_Texture* logo);
void Draw_Button(SDL_Renderer* renderer,Button button,SDL_Texture* texture);
void Draw_CodeBar(SDL_Renderer* renderer, Code_button* categories);

#endif //GRAPHIC_ELEMENT_H

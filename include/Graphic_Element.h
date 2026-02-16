#pragma once
#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include <SDL2/SDL.h>
#include <Entity.h>
#include <constants.h>
#include "Asset_Loader.h"
//#include <Game.h>

extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);


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
bool Is_mouse_on(int x,int y,int w,int h);
void Draw_loading_window(SDL_Renderer* renderer,Button button,SDL_Texture* texture);
void Draw_BlueBar_Top(SDL_Renderer* renderer,int width,SDL_Texture* logo);
void Draw_Top_Button(SDL_Renderer* renderer,Button button,SDL_Texture* texture);
void Draw_CodeBar_Item(SDL_Renderer* renderer, Button code_button[]);
void Draw_CodeBar(SDL_Renderer* renderer);
void Draw_Menu_Blocks(SDL_Renderer* renderer);
void Draw_RunningBar(SDL_Renderer* renderer);
void Draw_Character_Show_Bar(SDL_Renderer* renderer);
void Draw_Information_of_Character(SDL_Renderer* renderer);
void Draw_Stage_Bar(SDL_Renderer* renderer);
void Draw_Character(SDL_Renderer* renderer,Character &sprite);
void Handle_event_for_code_button(SDL_Event &e);
void Handle_event_for_motion_sprite(SDL_Event &e,Character &sprite);
#endif //GRAPHIC_ELEMENT_H

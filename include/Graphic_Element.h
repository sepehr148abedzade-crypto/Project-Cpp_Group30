#pragma once
#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include <SDL2/SDL.h>
#include <Entity.h>
#include <constants.h>
#include <map>
//#include <Game.h>

struct Button;

extern std::vector<Blocks> active_blocks;
extern std::vector<Blocks> menuBlocks;
extern std::map<std::string, BlockTemplate> blockMap;

extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font);

extern SDL_Color white;

void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font);
SDL_Color Hex_To_rgb(uint32_t hexcolor);
void Draw_BlueBar_Top(SDL_Renderer* renderer,int width,SDL_Texture* logo);
void Draw_Button(SDL_Renderer* renderer,Button button,SDL_Texture* texture);
void Draw_CodeBar(SDL_Renderer* renderer, Code_button* categories);

#endif //GRAPHIC_ELEMENT_H

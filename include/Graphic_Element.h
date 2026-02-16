#pragma once
#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include <SDL2/SDL.h>
#include <Entity.h>
#include <constants.h>
#include "Asset_Loader.h"
#include <vector>
//#include <Game.h>

extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
extern std::vector<BackdropItem> libraryItems;
extern bool isLibraryOpen;

struct Button;

extern SDL_Color white;


SDL_Color Hex_To_rgb(uint32_t hexcolor);
bool Is_mouse_on(int x,int y,int w,int h);
void Drawtext(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color, bool center);
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
void DrawBackdropPanel(SDL_Renderer* renderer, TTF_Font* font);
void DrawBackdropThumbnail(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect area);
void DrawBackdropCircleButton(SDL_Renderer* renderer);
void UpdateMenuState();
void DrawBackdropLibrary(SDL_Renderer* renderer, TTF_Font* font);
#endif //GRAPHIC_ELEMENT_H

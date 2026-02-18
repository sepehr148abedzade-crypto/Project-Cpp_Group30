#pragma once
#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include <SDL2/SDL.h>
#include <Entity.h>
#include <constants.h>
#include "Asset_Loader.h"
#include <vector>
//#include <SDL2/SDL_ttf.h>
//#include <Game.h>



extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
extern std::vector<BackdropItem> libraryItems;
extern bool isLibraryOpen;
extern SDL_Texture* currentBackdropTexture;
extern bool isStageSelected;
enum Tab { CODE, BACKDROPS, SOUNDS , COSTUMES};
extern Tab currentTab;
extern std::vector<Backdrop> projectBackdrops;
extern int selectedBackdropIndex;
extern int backdropScrollY;

void ApplyPen(SDL_Texture* target, int x, int y,SDL_Renderer* renderer);
void ApplyEraser(SDL_Texture* target, int x, int y,SDL_Renderer* renderer);
void ApplyFill(SDL_Texture* target,SDL_Renderer* renderer);
void ApplyTextToTexture(SDL_Texture* target, string text, int x, int y, SDL_Renderer* renderer, TTF_Font* font);
SDL_Texture* GetCurrentLayer();
SDL_Texture* ConvertToEditable(SDL_Texture* source, SDL_Renderer* renderer);
SDL_Texture* MakeTextureEditable(SDL_Renderer* renderer, SDL_Texture* sourceTex);
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
void DrawLineOnTexture(SDL_Texture* target, int x1, int y1, int x2, int y2, SDL_Renderer* renderer, bool isEraser);
void ApplyPen(SDL_Texture* target, int x, int y, SDL_Renderer* renderer);
void ClearCanvas(SDL_Texture* target, SDL_Renderer* renderer);
void ApplyEraser(SDL_Texture* target, int x, int y, SDL_Renderer* renderer);
void ApplyFill(SDL_Texture* target, SDL_Renderer* renderer);
void Draw_Menu_Blocks(SDL_Renderer* renderer);
void Draw_RunningBar(SDL_Renderer* renderer);
void Draw_Character_Show_Bar(SDL_Renderer* renderer);
void Draw_Information_of_Character(SDL_Renderer* renderer);
void Draw_Stage_Bar(SDL_Renderer* renderer, TTF_Font* font);
void DrawBackdropPanel(SDL_Renderer* renderer, TTF_Font* font);
void DrawBackdropThumbnail(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect area);
void Draw_Image_Editor(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* currentTex, string itemName);
void DrawBackdropCircleButton(SDL_Renderer* renderer);
void DrawBackdropSubMenu(SDL_Renderer* renderer);
void Draw_Backdrop_List_Sidebar(SDL_Renderer* renderer, TTF_Font* font);
void UpdateMenuState();
void DrawBackdropLibrary(SDL_Renderer* renderer, TTF_Font* font);
#endif //GRAPHIC_ELEMENT_H

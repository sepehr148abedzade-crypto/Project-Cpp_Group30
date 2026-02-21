#pragma once
#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include <SDL2/SDL.h>
#include <Entity.h>
#include <constants.h>
#include "Asset_Loader.h"
#include <vector>
//#include <SDL2/SDL_ttf.h>
#include <Game.h>


extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
extern std::vector<BackdropItem> libraryItems;
extern bool isLibraryOpen;
extern bool isStageSelected;
extern int backdropScrollY;
extern SDL_Color white;

enum Tab { CODE, BACKDROPS, SOUNDS , COSTUMES};
extern Tab currentTab;

SDL_Texture* GetCurrentLayer();
SDL_Texture* MakeTextureEditable(SDL_Renderer* renderer, SDL_Texture* sourceTex);

struct Button;

int calculatingWidthBlock (BlockTemplate& BT,vector<string>&value,TTF_Font* font );
int Draw_label(int current_x,SDL_Renderer* renderer,TTF_Font* font ,string text, int y,SDL_Color color );

SDL_Color GetBlockColor(Block_category cat);
SDL_Color Hex_To_rgb(uint32_t hexcolor);

bool Is_mouse_on(int x,int y,int w,int h);

void ApplyTextToTexture(SDL_Texture* target, string text, int x, int y, SDL_Renderer* renderer, TTF_Font* font);
void Drawtext(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color, bool center);
void Draw_loading_window(SDL_Renderer* renderer,Button button,SDL_Texture* texture);
void Draw_BlueBar_Top(SDL_Renderer* renderer,int width,SDL_Texture* logo);
void Draw_Top_Button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Draw_flag_and_stop_button(SDL_Renderer* renderer,Button &button1,Button &button2,SDL_Texture* texture1,SDL_Texture* texture2);
void Draw_CodeBar_Item(SDL_Renderer* renderer, Button code_button[]);
void Draw_CodeBar(SDL_Renderer* renderer);
void ClearCanvas(SDL_Texture* target, SDL_Renderer* renderer);
void Draw_Menu_Blocks(SDL_Renderer* renderer,TTF_Font* font);
void Draw_RunningBar(SDL_Renderer* renderer);
void DrawSimpleBlocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate& BT,vector<string>& values , SDL_Color color,TTF_Font*font, Blocks* block );
void Draw_Character_Show_Bar(SDL_Renderer* renderer);
void Draw_Information_of_Character(SDL_Renderer* renderer);
void Draw_Stage_Bar(SDL_Renderer* renderer, TTF_Font* font);
void Draw_Character(SDL_Renderer* renderer,Character &sprite);
void Handle_event_for_code_button(SDL_Event &e);
void Handle_event_for_flag_button(SDL_Event &e,Button &button);
void Handle_event_for_stop_button(SDL_Event &e,Button &button);
void Handle_event_for_motion_sprite(SDL_Event &e,Character &sprite);
void Draw_size_report(SDL_Renderer* renderer,TTF_Font* font,Character &sprite);
void Draw_time_report(SDL_Renderer* renderer,TTF_Font* font,Uint32 time);
void Draw_costume_report(SDL_Renderer* renderer,TTF_Font* font,Character &sprite);
void Draw_talking_box(SDL_Renderer* renderer,TTF_Font* font,Character &sprite);
void Draw_thinking_box(SDL_Renderer* renderer,TTF_Font* font,Character &sprite);
void DrawBackdropPanel(SDL_Renderer* renderer, TTF_Font* font);
void DrawBackdropThumbnail(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect area);
void DrawBackdropCircleButton(SDL_Renderer* renderer);
void DrawBackdropSubMenu(SDL_Renderer* renderer);
void Draw_Backdrop_List_Sidebar(SDL_Renderer* renderer, TTF_Font* font);
void UpdateMenuState();
void DrawBackdropLibrary(SDL_Renderer* renderer, TTF_Font* font);
void Draw_C_Blocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate&BT,vector<string>& values, SDL_Color color,TTF_Font*font,Blocks* block);
void Draw_sound_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Draw_report_informationOfCharacter_box(SDL_Renderer* renderer,InformationOfCharacter &information);
void Draw_X_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_Y_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_sprite_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_size_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_direction_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_show_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_show_and_hide_button(SDL_Renderer* renderer,Button &button1,Button &button2,SDL_Texture* texture1,SDL_Texture* texture2,Character &sprite);
void Handle_event_for_show_button(SDL_Event &e , Button &button,Character &sprite);
void Handle_event_for_hide_button(SDL_Event &e , Button &button,Character &sprite);
#endif //GRAPHIC_ELEMENT_H

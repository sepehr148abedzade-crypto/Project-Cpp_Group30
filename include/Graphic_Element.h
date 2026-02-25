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


extern bool isCharacterSubMenuOpen;
extern int btn_cx;
extern int btn_cy;
extern int radius;
extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
extern std::vector<BackdropItem> libraryItems;
extern bool isLibraryOpen;
extern bool isStageSelected;
extern int backdropScrollY;
extern SDL_Color white;

enum Tab { CODE, BACKDROPS, SOUNDS , COSTUMES ,CHARACTER};
extern Tab currentTab;
extern std::vector<Backdrop> projectBackdrops;
extern int selectedBackdropIndex;
extern int backdropScrollY;

void Update_Character_Menu_State();
void Draw_Character_Floating_Buttons(SDL_Renderer* renderer);

void ApplyFill(SDL_Texture* target,SDL_Renderer* renderer);
void ApplyTextToTexture(SDL_Texture* target, string text, int x, int y, SDL_Renderer* renderer, TTF_Font* font);
SDL_Texture* GetCurrentLayer();
SDL_Texture* ConvertToEditable(SDL_Texture* source, SDL_Renderer* renderer);
SDL_Texture* MakeTextureEditable(SDL_Renderer* renderer, SDL_Texture* sourceTex);

struct Button;

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
void DrawLineOnTexture(SDL_Texture* target, int x1, int y1, int x2, int y2, SDL_Renderer* renderer, bool isEraser);
void ApplyPen(SDL_Texture* target, int x, int y, SDL_Renderer* renderer);
void DrawCircleOnTexture(SDL_Texture* target, int centerX, int centerY, int radius, SDL_Renderer* renderer, bool fill);
void ClearCanvas(SDL_Texture* target, SDL_Renderer* renderer);
void ApplyFill(SDL_Texture* target, SDL_Renderer* renderer);
void Draw_Menu_Blocks(SDL_Renderer* renderer,TTF_Font* font);
void Draw_RunningBar(SDL_Renderer* renderer);
void Draw_Character_Show_Bar(SDL_Renderer* renderer);
void Draw_Information_of_Character(SDL_Renderer* renderer);
void Draw_Stage_Bar(SDL_Renderer* renderer, TTF_Font* font);
void Draw_Character(SDL_Renderer* renderer);
void Handle_event_for_code_button(SDL_Event &e);
void Handle_event_for_flag_button(SDL_Event &e,Button &button);
void Handle_event_for_stop_button(SDL_Event &e,Button &button);
void Handle_event_for_motion_sprite(SDL_Event &e,Character* sprite);
void Handle_event_for_timer_button(SDL_Event &e,Button* button);
void Handle_event_for_size_button(SDL_Event &e,Button* button);
void Handle_event_for_costume_number_button(SDL_Event &e,Button* button);
void Handle_event_for_next_costume_button(SDL_Event &e,SDL_Renderer* renderer,Button* button,Character* sprite);
void Draw_size_report(SDL_Renderer* renderer,TTF_Font* font,Character *sprite);
void Draw_time_report(SDL_Renderer* renderer,TTF_Font* font,Uint32 time);
void Draw_costume_report(SDL_Renderer* renderer,TTF_Font* font,Character* sprite);
void Draw_talking_box(SDL_Renderer* renderer,TTF_Font* font,Character *sprite);
void Draw_thinking_box(SDL_Renderer* renderer,TTF_Font* font,Character *sprite);
void DrawBackdropPanel(SDL_Renderer* renderer, TTF_Font* font);
void DrawBackdropThumbnail(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect area);
void Draw_Image_Editor(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* currentTex, string itemName);
void DrawBackdropCircleButton(SDL_Renderer* renderer);
void DrawBackdropSubMenu(SDL_Renderer* renderer);
void Draw_Backdrop_List_Sidebar(SDL_Renderer* renderer, TTF_Font* font);
void UpdateMenuState();
void DrawBackdropLibrary(SDL_Renderer* renderer, TTF_Font* font);
void Draw_C_Blocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate&BT,vector<InputValue>& inputs, SDL_Color color,TTF_Font*font,Blocks* block);
void Draw_sound_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Draw_Backdrop_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Draw_Back_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Draw_Code_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Draw_report_informationOfCharacter_box(SDL_Renderer* renderer,InformationOfCharacter &information);
void Draw_X_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_Y_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_sprite_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_size_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_direction_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_show_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_volume_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_frequency_text(SDL_Renderer* renderer,SDL_Texture* texture);
void Draw_show_and_hide_button(SDL_Renderer* renderer,Button &button1,Button &button2,SDL_Texture* texture1,SDL_Texture* texture2,Character* sprite);
void Draw_sounds_functions_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Handle_event_for_show_button(SDL_Event &e , Button &button,Character* sprite);
void Handle_event_for_hide_button(SDL_Event &e , Button &button,Character* sprite);
void Draw_name_of_character(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture);
void Handle_event_for_sound_button(SDL_Event &e,Button* button);
void Handle_event_for_Backdrop_button(SDL_Event &e,Button* button);
void Handle_event_for_Back_button(SDL_Event &e,Button* button);
void Handle_event_for_Code_button(SDL_Event &e,Button* button);
void Draw_positionX(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture);
void Draw_positionY(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture);
void Draw_size(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture);
void Draw_direction(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture);
void Draw_report_button(SDL_Renderer* renderer,Button* button,SDL_Texture* texture);
void Draw_Character_panel(SDL_Renderer* renderer);
void Draw_sound_panel(SDL_Renderer* renderer);
void Draw_sprite_button(SDL_Renderer* renderer,Button* button,SDL_Texture* picture,SDL_Texture* text);
void Handle_event_for_choose_sprite_in_Character_panel(SDL_Event &e,Button* button,sprite_button* sprite);
void Draw_sprite_button_under_stage(SDL_Renderer* renderer,sprite_button* button ,SDL_Texture* text);
void Handle_event_for_choose_now_sprite(SDL_Event &e,sprite_button* button);
void Handle_event_for_go_to_front_layer_button(SDL_Event &e,SDL_Renderer* renderer,Button* button,Character* sprite);
void Handle_event_for_drag_button(SDL_Event &e,SDL_Renderer* renderer,Button* button,Character* sprite);
void DrawExpressionBlock(SDL_Renderer* renderer, int x, int y, int w, int h,BlockTemplate& BT, vector<InputValue>& inputs,SDL_Color color, TTF_Font* font, Blocks* block);
void DrawBoolBlock(SDL_Renderer* renderer, int x, int y, int w, int h,
                   BlockTemplate& BT, vector<InputValue>& inputs,SDL_Color color, TTF_Font* font, Blocks* block);
void Draw_Character_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture);
void Handle_event_for_Character_button(SDL_Event &e,Button* button);
string InputValueToString(InputValue& val);
double GetInputAsNumber(InputValue& val, vector<vector<Blocks>>& chains);
string GetInputAsString(InputValue& val, vector<vector<Blocks>>& chains);
void CheckInputClick(int mx, int my);
int calculatingWidthBlock(BlockTemplate& BT, vector<InputValue>& inputs, TTF_Font* font, int parentChainIndex , int parentBlockIndex );
void DrawSimpleBlocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate& BT, vector<InputValue>& inputs, SDL_Color color,TTF_Font*font, Blocks* block );
vector<string> GetDropdownOptions(DropdownType type);
void DrawBlockOutput(SDL_Renderer* renderer, TTF_Font* font, Blocks& block);
#endif //GRAPHIC_ELEMENT_H

#pragma once
#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <string>
#include "Entity.h"

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

extern bool stop ;
extern bool isBackdropMenuOpen;

SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color);
bool Loading();
bool Init_Game();
void AddBlock(std::string type, int x, int y);
void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font);
void HandleBlockEvent(SDL_Event& e);
void SetRandomBackdrop();
bool IsCircleClicked(int mx, int my, int cx, int cy, int radius);
void AddBackdropToProject(SDL_Texture *tex, std::string name, bool forceSwitch, bool b);
void CheckInputClick(int mx, int my);
void CreateNewPaintBackdrop();
void UploadBackdrop();
bool IsValidChar(char c, InputType type);
void HandleKeyboardInput(SDL_Event& e);
void Handle_Scroll_Events(int mx, int my, const SDL_Event& e);
void Handle_Tab_Switch(int mx, int my);
void Handle_Backdrop_Selection(int mx, int my);
void Handle_Backdrop_Menu_Clicks(int mx, int my);
void Get_event();
void Draw_Stage_Content(SDL_Renderer* renderer);
void Update();
void Render();
void Clean();
void SDL_wait_key();

#endif //GAME_H

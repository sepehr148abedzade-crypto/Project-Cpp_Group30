#pragma once
#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <string>
#include "Entity.h"

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

extern bool stop ;
SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color);
bool Loading();
bool Init_Game();
void AddBlock(std::string type, int x, int y);
void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font);
void HandleBlockEvent(SDL_Event& e);
void CheckInputClick(int mx, int my);
bool IsValidChar(char c, InputType type);
void HandleKeyboardInput(SDL_Event& e);
void Get_event();
void Update();
void Render();
void Clean();
void SDL_wait_key();

#endif //GAME_H

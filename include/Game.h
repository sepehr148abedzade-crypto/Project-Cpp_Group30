#pragma once
#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <string>

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

extern bool stop ;
SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color);
bool Loading();
bool Init_Game();
void Get_event();
void Update();
void Render();
void Clean();
void SDL_wait_key();

#endif //GAME_H

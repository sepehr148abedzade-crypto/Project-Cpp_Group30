#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "SDL2/SDL.h"
#include "string"

struct Character;
struct Costume;

SDL_Texture* LoadTexture(SDL_Renderer* renderer,const char* path);
void SetWindowIcon(SDL_Window* window,const char* path);
void Load_Character(SDL_Renderer* renderer,Character* sprite,Costume* costume);
//SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color);

#endif //TEXTURE_MANAGER_H

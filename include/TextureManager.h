#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "SDL2/SDL.h"


SDL_Texture* LoadTexture(SDL_Renderer* renderer,const char* path);
void SetWindowIcon(SDL_Window* window,const char* path);
//SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color);

#endif //TEXTURE_MANAGER_H

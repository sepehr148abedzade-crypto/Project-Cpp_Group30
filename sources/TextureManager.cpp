#pragma once
#include "TextureManager.h"
#include "Entity.h"
#include "iostream"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"



//struct _TTF_Font;
//typedef struct _TTF_Font TTF_Font;

SDL_Texture* LoadTexture(SDL_Renderer* renderer,const char* path){
    SDL_Texture* texture = IMG_LoadTexture(renderer,path);
    if(texture== nullptr){
        std::cout << "Picture could not be load! path = "<< path <<" SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    return texture;
}

void SetWindowIcon(SDL_Window* window,const char* path){
    SDL_Surface* icon_surface= IMG_Load(path);
    if(icon_surface == nullptr){
        std::cout << "Window_Icon could not be load! SDL_Error: " << IMG_GetError() << std::endl;
    }
    SDL_SetWindowIcon(window,icon_surface);
    SDL_FreeSurface(icon_surface);
}

void Load_Character(SDL_Renderer* renderer,std::string name,Character &sprite,const char* path){
    SDL_Surface* surface= IMG_Load(path);
    if(surface == nullptr){
        std::cout << "texture of sprite could not be load! SDL_Error: " << IMG_GetError() << std::endl;
    }
    sprite.texture = SDL_CreateTextureFromSurface(renderer,surface);
    int width,height;
    SDL_QueryTexture(sprite.texture, nullptr, nullptr, &width, &height);
    sprite.width = (double)(width * sprite.size);
    sprite.height = (double)(height * sprite.size);
    sprite.path = path;
    sprite.name = name;
}

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfx.h>

using namespace std;

#ifndef A_M_F_FIRST_PAGE_H
#define A_M_F_FIRST_PAGE_H

struct Button{
    SDL_Rect rect;
    SDL_Color first_color;
    SDL_Color second_color;

    bool is_mouse_on;
};

const int width = 1300, height = 720;

//tabeh baraye navar abi
void blue_bar(SDL_Renderer* renderer, SDL_Texture* logo, Button file_button)
{
    SDL_Rect bar = {0, 0, 1300, 50};
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 150);
    SDL_RenderFillRect(renderer, &bar);

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("fonts/Montserrat-Bold.ttf", 15);
    SDL_Color white = {255, 255, 255, 255};


    if (logo != nullptr)
    {
        SDL_Rect logo_rect = {10, 10, 60, 30};
        SDL_RenderCopy(renderer, logo, nullptr, &logo_rect);
    }

    if (file_button.is_mouse_on)
    {
        SDL_SetRenderDrawColor(renderer, file_button.second_color.r, file_button.second_color.g,
                               file_button.second_color.b, 255);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, file_button.first_color.r, file_button.first_color.g,
                               file_button.first_color.b, 255);
    }
    SDL_RenderFillRect(renderer, &file_button.rect);

    // neveshtan matn "file" dar vasat dokmeh
    SDL_Surface* textSurf = TTF_RenderText_Blended(font, "File", white);
    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
    int tw, th;
    SDL_QueryTexture(textTex, nullptr, nullptr, &tw, &th);
    SDL_Rect textPos = {
            file_button.rect.x + (file_button.rect.w - tw) / 2,
            file_button.rect.y + (file_button.rect.h - th) / 2,
            tw, th};
    SDL_RenderCopy(renderer, textTex, nullptr, &textPos);

    SDL_FreeSurface(textSurf);
    SDL_DestroyTexture(textTex);
}

void codebar(SDL_Renderer* renderer, SDL_Texture* image)
{
    SDL_Rect left_bar = {0, 120, 90, 700};
    SDL_RenderCopy(renderer, image, nullptr, &left_bar);
}


#endif //A_M_F_FIRST_PAGE_H

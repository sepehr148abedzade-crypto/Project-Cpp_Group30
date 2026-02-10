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

struct Code_button{
    int x=30, y, r=15;
    SDL_Color color;
    string label;
};

struct Character{
    string name;
    int x, y;
    int size;
    int direc;
    bool visible;
};


const int width = 1300, height = 800;

// tabeh baraye neveshtan matn
void draw_text(SDL_Renderer* renderer, TTF_Font* font, string text, int x, int y) {
    SDL_Color color = {0, 000, 000, 255};
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    SDL_Rect dst = { x, y - (h/2), w, h };
    SDL_RenderCopy(renderer, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void sprite_panel(SDL_Renderer* renderer, TTF_Font* font, Character s) {

    SDL_Rect panelRect = {790, 470, 400, 80};
    roundedBoxRGBA(renderer, panelRect.x, panelRect.y,
                   panelRect.x + panelRect.w, panelRect.y + panelRect.h,
                   10, 255, 255, 255, 255); // سفید

    draw_text(renderer, font, "Sprite", panelRect.x + 20, panelRect.y + 30);
    roundedBoxRGBA(renderer, panelRect.x + 80, panelRect.y + 20,
                   panelRect.x + 200, panelRect.y + 50, 5, 240, 240, 240, 255);
    draw_text(renderer, font, s.name, panelRect.x + 90, panelRect.y + 35);

    draw_text(renderer, font, "x", panelRect.x + 220, panelRect.y + 30);
    draw_text(renderer, font, to_string(s.x), panelRect.x + 240, panelRect.y + 30);

    draw_text(renderer, font, "y", panelRect.x + 300, panelRect.y + 30);
    draw_text(renderer, font, to_string(s.y), panelRect.x + 320, panelRect.y + 30);
}

//tabeh baraye navar abi
void blue_bar(SDL_Renderer* renderer, SDL_Texture* logo, Button file_button, TTF_Font* font)
{
    SDL_Rect bar = {0, 0, 1300, 50};
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderFillRect(renderer, &bar);



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
    if (font != nullptr) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* textSurf = TTF_RenderText_Blended(font, "File", white);
        if (textSurf) {
            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
            int tw, th;
            SDL_QueryTexture(textTex, nullptr, nullptr, &tw, &th);
            SDL_Rect textPos = {
                    file_button.rect.x + (file_button.rect.w - tw) / 2,
                    file_button.rect.y + (file_button.rect.h - th) / 2,
                    tw, th
            };
            SDL_RenderCopy(renderer, textTex, nullptr, &textPos);

    SDL_FreeSurface(textSurf);
    SDL_DestroyTexture(textTex);
}
    }
        }

void codebar(SDL_Renderer* renderer, SDL_Texture* image, Code_button* codeButton, TTF_Font* font)
{
    SDL_Rect left_bar = {0, 120, 60, 700};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &left_bar);
    SDL_Color colors[] = {
            {76, 151, 255},
            {153, 102, 255},
            {214, 91, 229},
            {255, 191, 0},
            {255, 171, 25},
            {76, 191, 230},
            {89, 192, 89},
            {255, 140, 26},
    };
    string label[] = {"Motion", "Looks", "Sound", "Events", "Control", "Sensing", "Operators", "Variables"};

    int x = 30, y = 150, r = 15;
    for (int i = 0; i < 8; ++i)
    {

        codeButton[i].y = y;
        codeButton[i].color = {colors[i].r, colors[i].g, colors[i].b, 255};
        filledCircleRGBA(renderer, x, y, r, colors[i].r, colors[i].g,colors[i].b, 255);
        aacircleRGBA(renderer, codeButton[i].x, codeButton[i].y, codeButton[i].r, 0, 0, 0, 50);
        int dx;
        if (i>=0 && i<=3) dx = 15;
        else if (i>=4 && i<=5) dx = 17;
        else dx = 25;
        draw_text(renderer, font, label[i], x - dx, y+25);
        y+= 60;
    }

}



#endif //A_M_F_FIRST_PAGE_H

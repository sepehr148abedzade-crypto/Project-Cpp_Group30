#include "Graphic_Element.h"
#include "Entity.h"
#include <SDL2/SDL2_gfx.h>

SDL_Color white = {255,255,255};
SDL_Color Hex_To_rgb(uint32_t hexcolor){
    SDL_Color color;
    color.r = (hexcolor >> 16) & 0xFF;
    color.g = (hexcolor >> 8) & 0xFF;
    color.b = hexcolor & 0xFF;
    color.a = SDL_ALPHA_OPAQUE;
    return color;
}

bool Is_mouse_on(int x,int y,int w,int h) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h) {
        return true;
    }
    else {
        return false;
    }
}
void Draw_loading_window(SDL_Renderer* renderer,Button button,SDL_Texture* texture){
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            button.rect.x + (button.rect.w - texture_w) / 2,
            button.rect.y + (button.rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}

void Draw_BlueBar_Top(SDL_Renderer* renderer,int width,SDL_Texture* logo){
    SDL_Rect bar = {0,0,width,48};
    SDL_SetRenderDrawColor(renderer,77,151,255,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&bar);
    if (logo != nullptr){
        SDL_Rect logo_rect = {10, 10, (int)1072/14, (int)400/14};
        SDL_RenderCopy(renderer, logo, nullptr, &logo_rect);
    }
}

void Draw_Top_Button(SDL_Renderer* renderer,Button button,SDL_Texture* texture){
    if (Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button.first_color.r, button.first_color.g,
                               button.first_color.b, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &button.rect);
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            button.rect.x + (button.rect.w - texture_w) / 2,
            button.rect.y + (button.rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}

void Draw_CodeBar_Item(SDL_Renderer* renderer, Button code_button[]) {
    SDL_Rect left_bar = {0, 95, 60, Get_height() - 48};
    SDL_SetRenderDrawColor(renderer, 249, 249, 249, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &left_bar);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&left_bar);
    for (int i = 0; i < 8; i++) {
        filledCircleRGBA(renderer, code_button[i].rect.x + (code_button[i].rect.w/2), code_button[i].rect.y + (code_button[i].rect.h/2) - 10, code_button[i].radius,
                         code_button[i].first_color.r, code_button[i].first_color.g, code_button[i].first_color.b, SDL_ALPHA_OPAQUE);

        aacircleRGBA(renderer, code_button[i].rect.x + (code_button[i].rect.w/2), code_button[i].rect.y + (code_button[i].rect.h/2)-10, code_button[i].radius,
                     code_button[i].first_color.r, code_button[i].first_color.g, code_button[i].first_color.b, SDL_ALPHA_OPAQUE);

        aacircleRGBA(renderer, code_button[i].rect.x + (code_button[i].rect.w/2), code_button[i].rect.y + (code_button[i].rect.h/2)-10, code_button[i].radius,
                     0, 0, 0, 150);
        if (Is_mouse_on(code_button[i].rect.x,code_button[i].rect.y,code_button[i].rect.w,code_button[i].rect.h))
            SDL_SetTextureColorMod(code_button[i].Label_texture, 151, 214, 255);
        else
            SDL_SetTextureColorMod(code_button[i].Label_texture, 100, 100, 100);

        if (code_button[i].is_mouse_click_on) {
            SDL_SetRenderDrawColor(renderer, code_button[i].second_color.r, code_button[i].second_color.g,
                                   code_button[i].second_color.b, 50);
            SDL_RenderFillRect(renderer, &code_button[i].rect);
        }
        int dx;
        if (i <= 3) dx = 15;
        else if (i <= 5) dx = 17;
        else dx = 25;

        int texture_w, texture_h;
        SDL_QueryTexture(code_button[i].Label_texture, nullptr, nullptr, &texture_w, &texture_h);
        SDL_Rect textPosition = {
                code_button[i].rect.x + (code_button[i].rect.w - texture_w) / 2,
                code_button[i].rect.y + (code_button[i].rect.h - texture_h) / 2 + code_button[i].radius,
                texture_w, texture_h
        };
        SDL_RenderCopy(renderer, code_button[i].Label_texture, nullptr, &textPosition);
    }
}

void Draw_CodeBar(SDL_Renderer* renderer){
    SDL_Rect main_bar = {60,95,970,Get_height()};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&main_bar);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&main_bar);
}


#include "Graphic_Element.h"
#include "Entity.h"

//SDL_Color white = {255,255,255};
SDL_Color Hex_To_rgb(uint32_t hexcolor){
    SDL_Color color;
    color.r = (hexcolor >> 16) & 0xFF;
    color.g = (hexcolor >> 8) & 0xFF;
    color.b = hexcolor & 0xFF;
    color.a = SDL_ALPHA_OPAQUE;
    return color;
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

void Draw_Button(SDL_Renderer* renderer,Button button,SDL_Texture* texture){
    // barresi inkeh age mouse roye file bashe rangesh avaz besheh
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseX >= file_button.rect.x && mouseX<= file_button.rect.x + file_button.rect.w
        && mouseY >= file_button.rect.y && mouseY<= file_button.rect.y + file_button.rect.h)
    {
        file_button.is_mouse_on = true;
    }
    else {
        file_button.is_mouse_on = false;
    }

    if (button.is_mouse_on) {
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


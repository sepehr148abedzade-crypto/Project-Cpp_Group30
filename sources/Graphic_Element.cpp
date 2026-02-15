#include "Graphic_Element.h"
#include "Entity.h"
#include <SDL2/SDL2_gfx.h>

//SDL_Color white = {255,255,255};
SDL_Color Hex_To_rgb(uint32_t hexcolor){
    SDL_Color color;
    color.r = (hexcolor >> 16) & 0xFF;
    color.g = (hexcolor >> 8) & 0xFF;
    color.b = hexcolor & 0xFF;
    color.a = SDL_ALPHA_OPAQUE;
    return color;
}

void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &BlockBar);

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderDrawLine(renderer, 60, 100, 60, 900);
    SDL_RenderDrawLine(renderer, 310, 100, 310, 900);


    for (auto &block : active_blocks) {
        SDL_RenderCopy(renderer, block.image, NULL, &block.rect);

        if (blockMap.count(block.id)) {
            for (size_t i = 0; i < block.values.size(); i++) {

                if (i >= blockMap[block.id].inputs.size()) break;

                int posX = blockMap[block.id].inputs[i].posx;
                string textToShow = block.values[i];

                if (block.is_editing && block.active_value_index == i) {
                    textToShow += "|";
                }

                SDL_Texture* txtTex = LoadText(renderer, font, textToShow, {0, 0, 0, 255});
                if (txtTex) {
                    int tw, th;
                    SDL_QueryTexture(txtTex, NULL, NULL, &tw, &th);

                    SDL_Rect txtRect = {
                            block.rect.x + posX - (tw / 2),
                            block.rect.y + 22 - (th / 2),
                            tw, th
                    };
                    SDL_RenderCopy(renderer, txtTex, NULL, &txtRect);
                    SDL_DestroyTexture(txtTex);
                }
            }
        }
    }
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

void Draw_CodeBar(SDL_Renderer* renderer, Code_button* categories) {
    SDL_Rect left_bar = {0, 100, 60, Get_height() - 48};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &left_bar);

    for (int i = 0; i < 8; i++) {
        filledCircleRGBA(renderer, categories[i].x, categories[i].y, categories[i].r,
                         categories[i].color.r, categories[i].color.g, categories[i].color.b, 255);

        aacircleRGBA(renderer, categories[i].x, categories[i].y, categories[i].r, 0, 0, 0, 50);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= categories[i].x - 15 && mouseX <= categories[i].x + categories[i].r && mouseY >= categories[i].y-30 &&
        mouseY <= categories[i].y + 30)
            categories[i].is_mouse_on = true;
        else
            categories[i].is_mouse_on = false;

        if (categories[i].is_mouse_on)
            SDL_SetTextureColorMod(categories[i].labelTexture, 151, 214, 255);
        else
            SDL_SetTextureColorMod(categories[i].labelTexture, 0, 0, 0);

        int dx;
        if (i <= 3) dx = 15;
        else if (i <= 5) dx = 17;
        else dx = 25;

        int tw, th;
        SDL_QueryTexture(categories[i].labelTexture, NULL, NULL, &tw, &th);
        SDL_Rect textPos = { categories[i].x - dx, categories[i].y + 15, tw, th };
        SDL_RenderCopy(renderer, categories[i].labelTexture, NULL, &textPos);
    }
}


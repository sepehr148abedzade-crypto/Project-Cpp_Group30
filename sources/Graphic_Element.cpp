#include "Graphic_Element.h"
#include "Entity.h"
#include <SDL2/SDL2_gfx.h>
#include <vector>

Blocks* draggedBlock = nullptr;
bool isBackdropMenuOpen = false;
bool isLibraryOpen = false;
int offsetX = 0, offsetY = 0;
int sidebar_scroll_y = 0;
std::vector<BackdropItem> libraryItems;

SDL_Texture* icon_gallery = nullptr;
SDL_Texture* icon_paint = nullptr;
SDL_Texture* icon_upload = nullptr;
SDL_Texture* icon_surprise = nullptr;



SDL_Color white = {255,255,255};
SDL_Color Hex_To_rgb(uint32_t hexcolor){
    SDL_Color color;
    color.r = (hexcolor >> 16) & 0xFF;
    color.g = (hexcolor >> 8) & 0xFF;
    color.b = hexcolor & 0xFF;
    color.a = SDL_ALPHA_OPAQUE;
    return color;
}


void Drawtext(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color, bool center) {
    if (!font || text.empty()) return;
    SDL_Texture* tex = LoadText(renderer, font, text, color);
    if (tex) {
        int tw, th;
        SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
        SDL_Rect pos = { x, y, tw, th };
        if (center) {
            pos.x = x - (tw / 2);
        }
        SDL_RenderCopy(renderer, tex, NULL, &pos);
        SDL_DestroyTexture(tex);
    }
}


void DrawBlockInputs(SDL_Renderer* renderer, TTF_Font* font, Blocks& block) {
    if (blockMap.count(block.id)) {
        for (size_t i = 0; i < block.values.size(); i++) {
            int px = blockMap[block.id].inputs[i].posX;
            string text = block.values[i];
            if (block.is_editing && block.active_value_index == (int)i) {
                text += "|";
            }
            SDL_Texture* t = LoadText(renderer, font, text, {0, 0, 0, 255});
            if (t) {
                int tw, th;
                SDL_QueryTexture(t, NULL, NULL, &tw, &th);
                SDL_Rect textRect = { block.rect.x + px - (tw / 2), block.rect.y + 18 - (th / 2), tw, th };
                SDL_RenderCopy(renderer, t, NULL, &textRect);
                SDL_DestroyTexture(t);
            }
        }
    }
}

void Draw_Menu_Blocks(SDL_Renderer* renderer) {
    for (auto& mb : menu_blocks) {
        SDL_Rect renderPos = mb.rect;
        renderPos.y += sidebar_scroll_y;

        if (renderPos.y > 90 && renderPos.y < Get_height()) {
            SDL_RenderCopy(renderer, mb.image, NULL, &renderPos);
        }
    }
}

void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font) {
    for (auto& b : active_blocks) {
        SDL_RenderCopy(renderer, b.image, NULL, &b.rect);
        DrawBlockInputs(renderer, font, b);
    }
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
            SDL_SetTextureColorMod(code_button[i].Label_texture, 80, 80, 80);

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
    SDL_Rect code_bar = {60,95,250,Get_height()};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&code_bar);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&code_bar);
}
void Draw_RunningBar(SDL_Renderer* renderer){
    SDL_Rect rect = {60,95,970,Get_height()};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);

}

void Draw_Character_Show_Bar(SDL_Renderer* renderer){
    SDL_Rect rect = {1040,95,Get_width()-1040-10,Get_height()/2-80};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);

}

void Draw_Information_of_Character(SDL_Renderer* renderer){
    SDL_Rect main_bar = {1040,Get_height()/2+30,Get_width()-1040-100,100};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&main_bar);
    SDL_Rect rect = {1040,Get_height()/2+30,Get_width()-1040-100,340};
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_RenderDrawLine(renderer,1040,Get_height()/2+30+100,1040+(Get_width()-1040-100),Get_height()/2+30+100);
}

void DrawBackdropCircleButton(SDL_Renderer* renderer){

    filledCircleRGBA(renderer, Get_width()-53, 760, 20, 77, 151, 255, 255);
    aacircleRGBA(renderer, Get_width()-53, 760, 20, 77, 151, 255, 255);
}
void UpdateMenuState() {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    int cx = Get_width() - 53;
    int cy = 760;
    int r = 20;
    int dx = mx - cx;
    int dy = my - cy;
    if ((dx * dx + dy * dy) <= (r * r)) {
        isBackdropMenuOpen = true;
    } else {
        if (!(mx >= cx - 25 && mx <= cx + 25 && my < cy && my > cy - 150)) {
            isBackdropMenuOpen = false;
        }
    }
}

void DrawBackdropSubMenu(SDL_Renderer* renderer) {
    if (!isBackdropMenuOpen) return;

    int cx = Get_width() - 53; // مرکز دایره اصلی
    int cy = 760;              // مختصات Y دایره اصلی
    int spacing = 50;          // فاصله بین آیکون‌ها
    int iconSize = 30;         // اندازه خود تصویر آیکون

    SDL_Texture* icons[] = {icon_gallery, icon_surprise, icon_paint, icon_upload};

    for (int i = 0; i < 4; i++) {
        int targetY = cy - (spacing * (i + 1));

        filledCircleRGBA(renderer, cx, targetY, 20, 77, 151, 255, 255);
        aacircleRGBA(renderer, cx, targetY, 20, 77, 151, 255, 255);

        SDL_Rect iconRect = { cx - iconSize/2, targetY - iconSize/2, iconSize, iconSize };

        if (icons[i] != nullptr) {
            SDL_RenderCopy(renderer, icons[i], NULL, &iconRect);
        }
    }
}

void DrawBackdropLibrary(SDL_Renderer* renderer, TTF_Font* font) {
    if (!isLibraryOpen) return;
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);
    SDL_Rect header = {0, 0, Get_width(), 60};
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderFillRect(renderer, &header);
    SDL_Color white = {255, 255, 255, 255};
    Drawtext(renderer, font, "Choose a Backdrop", Get_width()/2, 20, white, true);
    SDL_Rect backBtn = { 20, 10, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 66, 128, 217, 255);
    SDL_RenderFillRect(renderer, &backBtn);
    Drawtext(renderer, font, "Back", 70, 20, white, true);
    int xStart = 50, yStart = 100;
    int padding = 20;
    int imgW = 150, imgH = 120;
    for (int i = 0; i < libraryItems.size(); i++) {
        SDL_Rect itemBox = { xStart + (i % 5) * (imgW + padding),
                             yStart + (i / 5) * (imgH + padding),
                             imgW, imgH };
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &itemBox);
        SDL_RenderCopy(renderer, libraryItems[i].texture, NULL, &itemBox);
    }
}

void Draw_Stage_Bar(SDL_Renderer* renderer){
    SDL_Rect rect = {Get_width()-100+5,Get_height()/2+30,85,Get_height()/2-80};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    DrawBackdropCircleButton(renderer);
    UpdateMenuState();
    DrawBackdropSubMenu(renderer);

}
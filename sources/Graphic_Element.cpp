#include "Graphic_Element.h"
#include "Entity.h"
#include "iostream"
#include "constants.h"
#include <SDL2/SDL2_gfx.h>
#include "sensing_functions.h"
#include "looks_functions.h"
#include "event_system.h"
#include <vector>
#include <ctime>
#include <cstdlib>
EditorSettings globalEditor;
Blocks* draggedBlock = nullptr;

bool isBackdropMenuOpen = false;
bool isLibraryOpen = false;
bool isStageSelected = false;

int offsetX = 0, offsetY = 0;
int sidebar_scroll_y = 0;
int selectedBackdropIndex = 0;
int backdropScrollY = 0;

std::vector<BackdropItem> libraryItems;
std::vector<Backdrop> libraryBackdrops;
std::vector<Backdrop> projectBackdrops;


Tab currentTab = CODE;

SDL_Texture* icon_gallery = nullptr;
SDL_Texture* icon_paint = nullptr;
SDL_Texture* icon_upload = nullptr;
SDL_Texture* icon_surprise = nullptr;

SDL_Color white = {255,255,255};
SDL_Color black = {0,0,0};
SDL_Color Hex_To_rgb(uint32_t hexcolor){
    SDL_Color color;
    color.r = (hexcolor >> 16) & 0xFF;
    color.g = (hexcolor >> 8) & 0xFF;
    color.b = hexcolor & 0xFF;
    color.a = SDL_ALPHA_OPAQUE;
    return color;
}

int calculatingWidthBlock (BlockTemplate& BT,vector<string>&value,TTF_Font* font ) {
    int totalWidth =20;
    totalWidth += Get_text_width(font,BT.Back_label);
    for (size_t i = 0 ; i <BT.inputs.size();++i) {
        if (BT.inputs[i].type==INPUT_NUMBER) {
            totalWidth+=5;
            string v = i<value.size() ? value[i]:BT.inputs[i].defaultValue;
            totalWidth+= Get_text_width(font,BT.Back_label);
        }
        if (i < BT.labels.size()) {
            totalWidth += 5;
            totalWidth += Get_text_width(font, BT.labels[i]);
        }
    }
    return totalWidth;
}

int Draw_label(int current_x,SDL_Renderer* renderer,TTF_Font* font ,string text, int y,SDL_Color color ) {
    SDL_Texture* text_tex = LoadText(renderer, font,text,color);
    if (text_tex) {
        SDL_Rect T_Rect;
        SDL_QueryTexture(text_tex, nullptr, nullptr, &T_Rect.w, &T_Rect.h);
        T_Rect.x = current_x;
        T_Rect.y = y+T_Rect.h;
        SDL_RenderCopy(renderer, text_tex, nullptr, &T_Rect);
        SDL_DestroyTexture(text_tex);
        return current_x+T_Rect.w;
    }
    return 0;
}

SDL_Texture* ConvertToEditable(SDL_Texture* source, SDL_Renderer* renderer) {
    if (!source) return nullptr;
    int w, h;
    SDL_QueryTexture(source, NULL, NULL, &w, &h);

    SDL_Texture* target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!target) return source;

    SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(renderer, source, NULL, NULL);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, oldTarget);
    return target;
}

SDL_Texture* GetCurrentLayer() {
    if (currentTab == BACKDROPS) {
        if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
            return projectBackdrops[selectedBackdropIndex].texture;
        }
    }
    return nullptr;
}

void ClearCanvas(SDL_Texture* target, SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);
}

void Drawtext(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color, bool center) {
    if (!font || text.empty()) return;
    SDL_Texture* tex = LoadText(renderer, font, text, color);
    if (tex) {
        int tw, th;
        SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
        SDL_Rect pos = { x, y, tw, th };
        if (center) {
            pos.x = x - (tw / 2);
        }
        SDL_RenderCopy(renderer, tex, nullptr, &pos);
        SDL_DestroyTexture(tex);
    }
}

void Draw_Backdrop_List_Sidebar(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect sidebarRect = { 0, 95, 105, Get_height() - 150 };
    SDL_RenderSetClipRect(renderer, &sidebarRect);

    for (int i = 0; i < (int)projectBackdrops.size(); i++) {
        int yPos = 110 + (i * 110) - backdropScrollY;
        SDL_Rect itemBox = { 10, yPos, 85, 90 };
        if (i == selectedBackdropIndex) {
            SDL_SetRenderDrawColor(renderer, 74, 151, 255, 255);
            SDL_RenderFillRect(renderer, &itemBox);
        } else {
            SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
            SDL_RenderFillRect(renderer, &itemBox);
        }
        SDL_Rect thumbRect = { 15, yPos + 5, 75, 60 };
        SDL_RenderCopy(renderer, projectBackdrops[i].texture, NULL, &thumbRect);
        Drawtext(renderer, font, std::to_string(i + 1), 20, yPos + 70, {100, 100, 100, 255}, false);
    }
    SDL_RenderSetClipRect(renderer, nullptr);
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
                SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
                SDL_Rect textRect = { block.rect.x + px - (tw / 2), block.rect.y + 22 - (th / 2), tw, th };
                SDL_RenderCopy(renderer, t, nullptr, &textRect);
                SDL_DestroyTexture(t);
            }
        }
    }
}

void Draw_Menu_Blocks(SDL_Renderer* renderer,TTF_Font* font) {
    for (auto& mb : menu_blocks) {
        SDL_Rect renderPos = mb.rect;
        renderPos.y += sidebar_scroll_y;
        SDL_Color color =GetBlockColor(blockMap[mb.id].category);

        if (renderPos.y > 90 && renderPos.y < Get_height()) {
            switch (mb.type) {
                case Simple_Block:
                    DrawSimpleBlocks(renderer,renderPos.x,renderPos.y,renderPos.w,renderPos.h,blockMap[mb.id],mb.values,color,font,nullptr);
                    break;
                case C_Block: C_Block:Draw_C_Blocks(renderer,renderPos.x,renderPos.y,renderPos.w,renderPos.h,blockMap[mb.id],mb.values,color,font, nullptr);
            }
        }
    }
}
void DrawSimpleBlocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate&BT,vector<string>& values, SDL_Color color,TTF_Font*font, Blocks* block ) {
    roundedBoxRGBA(renderer,x,y+10,x+w,y+h-10,0,color.r,color.g,color.b,color.a);
    Imaginary_circle C1 {x+20,y-10,15 };
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
    for (int i= x;i<=x+w;++i) {
        for (int j=y;j<=y+10;++j) {
            if (((i-C1.x)*(i-C1.x))+((j-C1.y)*(j-C1.y))>=C1.R*C1.R) {
                SDL_RenderDrawPoint(renderer,i,j);
            }
        }
    }
    C1.y+=h-10;
    for (int i= x;i<=x+w;++i) {
        for (int j=y+h-10;j<=y+h;++j) {
            if (((i-C1.x)*(i-C1.x))+((j-C1.y)*(j-C1.y))<=C1.R*C1.R) {
                SDL_RenderDrawPoint(renderer,i,j);
            }
        }
    }
    int current_x = x+5;
    current_x=Draw_label(current_x,renderer,font,BT.Back_label,y,{255,255,255,255})+5;
    for (size_t i =0 ; i<values.size(); i++) {
        string val = (i<values.size()) ? values[i]:BT.inputs[i].defaultValue;
        if (block && block->is_editing && block->active_value_index == (int)i) {
            val += "|";
        }
        int input_width = max(40,Get_text_width(font,val)+10);
        roundedBoxRGBA(renderer,current_x,y+9,current_x+input_width-3,y+29,10,255,255,255,255);
        Draw_label(current_x+(input_width-Get_text_width(font,val))/2,renderer,font,val,y,{100,100,100,255});
        current_x+=input_width+5;
        if (i<BT.labels.size()) {
            current_x=Draw_label(current_x,renderer,font,BT.labels[i],y,{255,255,255,255})+5;
        }

    }

    }
void Draw_C_Blocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate&BT,vector<string>& values, SDL_Color color,TTF_Font*font,Blocks* block) {
    SDL_Rect head{x,y , w,35};
    roundedBoxRGBA(renderer ,head.x,head.y,head.x +head.w,head.y+head.h,5,color.r,color.g,color.b,color.a);
    SDL_Rect left_wall {x,y+30,15,h-35};
    SDL_RenderFillRect(renderer,&left_wall);
    SDL_Rect bottom = {x, y + h - 25, w, 25};
    SDL_RenderFillRect(renderer, &bottom);
    int current_x = x+5;
    current_x=Draw_label(current_x,renderer,font,BT.Back_label,y,{255,255,255,255})+5;
    for (size_t i =0 ; i<values.size(); i++) {
        string val = (i<values.size()) ? values[i]:BT.inputs[i].defaultValue;
        int input_width = max(40,Get_text_width(font,val)+10);
        if (block && block->is_editing && block->active_value_index == (int)i) {
            val += "|";
        }

        roundedBoxRGBA(renderer, current_x - 3, y + 8, current_x + input_width - 3, y + 29, 10, 255, 255, 255, 255);
        Draw_label(current_x + (input_width - Get_text_width(font, val))/2, renderer, font, val, y , {100,100,100,255});
        current_x += input_width + 5;

    }

}
SDL_Color GetBlockColor(Block_category cat) {
    switch (cat) {
        case CAT_MOTION : return {76, 151, 255,255}; break;
        case CAT_LOOK : return {153, 102, 255,255}; break;
        case CAT_SOUND: return {214, 91, 229, 255}; break;
        case CAT_EVENT: return  {255, 200, 0, 255}; break;
        case CAT_CONTROL:   return {255, 140, 0, 255};
        case CAT_SENSING:   return {76, 191, 230, 255};
        case CAT_OPERATORS: return {89, 192, 89, 255};
        case CAT_VARIABLES: return {255, 140, 26, 255};
        default:            return {76, 151, 255, 255};

    }
}

void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font) {
    for (auto& b : active_blocks) {
        SDL_Color color= GetBlockColor(blockMap[b.id].category);
        switch (b.type) {
            case Simple_Block :
                DrawSimpleBlocks(renderer,b.rect.x,b.rect.y,b.rect.w,b.rect.h,blockMap[b.id],b.values,color,font,&b);
                break;
                case C_Block:
                Draw_C_Blocks(renderer,b.rect.x, b.rect.y, b.rect.w, b.rect.h, blockMap[b.id], b.values, color, font, &b);
        }
        //DrawBlockInputs(renderer, font, b);
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

void Draw_Top_Button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture){
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

void Draw_flag_and_stop_button(SDL_Renderer* renderer,Button &button1,Button &button2,SDL_Texture* texture1,SDL_Texture* texture2){
    if (Is_mouse_on(button1.rect.x,button1.rect.y,button1.rect.w,button1.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button1.second_color.r, button1.second_color.g,
                               button1.second_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button1.first_color.r, button1.first_color.g,
                               button1.first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(flag_active) {
        SDL_SetRenderDrawColor(renderer, button1.third_color.r, button1.third_color.g,
                               button1.third_color.b, SDL_ALPHA_OPAQUE);
    }

    SDL_RenderFillRect(renderer, &button1.rect);
    int texture1_w, texture1_h;
    SDL_QueryTexture(texture1, nullptr, nullptr, &texture1_w, &texture1_h);
    double scale1 = std::min(
            (double)button1.rect.w / texture1_w,
            (double)button1.rect.h / texture1_h
            );
    int new1_w = texture1_w * scale1;
    int new1_h = texture1_h * scale1;
    SDL_Rect textPosition1 = {
            button1.rect.x + (button1.rect.w - new1_w) / 2,
            button1.rect.y + (button1.rect.h - new1_h) / 2,
            new1_w, new1_h
    };
    SDL_RenderCopy(renderer, texture1, nullptr, &textPosition1);

    if (Is_mouse_on(button2.rect.x,button2.rect.y,button2.rect.w,button2.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button2.second_color.r, button2.second_color.g,
                               button2.second_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button2.first_color.r, button2.first_color.g,
                               button2.first_color.b, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &button2.rect);
    int texture2_w, texture2_h;
    SDL_QueryTexture(texture2, nullptr, nullptr, &texture2_w, &texture2_h);
    double scale2 = std::min(
            (double)button2.rect.w / texture2_w,
            (double)button2.rect.h / texture2_h
    );
    int new2_w = texture2_w * scale2;
    int new2_h = texture2_h * scale2;
    SDL_Rect textPosition2 = {
            button2.rect.x + (button2.rect.w - new2_w) / 2,
            button2.rect.y + (button2.rect.h - new2_h) / 2,
            new2_w, new2_h
    };
    SDL_RenderCopy(renderer, texture2, nullptr, &textPosition2);
}
void Draw_CodeBar_Item(SDL_Renderer* renderer, Button code_button[]) {
    SDL_Rect left_bar = {0, 95, 60, Get_height() - 48};
    SDL_SetRenderDrawColor(renderer, 249, 249, 249, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &left_bar);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&left_bar);
    for (int i = 0; i < 9; i++) {
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
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&stage);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&stage);

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
    int sw = Get_width();
    int sh = Get_height();
    int cx = sw - 100 + 5 + (85 / 2);
    int cy = (sh / 2 + 30) + (sh / 2 - 80) - 55;

    filledCircleRGBA(renderer, cx, cy, 18, 77, 151, 255, 255);
    if (icon_gallery) {
        SDL_Rect iconRect = { cx - 10, cy - 10, 20, 20 };
        SDL_RenderCopy(renderer, icon_gallery, NULL, &iconRect);
    }
}

void DrawBackdropSubMenu(SDL_Renderer* renderer) {
    if (!isBackdropMenuOpen) return;

    int sw = Get_width();
    int sh = Get_height();
    int cx = sw - 100 + 5 + (85 / 2);
    int cy = (sh / 2 + 30) + (sh / 2 - 80) - 55;
    int spacing = 45;

    SDL_Texture* upperIcons[] = {icon_surprise, icon_paint, icon_upload};
    for (int i = 0; i < 3; i++) {
        int targetY = cy - (spacing * (i + 1));
        filledCircleRGBA(renderer, cx, targetY, 18, 77, 151, 255, 255);
        if (upperIcons[i] != nullptr) {
            SDL_Rect iconRect = { cx - 10, targetY - 10, 20, 20 };
            SDL_RenderCopy(renderer, upperIcons[i], NULL, &iconRect);
        }
    }
}

void UpdateMenuState() {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    int sw = Get_width();
    int sh = Get_height();
    SDL_Rect rect = {sw - 100 + 5, sh / 2 + 30, 85, sh / 2 - 80};

    int cx = rect.x + (rect.w / 2);
    int cy = rect.y + rect.h - 55;

    int dx = mx - cx;
    int dy = my - cy;
    bool isOnMainButton = (dx * dx + dy * dy) <= (20 * 20);

    bool isOnSubMenu = (mx >= cx - 30 && mx <= cx + 30 && my < cy && my > cy - 165);

    isBackdropMenuOpen = (isOnMainButton || isOnSubMenu);
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
        SDL_RenderCopy(renderer, libraryItems[i].texture, NULL, &itemBox);
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        if (mx >= itemBox.x && mx <= itemBox.x + itemBox.w &&
            my >= itemBox.y && my <= itemBox.y + itemBox.h) {
            SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
            SDL_RenderDrawRect(renderer, &itemBox);
            SDL_Rect thickBox = { itemBox.x - 1, itemBox.y - 1, itemBox.w + 2, itemBox.h + 2 };
            SDL_RenderDrawRect(renderer, &thickBox);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &itemBox);
        }
        Drawtext(renderer, font, libraryItems[i].name,itemBox.x + 73, itemBox.y + imgH + 10, black
                , true);
    }
}

void Draw_Stage_Bar(SDL_Renderer* renderer, TTF_Font* font) {
    int sw = Get_width();
    int sh = Get_height();
    SDL_Rect rect = {sw - 100 + 5, sh / 2 + 30, 85, sh / 2 - 80};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);

    if (isStageSelected) {
        SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
        for(int i=0; i<2; i++) {
            SDL_Rect outline = {rect.x - i, rect.y - i, rect.w + i*2, rect.h + i*2};
            SDL_RenderDrawRect(renderer, &outline);
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    Drawtext(renderer, font, "Stage", rect.x + 17, rect.y + 5, {150, 150, 150}, false);

    int cx = rect.x + (rect.w / 2);
    int cy = rect.y + rect.h - 55;

    SDL_Rect thumbRect = { cx - 35, rect.y + 35, 70, 50 };
    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &thumbRect);

    if (currentBackdropTexture) {
        SDL_RenderCopy(renderer, currentBackdropTexture, NULL, &thumbRect);
    }
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &thumbRect);

    Drawtext(renderer, font, "Backdrops", cx-1, thumbRect.y + thumbRect.h + 8, {130, 130, 130}, true);

    int spacing = 45;
    if (isBackdropMenuOpen) {
        SDL_Texture* icons[] = {icon_surprise, icon_paint, icon_upload};
        for (int i = 0; i < 3; i++) {
            int ty = cy - (spacing * (i + 1));
            filledCircleRGBA(renderer, cx, ty, 18, 77, 151, 255, 255);
            if (icons[i]) {
                SDL_Rect iconRect = { cx - 10, ty - 10, 20, 20 };
                SDL_RenderCopy(renderer, icons[i], NULL, &iconRect);
            }
        }
    }

    filledCircleRGBA(renderer, cx, cy, 18, 77, 151, 255, 255);
    if (icon_gallery) {
        SDL_Rect iconRect = { cx - 10, cy - 10, 20, 20 };
        SDL_RenderCopy(renderer, icon_gallery, NULL, &iconRect);
    }
}

void Draw_Character(SDL_Renderer* renderer,Character* sprite){
    double centerX = stage.x + ((double )stage.w/2);
    double centerY = stage.y + ((double)stage.h/2);

    SDL_Rect dest;
    dest.w = (int)(sprite->width);
    dest.h = (int)(sprite->height);
    dest.x = (int)(centerX + sprite->x - ((double)dest.w/2));
    dest.y = (int)(centerY + sprite->y - ((double)dest.h/2));

    if(sprite->isvisible)
        SDL_RenderCopyEx(renderer,sprite->costumes[sprite->currentCostumeIndex]->texture, nullptr,&dest,0, nullptr,SDL_FLIP_NONE);
}

void Handle_event_for_code_button(SDL_Event &e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            for (int i = 0; i < 9; i++) {
                if (Is_mouse_on(categories[i].rect.x, categories[i].rect.y, categories[i].rect.w,
                                categories[i].rect.h)) {
                    for (int j = 0; j < 9; j++) {
                        categories[j].is_mouse_click_on = false;
                    }
                    categories[i].is_mouse_click_on = true;
                }
            }
        }
    }
}

void Handle_event_for_flag_button(SDL_Event &e , Button &button){
    if(e.type== SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h))
                flag_active = true;
        }
    }
}

void Handle_event_for_stop_button(SDL_Event &e , Button &button){
    if(e.type== SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h))
                flag_active= false;
        }
    }
}
void Handle_event_for_motion_sprite(SDL_Event &e, Character* sprite) {
    if (draggable) {
        static int mouse_firstX, mouse_firstY;
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            mouse_firstX = e.button.x;
            mouse_firstY = e.button.y;
            double stage_centerX = stage.x + (stage.w / 2);
            double stage_centerY = stage.y + (stage.h / 2);
            if (Is_mouse_on((int) (sprite->x + stage_centerX - (sprite->width / 2)),
                            (int) (sprite->y + stage_centerY - (sprite->height / 2)),
                            (int) sprite->width, (int) sprite->height))
                sprite->is_mouse_on = true;
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (!Limit_CharacterY(*sprite) || !Limit_CharacterX(*sprite)) {
                    double stage_centerX = stage.x + (stage.w / 2);
                    double stage_centerY = stage.y + (stage.h / 2);
                    sprite->x = mouse_firstX - stage_centerX;
                    sprite->y = mouse_firstY - stage_centerY;
                }
                sprite->is_mouse_on = false;
            }
        }
        if (e.type == SDL_MOUSEMOTION && sprite->is_mouse_on) {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            double stage_centerX = stage.x + (stage.w / 2);
            double stage_centerY = stage.y + (stage.h / 2);
            sprite->x = mouseX - stage_centerX;
            sprite->y = mouseY - stage_centerY;
        }
    }
}

void Handle_event_for_sound_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            currentTab = SOUNDS;
    }
}
void Handle_event_for_Backdrop_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            currentTab = BACKDROPS;
    }
}
void Handle_event_for_Back_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            currentTab = CODE;
    }
}

void Handle_event_for_Code_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            currentTab = CODE;
    }
}

void Handle_event_for_timer_button(SDL_Event &e,Button* button) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (Is_mouse_on(button->rect.x, button->rect.y, button->rect.w, button->rect.h)) {
            is_timer = !is_timer;
            button->is_mouse_click_on = !button->is_mouse_click_on;
        }
    }
}
void Handle_event_for_size_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h)) {
            is_size_on = !is_size_on;
            button->is_mouse_click_on = !button->is_mouse_click_on;
        }
    }
}
void Handle_event_for_costume_number_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h)) {
            is_costume_number_on = !is_costume_number_on;
            button->is_mouse_click_on = !button->is_mouse_click_on;
        }
    }
}
void Handle_event_for_next_costume_button(SDL_Event &e,SDL_Renderer* renderer,Button* button,Character* sprite){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h)) {
            switch_costume_to(renderer,sprite);
            button->is_mouse_click_on = !button->is_mouse_click_on;
        }
    }
}
void Draw_size_report(SDL_Renderer* renderer,TTF_Font* font,Character* sprite){
    std::string message = sprite->name + " " + "size is : " + to_string((int)sprite->size*500);
    SDL_Texture* texture = LoadText(renderer,font,message,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+5,stage.y+5,texture_w+10,25};
    SDL_SetRenderDrawColor(renderer,229,240,255,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2 ,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_time_report(SDL_Renderer* renderer,TTF_Font* font,Uint32 time){
    std::string message = "Timer : " + to_string((int)time/1000);
    SDL_Texture* texture = LoadText(renderer,font,message,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+stage.w-70,stage.y+5,texture_w+10,30};
    SDL_SetRenderDrawColor(renderer,229,240,255,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2 ,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_costume_report(SDL_Renderer* renderer,TTF_Font* font,Character* sprite){
    std::string message = sprite->name + " " + "costume number is : " + to_string((int)sprite->currentCostumeIndex);
    SDL_Texture* texture = LoadText(renderer,font,message,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+5,stage.y+25+10,texture_w+10,25};
    SDL_SetRenderDrawColor(renderer,229,240,255,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2 ,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_talking_box(SDL_Renderer* renderer,TTF_Font* font,Character &sprite){
    SDL_Texture* texture = LoadText(renderer,font,sprite.monologue,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+stage.w/2-texture_w/2,stage.y+stage.h - 35,texture_w+20,30};
    SDL_SetRenderDrawColor(renderer,229,240,255,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2 ,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}

void Draw_thinking_box(SDL_Renderer* renderer,TTF_Font* font,Character &sprite){
    SDL_Texture* texture = LoadText(renderer,font,sprite.monologue,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+stage.w/2-texture_w/2,stage.y+stage.h - 35,texture_w+20,30};
    SDL_SetRenderDrawColor(renderer,229,240,255,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2 ,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}

void Draw_sound_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture) {
    if (Is_mouse_on(button.rect.x, button.rect.y, button.rect.w, button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    } else {
        SDL_SetRenderDrawColor(renderer, button.first_color.r, button.first_color.g,
                               button.first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(currentTab == SOUNDS) {
        SDL_SetRenderDrawColor(renderer, button.third_color.r, button.third_color.g,
                               button.third_color.b, SDL_ALPHA_OPAQUE);
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
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button.rect);
}

void Draw_Backdrop_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture) {
    if (Is_mouse_on(button.rect.x, button.rect.y, button.rect.w, button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    } else {
        SDL_SetRenderDrawColor(renderer, button.first_color.r, button.first_color.g,
                               button.first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(currentTab == BACKDROPS) {
        SDL_SetRenderDrawColor(renderer, button.third_color.r, button.third_color.g,
                               button.third_color.b, SDL_ALPHA_OPAQUE);
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
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button.rect);
}

void Draw_Back_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture) {
    if (Is_mouse_on(button.rect.x, button.rect.y, button.rect.w, button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    } else {
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
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button.rect);
}

void Draw_Code_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture) {
    if (Is_mouse_on(button.rect.x, button.rect.y, button.rect.w, button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    } else {
        SDL_SetRenderDrawColor(renderer, button.first_color.r, button.first_color.g,
                               button.first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(currentTab == CODE) {
        SDL_SetRenderDrawColor(renderer, button.third_color.r, button.third_color.g,
                               button.third_color.b, SDL_ALPHA_OPAQUE);
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
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button.rect);
}
void Draw_sounds_functions_button(SDL_Renderer* renderer,Button &button,SDL_Texture* texture) {
    if (Is_mouse_on(button.rect.x, button.rect.y, button.rect.w, button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    } else {
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
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button.rect);
}
void Draw_report_informationOfCharacter_box(SDL_Renderer* renderer,InformationOfCharacter &information){
    SDL_SetRenderDrawColor(renderer,150,150,150,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&information.rect);
}

void Draw_X_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={stage.x+170,stage.y+stage.h+30,40,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_Y_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={stage.x+270,stage.y+stage.h+30,40,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_sprite_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={stage.x+10,stage.y+stage.h+30,40,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_size_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={stage.x+110,stage.y+stage.h+75,70,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_direction_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={stage.x+235,stage.y+stage.h+75,70,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_show_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={stage.x+15,stage.y+stage.h+75,20,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_volume_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={200,Get_height() - 320,100,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_frequency_text(SDL_Renderer* renderer,SDL_Texture* texture){
    SDL_Rect rect ={500,Get_height() - 320,100,30};
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_show_and_hide_button(SDL_Renderer* renderer,Button &button1,Button &button2,SDL_Texture* texture1,SDL_Texture* texture2,Character* sprite){
    if(sprite->isvisible)
        SDL_SetRenderDrawColor(renderer, button1.second_color.r, button1.second_color.g,
                               button1.second_color.b, SDL_ALPHA_OPAQUE);
    else {
        SDL_SetRenderDrawColor(renderer, button1.first_color.r, button1.first_color.g,
                               button1.first_color.b, SDL_ALPHA_OPAQUE);
    }

    SDL_RenderFillRect(renderer, &button1.rect);
    int texture1_w, texture1_h;
    SDL_QueryTexture(texture1, nullptr, nullptr, &texture1_w, &texture1_h);
    double scale1 = std::min(
            (double)button1.rect.w / texture1_w,
            (double)button1.rect.h / texture1_h
    );
    int new1_w = texture1_w * scale1;
    int new1_h = texture1_h * scale1;
    SDL_Rect textPosition1 = {
            button1.rect.x + (button1.rect.w - new1_w) / 2,
            button1.rect.y + (button1.rect.h - new1_h) / 2,
            new1_w, new1_h
    };
    SDL_RenderCopy(renderer, texture1, nullptr, &textPosition1);

    if (!sprite->isvisible) {
        SDL_SetRenderDrawColor(renderer, button2.second_color.r, button2.second_color.g,
                               button2.second_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button2.first_color.r, button2.first_color.g,
                               button2.first_color.b, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &button2.rect);
    int texture2_w, texture2_h;
    SDL_QueryTexture(texture2, nullptr, nullptr, &texture2_w, &texture2_h);
    double scale2 = std::min(
            (double)button2.rect.w / texture2_w,
            (double)button2.rect.h / texture2_h
    );
    int new2_w = texture2_w * scale2;
    int new2_h = texture2_h * scale2;
    SDL_Rect textPosition2 = {
            button2.rect.x + (button2.rect.w - new2_w) / 2,
            button2.rect.y + (button2.rect.h - new2_h) / 2,
            new2_w, new2_h
    };
    SDL_RenderCopy(renderer, texture2, nullptr, &textPosition2);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button1.rect);
    SDL_RenderDrawRect(renderer,&button2.rect);
}

void Handle_event_for_show_button(SDL_Event &e , Button &button,Character* sprite){
    if(e.type== SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h))
                sprite->isvisible = true;
        }
    }
}

void Handle_event_for_hide_button(SDL_Event &e , Button &button,Character* sprite){
    if(e.type== SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h))
                sprite->isvisible= false;
        }
    }
}

void Draw_name_of_character(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture){
    SDL_Rect rect =information.rect;
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_positionX(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture){
    SDL_Rect rect =information.rect;
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_size(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture){
    SDL_Rect rect =information.rect;
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}
void Draw_direction(SDL_Renderer* renderer,InformationOfCharacter &information,SDL_Texture* texture){
    SDL_Rect rect =information.rect;
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            rect.x + (rect.w - texture_w) / 2,
            rect.y + (rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}

void Draw_sound_panel(SDL_Renderer* renderer){
    SDL_Rect rect = {0,95,1030,Get_height()};
    SDL_SetRenderDrawColor(renderer,white.r,white.g,white.b,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
}

void Draw_report_button(SDL_Renderer* renderer,Button* button,SDL_Texture* texture){
    if (Is_mouse_on(button->rect.x, button->rect.y, button->rect.w, button->rect.h)) {
        SDL_SetRenderDrawColor(renderer, button->second_color.r, button->second_color.g,
                               button->second_color.b, SDL_ALPHA_OPAQUE);
    } else if(!button->is_mouse_click_on) {
        SDL_SetRenderDrawColor(renderer, button->first_color.r, button->first_color.g,
                               button->first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(button->is_mouse_click_on){
        SDL_SetRenderDrawColor(renderer, button->third_color.r, button->third_color.g,
                               button->third_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button->first_color.r, button->first_color.g,
                               button->first_color.b, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &button->rect);
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    SDL_Rect textPosition = {
            button->rect.x + (button->rect.w - texture_w) / 2,
            button->rect.y + (button->rect.h - texture_h) / 2,
            texture_w, texture_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&button->rect);
}
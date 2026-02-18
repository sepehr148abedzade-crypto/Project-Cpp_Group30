#include "Graphic_Element.h"
#include "Entity.h"
#include <SDL2/SDL2_gfx.h>

Blocks* draggedBlock = nullptr;
int offsetX = 0, offsetY = 0;
int sidebar_scroll_y = 0;


SDL_Color white = {255,255,255};
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
                    DrawSimpleBlocks(renderer,renderPos.x,renderPos.y,renderPos.w,renderPos.h,blockMap[mb.id],mb.values,color,font);
                    break;
            }
        }
    }
}
void DrawSimpleBlocks(SDL_Renderer* renderer,int x , int y , int w , int h ,BlockTemplate&BT,vector<string>& values, SDL_Color color,TTF_Font*font ) {
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
        int input_width = max(40,Get_text_width(font,val)+10);
        roundedBoxRGBA(renderer,current_x,y+9,current_x+input_width-3,y+29,10,255,255,255,255);
        Draw_label(current_x+(input_width-Get_text_width(font,val))/2,renderer,font,val,y,{100,100,100,255});
        current_x+=input_width+5;
        if (i<BT.labels.size()) {
            current_x=Draw_label(current_x,renderer,font,BT.labels[i],y,{255,255,255,255})+5;
        }

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
                DrawSimpleBlocks(renderer,b.rect.x,b.rect.y,b.rect.w,b.rect.h,blockMap[b.id],b.values,color,font);
                break;
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

void Draw_Stage_Bar(SDL_Renderer* renderer){
    SDL_Rect rect = {Get_width()-100+5,Get_height()/2+30,85,Get_height()/2-80};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
}
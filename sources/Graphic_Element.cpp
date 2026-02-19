#include "Graphic_Element.h"
#include "Entity.h"
#include "constants.h"
#include <SDL2/SDL2_gfx.h>
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
std::vector<BackdropItem> libraryItems;
std::vector<Backdrop> libraryBackdrops;
std::vector<Backdrop> projectBackdrops;
int selectedBackdropIndex = 0;
Tab currentTab = CODE;

SDL_Texture* icon_gallery = nullptr;
SDL_Texture* icon_paint = nullptr;
SDL_Texture* icon_upload = nullptr;
SDL_Texture* icon_surprise = nullptr;
int backdropScrollY = 0;


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

void DrawLineOnTexture(SDL_Texture* target, int x1, int y1, int x2, int y2, SDL_Renderer* renderer, bool isEraser) {
    if (!target) return;

    SDL_SetRenderTarget(renderer, target);

    if (isEraser) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    } else {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
    }

    int radius = isEraser ? 10 : 3;

    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawLine(renderer, x1 + w, y1 + h, x2 + w, y2 + h);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

SDL_Texture* GetCurrentLayer() {
    if (currentTab == BACKDROPS) {
        if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
            return projectBackdrops[selectedBackdropIndex].texture;
        }
    }
    return nullptr;
}

void Draw_Image_Editor(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* currentTex, string itemName) {
    int barX = 110, barY = 95, barW = 920, barH = 110;
    SDL_Rect topBar = { barX, barY, barW, barH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &topBar);

    Drawtext(renderer, font, "Name:", barX + 20, barY + 20, {100, 100, 100, 255}, false);

    SDL_Rect nameBox = { barX + 80, barY + 15, 180, 35 };
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &nameBox);

    string displayName = itemName;
    if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
        displayName = projectBackdrops[selectedBackdropIndex].name;
    }
    Drawtext(renderer, font, displayName, nameBox.x + 10, nameBox.y + 5, {0, 0, 0, 255}, false);

    int toolX = 115, toolY = 220, btnS = 45;
    for (int i = 0; i < 8; i++) {
        SDL_Rect r = { toolX + (i % 2) * 55, toolY + (i / 2) * 55, btnS, btnS };
        bool isActive = false;

        if (i == 1 && globalEditor.activeTool == TOOL_PEN) isActive = true;
        else if (i == 3 && globalEditor.activeTool == TOOL_LINE) isActive = true;
        else if (i == 4 && globalEditor.activeTool == TOOL_CIRCLE) isActive = true;
        else if (i == 5 && globalEditor.activeTool == TOOL_TEXT) isActive = true;
        else if (i == 6 && globalEditor.activeTool == TOOL_FILL) isActive = true;
        else if (i == 7 && globalEditor.activeTool == TOOL_ERASER) isActive = true;

        if (isActive) {
            SDL_SetRenderDrawColor(renderer, 66, 133, 244, 40);
            SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawColor(renderer, 66, 133, 244, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        }
        SDL_RenderDrawRect(renderer, &r);

        if (i == 1) Drawtext(renderer, font, "P", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 3) Drawtext(renderer, font, "/", r.x + 18, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 4) Drawtext(renderer, font, "O", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        else if (i == 5) Drawtext(renderer, font, "T", r.x + 12, r.y + 2, {0, 0, 0, 255}, false);
        else if (i == 6) {
            Drawtext(renderer, font, "F", r.x + 15, r.y + 12, {0, 0, 0, 255}, false);
        }
        else if (i == 7) Drawtext(renderer, font, "E", r.x + 15, r.y + 12, {255, 0, 0, 255}, false);
    }

    SDL_Rect canvasBG = { 240, 220, 780, 520 };
    SDL_SetRenderDrawColor(renderer, 232, 237, 247, 255);
    SDL_RenderFillRect(renderer, &canvasBG);

    if (currentTex) {
        SDL_Rect imgPos = { 330, 280, 600, 380 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &imgPos);
        SDL_RenderCopy(renderer, currentTex,
nullptr, &imgPos);

        if (selectedBackdropIndex >= 0 && projectBackdrops[selectedBackdropIndex].drawingLayer) {
            SDL_SetTextureBlendMode(projectBackdrops[selectedBackdropIndex].drawingLayer, SDL_BLENDMODE_BLEND);
            SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].drawingLayer, NULL, &imgPos);
        }

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &imgPos);
    }
}

void ApplyPen(SDL_Texture* target, int x, int y, SDL_Renderer* renderer) {
    if (!target) return;
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
    SDL_Rect r = { x - globalEditor.brushSize / 2, y - globalEditor.brushSize / 2, globalEditor.brushSize, globalEditor.brushSize };
    SDL_RenderFillRect(renderer, &r);
    SDL_SetRenderTarget(renderer,
nullptr);
}

void ApplyEraser(SDL_Texture* target, int x, int y, SDL_Renderer* renderer) {
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect r = { x - 10, y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &r);

    SDL_SetRenderTarget(renderer, oldTarget);
}

void DrawCircleOnTexture(SDL_Texture* target, int centerX, int centerY, int radius, SDL_Renderer* renderer, bool fill) {
    if (!target || radius <= 0) return;
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);

    if (fill) {
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    } else {
        int x = radius - 1;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int err = dx - (radius << 1);
        while (x >= y) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
            if (err <= 0) { y++; err += dy; dy += 2; }
            if (err > 0) { x--; dx += 2; err += dx - (radius << 1); }
        }
    }
    SDL_SetRenderTarget(renderer, nullptr);
}

void ClearCanvas(SDL_Texture* target, SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);
}

void ApplyFill(SDL_Texture* target, SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
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

void Draw_flag_button(SDL_Renderer* renderer,Button button,SDL_Texture* texture){
    if (Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button.first_color.r, button.first_color.g,
                               button.first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(button.is_mouse_click_on){
        SDL_SetRenderDrawColor(renderer,button.third_color.r,button.third_color.g,
                               button.third_color.b,SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &button.rect);
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    double scale = std::min(
            (double)button.rect.w / texture_w,
            (double)button.rect.h / texture_h
            );
    int new_w = texture_w * scale;
    int new_h = texture_h * scale;
    SDL_Rect textPosition = {
            button.rect.x + (button.rect.w - new_w) / 2,
            button.rect.y + (button.rect.h - new_h) / 2,
            new_w, new_h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &textPosition);
}

void Draw_stop_button(SDL_Renderer* renderer,Button button,SDL_Texture* texture){
    if (Is_mouse_on(button.rect.x,button.rect.y,button.rect.w,button.rect.h)) {
        SDL_SetRenderDrawColor(renderer, button.second_color.r, button.second_color.g,
                               button.second_color.b, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, button.first_color.r, button.first_color.g,
                               button.first_color.b, SDL_ALPHA_OPAQUE);
    }
    if(button.is_mouse_click_on){
        flag_button.is_mouse_click_on = false;
    }
    SDL_RenderFillRect(renderer, &button.rect);
    int texture_w, texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_w, &texture_h);
    double scale = std::min(
            (double)button.rect.w / texture_w,
            (double)button.rect.h / texture_h
    );
    int new_w = texture_w * scale;
    int new_h = texture_h * scale;
    SDL_Rect textPosition = {
            button.rect.x + (button.rect.w - new_w) / 2,
            button.rect.y + (button.rect.h - new_h) / 2,
            new_w, new_h
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

    Drawtext(renderer, font, "Backdrops", cx, thumbRect.y + thumbRect.h + 8, {130, 130, 130}, true);
    Drawtext(renderer, font, "1", cx, thumbRect.y + thumbRect.h + 24, {130, 130, 130}, true);

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

void Draw_Stage_Bar(SDL_Renderer* renderer){
    SDL_Rect rect = {Get_width()-100+5,Get_height()/2+30,85,Get_height()/2-80};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,&rect);
    SDL_SetRenderDrawColor(renderer,200,200,200,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer,&rect);
}

void Draw_Character(SDL_Renderer* renderer,Character &sprite){
    double centerX = stage.x + ((double )stage.w/2);
    double centerY = stage.y + ((double)stage.h/2);

    SDL_Rect dest;
    dest.w = (int)(sprite.width);
    dest.h = (int)(sprite.height);
    dest.x = (int)(centerX + sprite.x - ((double)dest.w/2));
    dest.y = (int)(centerY + sprite.y - ((double)dest.h/2));

    if(sprite.isvisible)
        SDL_RenderCopyEx(renderer,sprite.texture, nullptr,&dest,-sprite.degree, nullptr,SDL_FLIP_NONE);
}

void Handle_event_for_code_button(SDL_Event &e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            for (int i = 0; i < 8; i++) {
                if (Is_mouse_on(categories[i].rect.x, categories[i].rect.y, categories[i].rect.w,
                                categories[i].rect.h)) {
                    for (int j = 0; j < 8; j++) {
                        categories[j].is_mouse_click_on = false;
                    }
                    categories[i].is_mouse_click_on = true;
                }
            }
        }
    }
}

void Handle_event_for_flag_button(SDL_Event &e){
    if(e.type== SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(flag_button.rect.x,flag_button.rect.h,flag_button.rect.w,flag_button.rect.h))
            flag_button.is_mouse_click_on = true;
        }
    }
}

void Handle_event_for_stop_button(SDL_Event &e){
    if(e.type== SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(stop_button.rect.x,stop_button.rect.h,stop_button.rect.w,stop_button.rect.h))
            stop_button.is_mouse_click_on = true;
        }
    }
}
void Handle_event_for_motion_sprite(SDL_Event &e, Character &sprite){
    static int mouse_firstX , mouse_firstY;
    if(e.type == SDL_MOUSEBUTTONDOWN){
        mouse_firstX = e.button.x;
        mouse_firstY = e.button.y;
        double stage_centerX = stage.x + (stage.w/2);
        double stage_centerY = stage.y + (stage.h/2);
        if(Is_mouse_on((int)(sprite.x+stage_centerX-(sprite.width/2)),(int)(sprite.y+stage_centerY-(sprite.height/2)),
                       (int)sprite.width,(int)sprite.height)) sprite.is_mouse_on = true;
    }
    if(e.type==SDL_MOUSEBUTTONUP){
        if(e.button.button == SDL_BUTTON_LEFT) {
            if(!Limit_CharacterY(sprite) || !Limit_CharacterX(sprite)){
                double stage_centerX = stage.x + (stage.w/2);
                double stage_centerY = stage.y + (stage.h/2);
                sprite.x = mouse_firstX - stage_centerX;
                sprite.y = mouse_firstY - stage_centerY;
            }
            sprite.is_mouse_on = false;
        }
    }
    if(e.type == SDL_MOUSEMOTION && sprite.is_mouse_on) {
        int mouseX = e.motion.x;
        int mouseY = e.motion.y;
        double stage_centerX = stage.x + (stage.w/2);
        double stage_centerY = stage.y + (stage.h/2);
        sprite.x = mouseX - stage_centerX;
        sprite.y = mouseY - stage_centerY;
    }
}

void Draw_size_report(SDL_Renderer* renderer,TTF_Font* font,Character &sprite){
    std::string message = sprite.name + " " + "size is : " + to_string(sprite.size*500);
    SDL_Texture* texture = LoadText(renderer,font,message,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+5,stage.y+5,texture_w+10,25};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
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

void Draw_costume_report(SDL_Renderer* renderer,TTF_Font* font,Character &sprite){
    std::string message = sprite.name + " " + "costume number is : " + to_string(sprite.costumes.size());
    SDL_Texture* texture = LoadText(renderer,font,message,{50,50,50});
    int texture_w,texture_h;
    SDL_QueryTexture(texture, nullptr, nullptr,&texture_w,&texture_h);
    SDL_Rect rect = {stage.x+5,stage.y+5+25+5,texture_w+10,25};
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
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
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
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
    SDL_SetRenderDrawColor(renderer,249,249,249,SDL_ALPHA_OPAQUE);
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
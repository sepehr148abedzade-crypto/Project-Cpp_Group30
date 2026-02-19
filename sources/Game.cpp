#include "Game.h"
#include "TextureManager.h"
#include "iostream"
#include "SDL2/SDL_ttf.h"
#include "Graphic_Element.h"
#include "motion_functions.h"
#include "looks_functions.h"
#include "constants.h"
#include "Entity.h"
#include <Asset_Loader.h>
#include <vector>
#include <map>
#include <cmath>
using namespace std;

SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* Scratch_logo = nullptr;
SDL_Texture* File_Text = nullptr;
SDL_Texture* cat_texture = nullptr;
TTF_Font* loading_font = nullptr;
TTF_Font* main_font = nullptr;
TTF_Font* report_font = nullptr;
TTF_Font* talking_font = nullptr;
TTF_Font* edit_font = nullptr;
TTF_Font* code_bar_font = nullptr;
SDL_Texture* currentBackdropTexture = nullptr;
Character* currentSprite = nullptr;
SDL_Texture* globalDrawingLayer = nullptr;

std::vector<Character> allCharacters;
int lastMouseX = -1;
int lastMouseY = -1;
int lineStartX = -1, lineStartY = -1;
bool isDrawingLine = false;
bool isDrawingCircle = false;
int circleStartX = -1, circleStartY = -1;
bool isTypingText = false;
string textToDraw = "";
int textClickX = -1, textClickY = -1;
bool isSaveModalOpen = false;
string saveInputText = "";

string textInput = "";
bool isTyping = false;
int textX = 0, textY = 0;

bool stop = false;
void UpdateBlockWidth(Blocks& block , TTF_Font* font) {
    if (blockMap.count(block.id)) {
        int new_width = calculatingWidthBlock(blockMap[block.id],block.values,font);
        block.rect.w=new_width;
    }
}


void AddBackdropToProject(SDL_Texture *tex, string name, bool forceSwitch, bool b) {
    if (!tex || !renderer) return;

    int w, h;
    if (SDL_QueryTexture(tex, NULL, NULL, &w, &h) != 0) return;

    Backdrop newBD;
    newBD.name = name;

    newBD.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!newBD.texture) return;

    SDL_SetTextureBlendMode(newBD.texture, SDL_BLENDMODE_BLEND);

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    if (SDL_SetRenderTarget(renderer, newBD.texture) == 0) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, NULL, NULL);
    }

    newBD.drawingLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (newBD.drawingLayer) {
        SDL_SetTextureBlendMode(newBD.drawingLayer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, newBD.drawingLayer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    SDL_SetRenderTarget(renderer, oldTarget);

    projectBackdrops.push_back(newBD);
    selectedBackdropIndex = (int)projectBackdrops.size() - 1;

    if (forceSwitch) {
        currentTab = BACKDROPS;
        currentBackdropTexture = projectBackdrops[selectedBackdropIndex].texture;
    }
}

std::string GetUniqueBackdropName(std::string baseName) {
    int counter = 1;
    std::string finalName = baseName;
    bool exists = true;

    while (exists) {
        exists = false;
        for (size_t i = 0; i < projectBackdrops.size(); i++) {
            if (projectBackdrops[i].name == finalName) {
                exists = true;
                break;
            }
        }
        if (exists) {
            finalName = baseName + std::to_string(++counter);
        }
    }
    return finalName;
}

void AddUploadedBackdrop(SDL_Texture* tex, std::string fileName) {
    size_t lastDot = fileName.find_last_of(".");
    std::string baseName = (lastDot == std::string::npos) ? fileName : fileName.substr(0, lastDot);

    Backdrop newBD;
    newBD.texture = tex;
    newBD.name = GetUniqueBackdropName(baseName);
    projectBackdrops.push_back(newBD);
}

void CreateNewPaintBackdrop() {
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 780, 520);
    if (tex) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, tex);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);

        AddBackdropToProject(tex, "backdrop" + std::to_string(projectBackdrops.size() + 1), true, false);
        currentTab = BACKDROPS;
    }
}

void SetRandomBackdrop() {
    if (libraryItems.empty()) return;
    int r = rand() % libraryItems.size();
    bool shouldSwitch = (currentTab == BACKDROPS);
    AddBackdropToProject(libraryItems[r].texture, libraryItems[r].name, shouldSwitch, false);
}

void UploadBackdrop() {
    string path = OpenFileDialog();
    if (!path.empty()) {
        SDL_Texture* newTex = IMG_LoadTexture(renderer, path.c_str());
        if (newTex) {
            Backdrop bd;
            bd.texture = newTex;

            size_t lastSlash = path.find_last_of("/\\");
            string fileName = (lastSlash == string::npos) ? path : path.substr(lastSlash + 1);
            size_t lastDot = fileName.find_last_of(".");
            bd.name = (lastDot == string::npos) ? fileName : fileName.substr(0, lastDot);

            int w, h;
            SDL_QueryTexture(newTex, NULL, NULL, &w, &h);

            bd.drawingLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
            SDL_SetTextureBlendMode(bd.drawingLayer, SDL_BLENDMODE_BLEND);

            SDL_SetRenderTarget(renderer, bd.drawingLayer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, NULL);

            projectBackdrops.push_back(bd);
            selectedBackdropIndex = projectBackdrops.size() - 1;
        }
    }
}

bool IsCircleClicked(int mx, int my, int cx, int cy, int r) {
    return ((mx - cx) * (mx - cx) + (my - cy) * (my - cy)) <= (r * r);
}

void CheckInputClick(int mx, int my) {
    SDL_Point mPos = {mx, my};
    bool foundFocus = false;
    for (int i = active_blocks.size() - 1; i >= 0; i--) {
        if (SDL_PointInRect(&mPos, &active_blocks[i].rect)) {
            for (size_t j = 0; j < active_blocks[i].values.size(); j++) {
                int px = blockMap[active_blocks[i].id].inputs[j].posX;
                int centerY = active_blocks[i].rect.h / 2;
                SDL_Rect inputArea = {
                        active_blocks[i].rect.x + px - 20,
                        active_blocks[i].rect.y + centerY - 10,
                        40, 20
                };
                if (SDL_PointInRect(&mPos, &inputArea)) {
                    for(auto& b : active_blocks) b.is_editing = false;
                    active_blocks[i].is_editing = true;
                    active_blocks[i].active_value_index = (int)j;
                    foundFocus = true;
                    break;
                }
            }
        }
        if (foundFocus) break;
    }
   if (!foundFocus) {
        for(auto& b : active_blocks) b.is_editing = false;
    }
}

bool IsValidChar(char c, InputType type) {
    if (type == INPUT_NUMBER) {
        return (c >= '0' && c <= '9') || c == '-';
    }
    if (type ==INPUT_BOOLEAN) {
        return (c >= 32 && c <= 126);
    }
    return false;
}

void ApplyTextToLayer() {
    if (textInput.empty() || selectedBackdropIndex < 0) {
        isTyping = false;
        return;
    }

    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    if (!target) return;

    int tw, th;
    SDL_QueryTexture(target, NULL, NULL, &tw, &th);

    int fx = (textX - 330) * tw / 600;
    int fy = (textY - 280) * th / 380;

    SDL_Surface* surf = TTF_RenderText_Blended(edit_font, textInput.c_str(), globalEditor.currentColor);
    if (!surf) return;

    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dest = { fx, fy, surf->w, surf->h };

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);
    SDL_RenderCopy(renderer, textTex, NULL, &dest);
    SDL_SetRenderTarget(renderer, oldTarget);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(textTex);

    isTyping = false;
    textInput = "";
    SDL_StopTextInput();
}

void HandleKeyboardInput(SDL_Event& e) {
    if (isTyping) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !textInput.empty()) {
                textInput.pop_back();
            } else if (e.key.keysym.sym == SDLK_RETURN) {
                ApplyTextToLayer();
            }
        } else if (e.type == SDL_TEXTINPUT) {
            textInput += e.text.text;
        }
        return;
    }

    for (auto& b : active_blocks) {
        if (b.is_editing && b.active_value_index != -1) {
            string& str = b.values[b.active_value_index];
            InputType currentType = blockMap[b.id].inputs[b.active_value_index].type;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !str.empty()) {
                    str.pop_back();
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    b.is_editing = false;
                }
            } else if (e.type == SDL_TEXTINPUT) {
                char c = e.text.text[0];
                if (IsValidChar(c, currentType)) {
                    if (str.length() < 20) str += c;
                }
            }
        }
    }
}

void HandleLibraryClick(int mx, int my) {
    int xStart = 50, yStart = 100, padding = 20, imgW = 150, imgH = 120;

    for (int i = 0; i < (int)libraryItems.size(); i++) {
        SDL_Rect box = { xStart + (i % 5) * (imgW + padding),
                         yStart + (i / 5) * (imgH + padding),
                         imgW, imgH };

        if (mx >= box.x && mx <= box.x + box.w && my >= box.y && my <= box.y + box.h) {
            AddBackdropToProject(libraryItems[i].texture, libraryItems[i].name, (currentTab == BACKDROPS), false);
            isLibraryOpen = false;
            return;
        }
    }
}

void HandleBlockEvent(SDL_Event& e){
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mPos = {mx, my};
    if (e.type == SDL_MOUSEWHEEL) {
        if (mx > 60 && mx < 310) {
            sidebar_scroll_y += e.wheel.y * 25;
            if (sidebar_scroll_y > 0) sidebar_scroll_y = 0;
            if (sidebar_scroll_y < -1000) sidebar_scroll_y = -1000;
        }
    }
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        bool clickedOnMenu = false;
        if (mx > 60 && mx < 310 && my > 95) {
            for (auto& mb : menu_blocks) {
                SDL_Rect actual_pos = mb.rect;
                actual_pos.y += sidebar_scroll_y;
                if (SDL_PointInRect(&mPos, &actual_pos)) {
                    Blocks new_block = mb;
                    new_block.rect.x= mx- new_block.rect.w/2;
                    new_block.rect.y= my- new_block.rect.h/2;
                    new_block.values.clear();
                    if (blockMap.count(mb.id)) {
                        for (auto& Input : blockMap[mb.id].inputs) {
                            new_block.values.push_back(Input.defaultValue);
                        }
                    }
                    active_blocks.push_back(new_block);
                    //UpdateBlockWidth(active_blocks.back(),code_bar_font);
                    draggedBlock = &active_blocks.back();
                    offsetX=mx - draggedBlock->rect.x;
                    offsetY=my - draggedBlock->rect.y;
                    clickedOnMenu = true;
                    break;
                }
            }
        }
        if (!clickedOnMenu) {
            bool clickedOnInput = false;
            for (auto& block : active_blocks) {
                if (blockMap.count(block.id)) {
                    for (size_t i = 0; i < block.values.size(); i++) {
                        int input_x=block.rect.x+blockMap[block.id].inputs[i].posX;
                        SDL_Rect input_rect ={input_x,block.rect.y+12,40,20};

                        if (SDL_PointInRect(&mPos, &input_rect)) {
                            for (auto& b :active_blocks) {
                                b.is_editing = false;
                            }
                            block.is_editing = true;
                            block.active_value_index=i;
                            clickedOnInput = true;
                            SDL_StartTextInput();
                            break;
                        }
                    }
                    if (clickedOnInput) break;
                }
            }
            if (!clickedOnInput) {
                for (int i = active_blocks.size()-1; i>=0; i--) {
                    if (SDL_PointInRect(&mPos, &active_blocks[i].rect)) {
                        draggedBlock = &active_blocks[i];
                        offsetX= mx-active_blocks[i].rect.x;
                        offsetY=my-active_blocks[i].rect.y;
                        Blocks temp = active_blocks[i];
                        active_blocks.erase(active_blocks.begin()+i);
                        active_blocks.push_back(temp);
                        draggedBlock = &active_blocks.back();
                        break;
                    }
                }
            }
        }
    }
    if (e.type == SDL_MOUSEMOTION && draggedBlock != nullptr) {
        draggedBlock->rect.x = mx - offsetX;
        draggedBlock->rect.y = my - offsetY;
    }
    if (e.type == SDL_MOUSEBUTTONUP) {
        if (draggedBlock != nullptr) {
            int safeZoneX_Start = 310;
            int safeZoneY_Start = 95;

            bool shouldDelete = false;
            if (draggedBlock->rect.x < safeZoneX_Start ||
                draggedBlock->rect.y < safeZoneY_Start ||
                draggedBlock->rect.x > 900) {
                shouldDelete = true;
            }
            if (shouldDelete) {
                for (auto it = active_blocks.begin(); it != active_blocks.end(); ++it) {
                    if (&(*it) == draggedBlock) {
                        draggedBlock = nullptr;
                        active_blocks.erase(it);
                        break;
                    }
                }
            }
             draggedBlock = nullptr;
        }
    }
}

SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color){
        if(!font) {
                std::cout << "font is not find! SDL_Error : " << TTF_GetError() << std::endl;
                return nullptr;
        }
        SDL_Surface* text_surface = TTF_RenderText_Blended(font,text.c_str(),color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,text_surface);
        SDL_FreeSurface(text_surface);
        return texture;
}
int Get_text_width(TTF_Font* font,string text ) {
    int width ;
    TTF_SizeText(font,text.c_str(),&width,nullptr);
    return width;
}

bool Loading(){
        if(TTF_Init()==-1){
                std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
                return false;
        }
        loading_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",50);
        if(loading_font== nullptr){
                std::cout << "Loading Font could not be found! Error: " << TTF_GetError() << std::endl;
                return false;
        }
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                std::cout << "SDL could not initialize! SDL_Error : " << SDL_GetError() << std::endl;
                return false;
        }
        main_window= SDL_CreateWindow(
                "Scratch",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                Get_width(),
                Get_width(),
                SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED |SDL_WINDOW_RESIZABLE
        );

        if(main_window == nullptr) {
                std::cout << "main_window could not be created! SDL_Error : " << SDL_GetError() << std::endl;
                return false;
        }

        renderer = SDL_CreateRenderer(main_window,-1,SDL_RENDERER_ACCELERATED);

        if(renderer == nullptr) {
                std::cout << "renderer could not be created! SDL_Error : " << SDL_GetError() << std::endl;
                return false;
        }
        SDL_Texture* Loading_text = LoadText(renderer,loading_font,"Scratch is loading...",white);

        SDL_SetRenderDrawColor(renderer,77,151,255,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        Init_Load_button();
        Draw_loading_window(renderer,Load_button,Loading_text);
        SDL_RenderPresent(renderer);
        //SDL_Delay(3000);
}

bool Init_Game(){
        SDL_SetRenderDrawColor(renderer, 229, 240, 255, 255);
        SDL_RenderClear(renderer);

        if(TTF_Init()==-1){
                std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
                return false;
        }
        main_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",15);
        edit_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",40);
        if(main_font== nullptr){
                std::cout << "Font could not be found! Error: " << TTF_GetError() << std::endl;
                return false;
        }
        Init_Button();
        report_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",10);
        if(report_font== nullptr){
            std::cout << "Report Font could not be found! Error: " << TTF_GetError() << std::endl;
            return false;
        }
        talking_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",20);
        if(talking_font== nullptr){
            std::cout << "Talking Font could not be found! Error: " << TTF_GetError() << std::endl;
            return false;
        }
        File_Text = LoadText(renderer,main_font,"File",white);
        Scratch_logo = LoadTexture(renderer,"asset/images/logo/scratch.png");
        SetWindowIcon(main_window,"asset/images/logo/icon.png");

        code_bar_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf", 10);
        if(code_bar_font == nullptr){
                std::cout << "Code_bar Font could not be found! Error: " << TTF_GetError() << std::endl;
                return false;
        }
        Init_code_button(renderer,code_bar_font);
        LoadAllAssets(renderer);
        Init_Menu_Blocks();
        LoadBackdropLibraryManual(renderer);
        SDL_StartTextInput();
        Load_Character(renderer,"cat",cat,"asset/images/sprite/cat.png");
        Load_Character(renderer,"cat_running",cat_running,"asset/images/sprite/cat_running.png");
        now_sprite = cat_running;
        return true;
}

void Handle_Scroll_Events(int mx, int my, const SDL_Event& e) {
    if (e.type == SDL_MOUSEWHEEL && !isLibraryOpen) {
        if (mx < 110 && currentTab == BACKDROPS) {
            if (e.wheel.y > 0) backdropScrollY -= 30;
            else if (e.wheel.y < 0) backdropScrollY += 30;

            if (backdropScrollY < 0) backdropScrollY = 0;
            int contentHeight = projectBackdrops.size() * 110;
            int visibleHeight = Get_height() - 200;
            if (backdropScrollY > contentHeight - visibleHeight && contentHeight > visibleHeight)
                backdropScrollY = contentHeight - visibleHeight;
        }
    }
}

void Handle_Tab_Switch(int mx, int my) {
    if (my >= 50 && my <= 90) {
        if (mx >= 100 && mx <= 200) currentTab = CODE;
        else if (mx >= 200 && mx <= 300) {
            currentTab = BACKDROPS;
            backdropScrollY = 0;
        }
    }
}

void Handle_Backdrop_Selection(int mx, int my) {
    if (currentTab == BACKDROPS && mx < 105 && my > 95) {
        for (int i = 0; i < (int)projectBackdrops.size(); i++) {
            int yPos = 110 + (i * 110) - backdropScrollY;
            if (my >= yPos && my <= yPos + 90) {
                selectedBackdropIndex = i;
                currentBackdropTexture = projectBackdrops[i].texture;
                break;
            }
        }
    }
}

void Handle_Backdrop_Menu_Clicks(int mx, int my) {
    int cx = Get_width() - 53;
    int cy = 760;

    if (isBackdropMenuOpen) {
         if (IsCircleClicked(mx, my, cx, cy - 150, 20)) {
            UploadBackdrop();
            isBackdropMenuOpen = false;
        }
         else if (IsCircleClicked(mx, my, cx, cy - 100, 20)) {
            CreateNewPaintBackdrop();
            isBackdropMenuOpen = false;
        }
         else if (IsCircleClicked(mx, my, cx, cy - 50, 20)) {
            SetRandomBackdrop();
            isBackdropMenuOpen = false;
        }
         else if (IsCircleClicked(mx, my, cx, cy, 20)) {
            isLibraryOpen = true;
            isBackdropMenuOpen = false;
        }
    }
}

void MapMouseToCanvas(int mx, int my, int* outX, int* outY, SDL_Texture* target) {
    int tw, th;
    SDL_QueryTexture(target, NULL, NULL, &tw, &th);
    *outX = (mx - 330) * tw / 600;
    *outY = (my - 280) * th / 380;
}

void HandleToolSelection(int mx, int my) {
    int toolX = 115, toolY = 220, btnS = 45;
    for (int i = 0; i < 9; i++) {
        SDL_Rect r = { toolX + (i % 2) * 55, toolY + (i / 2) * 55, btnS, btnS };
        if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {

            if (i == 8) { // دکمه بازگشت
                currentTab = CODE;
            }
            else if (i == 7) { // دکمه ذخیره
                isSaveModalOpen = true;
                saveInputText = projectBackdrops[selectedBackdropIndex].name;
                SDL_StartTextInput();
            }
            else {
                if (isTyping && i != 4) ApplyTextToLayer();

                if (i == 0) ClearCurrentDrawingLayer(renderer);
                else if (i == 1) globalEditor.activeTool = TOOL_PEN;
                else if (i == 2) globalEditor.activeTool = TOOL_LINE;
                else if (i == 3) globalEditor.activeTool = TOOL_CIRCLE;
                else if (i == 4) globalEditor.activeTool = TOOL_TEXT;
                else if (i == 5) globalEditor.activeTool = TOOL_FILL;
                else if (i == 6) globalEditor.activeTool = TOOL_ERASER;
            }
            break;
        }
    }
}

void HandleCanvasMouseDown(int mx, int my) {
    if (currentTab != BACKDROPS || selectedBackdropIndex < 0) return;
    if (mx < 330 || mx > 930 || my < 280 || my > 660) return;

    auto tool = globalEditor.activeTool;
    if (tool == TOOL_TEXT) {
        if (isTyping) ApplyTextToLayer();
        isTyping = true;
        textX = mx; textY = my;
        textInput = "";
        SDL_StartTextInput();
    } else if (tool == TOOL_FILL) {
        ApplyFill(projectBackdrops[selectedBackdropIndex].drawingLayer, renderer);
    } else if (tool == TOOL_CIRCLE) {
        circleStartX = mx; circleStartY = my;
        isDrawingCircle = true;
    } else if (tool == TOOL_LINE) {
        lineStartX = mx; lineStartY = my;
        isDrawingLine = true;
    }
}



void HandleColorSelection(int mx, int my) {
    int colorX = 110 + 300;
    int colorY = 95 + 32;
    SDL_Color colors[] = {{0, 0, 0, 255}, {255, 0, 0, 255}, {0, 0, 255, 255}};

    for (int i = 0; i < 3; i++) {
        int targetX = colorX + i * 45;
        int targetY = colorY;
        if (pow(mx - targetX, 2) + pow(my - targetY, 2) <= pow(15, 2)) {
            globalEditor.currentColor = colors[i];
            break;
        }
    }
}

void HandleCanvasMouseUp(int mx, int my) {
    if (selectedBackdropIndex < 0) return;
    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    if (!target) return;

    int x1, y1, x2, y2;
    if (isDrawingCircle && globalEditor.activeTool == TOOL_CIRCLE) {
        MapMouseToCanvas(circleStartX, circleStartY, &x1, &y1, target);
        MapMouseToCanvas(mx, my, &x2, &y2, target);
        int radius = (int)sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        DrawCircleOnTexture(target, x1, y1, radius, renderer, true);
    } else if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) {
        MapMouseToCanvas(lineStartX, lineStartY, &x1, &y1, target);
        MapMouseToCanvas(mx, my, &x2, &y2, target);
        DrawLineOnTexture(target, x1, y1, x2, y2, renderer, false);
    }
    isDrawingCircle = isDrawingLine = false;
    lastMouseX = lastMouseY = -1;
}

void HandleContinuousDrawing(int mx, int my) {
    if (currentTab != BACKDROPS || selectedBackdropIndex < 0 || isDrawingLine || isDrawingCircle || isTyping) return;

    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    int lx = mx - 330, ly = my - 280;
    if (lx < 0 || lx > 600 || ly < 0 || ly > 380) {
        lastMouseX = lastMouseY = -1;
        return;
    }

    int fx, fy;
    MapMouseToCanvas(mx, my, &fx, &fy, target);
    bool isEraser = (globalEditor.activeTool == TOOL_ERASER);
    if (globalEditor.activeTool == TOOL_PEN || isEraser) {
        if (lastMouseX != -1) DrawLineOnTexture(target, lastMouseX, lastMouseY, fx, fy, renderer, isEraser);
        else isEraser ? ApplyEraser(target, fx, fy, renderer) : ApplyPen(target, fx, fy, renderer);
        lastMouseX = fx; lastMouseY = fy;
    }
}

void HandleBrushSizeSelection(int mx, int my) {
    int sizeX = 110 + 450;
    int sizeY = 95 + 32;
    int sizes[] = {2, 5, 10};

    for (int i = 0; i < 3; i++) {
        int targetX = sizeX + i * 40;
        int targetY = sizeY;
        if ((mx - targetX) * (mx - targetX) + (my - targetY) * (my - targetY) <= 400) {
            globalEditor.brushSize = sizes[i];
            break;
        }
    }
}

void Get_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) stop = true;
        Handle_event_for_code_button(e);
        Handle_event_for_motion_sprite(e, now_sprite);

        int mx, my;
        Uint32 mouseState = SDL_GetMouseState(&mx, &my);
        bool isLeftPressed = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (isLibraryOpen) {
                if (mx >= 20 && mx <= 120 && my >= 10 && my <= 50) isLibraryOpen = false;
                else HandleLibraryClick(mx, my);
            } else {
                Handle_Tab_Switch(mx, my);
                Handle_Backdrop_Selection(mx, my);
                Handle_Backdrop_Menu_Clicks(mx, my);
                HandleToolSelection(mx, my);
                HandleColorSelection(mx, my);
                HandleBrushSizeSelection(mx, my);
                HandleCanvasMouseDown(mx, my);
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            HandleCanvasMouseUp(mx, my);
        }

        if (isLeftPressed) {
            HandleContinuousDrawing(mx, my);
        }

        HandleBlockEvent(e);
        HandleKeyboardInput(e);
    }
}

void Draw_Stage_Content(SDL_Renderer* renderer) {
    int sw = Get_width();
    int stageW = 486;
    int stageH = 352;
    int stageX = sw - stageW - 10;
    int stageY = 95;
    SDL_Rect stageArea = { stageX, stageY, stageW, stageH };

    if (!projectBackdrops.empty() && selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
        SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].texture, NULL, &stageArea);
        SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].drawingLayer, NULL, &stageArea);
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &stageArea);

    for (auto& ch : allCharacters) {
        if (ch.isvisible) { //!ch.costumes.empty()
            int centerX = stageArea.x + (stageArea.w / 2);
            int centerY = stageArea.y + (stageArea.h / 2);
            int rSize = (ch.size > 0) ? ch.size : 100;
            int  Xpos = centerX + ch.x - (rSize / 2);
            int YPos = centerY + ch.y - (rSize / 2);
            SDL_Rect charPos =
                {
                    Xpos,
                    YPos,
                    rSize,
                    rSize
            };
           // SDL_RenderCopyEx(renderer, ch.costumes[ch.currentCostumeIndex], NULL, &charPos, (double)ch.degree, NULL, SDL_FLIP_NONE);
        }
    }
}

void DrawSaveModal(SDL_Renderer* renderer, TTF_Font* font) {
    if (!isSaveModalOpen) return;

    SDL_Rect overlay = { 0, 0, Get_width(), Get_height() };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, &overlay);

    int mW = 400, mH = 200;
    SDL_Rect modal = { (Get_width() - mW) / 2, (Get_height() - mH) / 2, mW, mH };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &modal);
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderDrawRect(renderer, &modal);

    Drawtext(renderer, font, "Save to Library", modal.x + 20, modal.y + 20, {77, 151, 255, 255}, false);
    Drawtext(renderer, font, "Enter Name:", modal.x + 20, modal.y + 60, {100, 100, 100, 255}, false);

    SDL_Rect inputField = { modal.x + 20, modal.y + 85, 360, 40 };
    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &inputField);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &inputField);

    if (!saveInputText.empty()) {
        Drawtext(renderer, font, saveInputText, inputField.x + 10, inputField.y + 10, {0, 0, 0, 255}, false);
    }

    SDL_Rect saveBtn = { modal.x + 280, modal.y + 140, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderFillRect(renderer, &saveBtn);
    Drawtext(renderer, font, "SAVE", saveBtn.x + 25, saveBtn.y + 10, {255, 255, 255, 255}, false);

    SDL_Rect cancelBtn = { modal.x + 170, modal.y + 140, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &cancelBtn);
    Drawtext(renderer, font, "Cancel", cancelBtn.x + 20, cancelBtn.y + 10, {255, 255, 255, 255}, false);
}

void Update() {
    UpdateMenuState();
    SDL_SetRenderDrawColor(renderer, 229, 240, 255, 255);
    SDL_RenderClear(renderer);

    if (isLibraryOpen) {
        DrawBackdropLibrary(renderer, main_font);
    } else {
        Draw_BlueBar_Top(renderer, Get_width(), Scratch_logo);
        Draw_Top_Button(renderer, Top_button, File_Text);

        if (currentTab == CODE) {
            Draw_RunningBar(renderer);
            Draw_CodeBar(renderer);
            Draw_CodeBar_Item(renderer, categories);
            Draw_Menu_Blocks(renderer, code_bar_font);
            DrawALLBlocks(renderer, code_bar_font);
        }
        else if (currentTab == BACKDROPS || currentTab == COSTUMES) {
            Draw_Backdrop_List_Sidebar(renderer, main_font);

            SDL_Texture* baseTex = nullptr;
            string bName = "Backdrop";
            if (!projectBackdrops.empty() && selectedBackdropIndex >= 0) {
                baseTex = projectBackdrops[selectedBackdropIndex].texture;
                bName = projectBackdrops[selectedBackdropIndex].name;
            }

            Draw_Image_Editor(renderer, main_font, baseTex, bName);

            if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
                if (isDrawingCircle && globalEditor.activeTool == TOOL_CIRCLE) {
                    int curX, curY;
                    SDL_GetMouseState(&curX, &curY);
                    int radius = (int)sqrt(pow(curX - circleStartX, 2) + pow(curY - circleStartY, 2));
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
                    for (int a = 0; a < 360; a++) {
                        float r1 = a * M_PI / 180.0f;
                        float r2 = (a + 1) * M_PI / 180.0f;
                        SDL_RenderDrawLine(renderer, circleStartX + (int)(radius * cos(r1)), circleStartY + (int)(radius * sin(r1)), circleStartX + (int)(radius * cos(r2)), circleStartY + (int)(radius * sin(r2)));
                    }
                }
                else if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) {
                    int curX, curY; SDL_GetMouseState(&curX, &curY);
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
                    SDL_RenderDrawLine(renderer, lineStartX, lineStartY, curX, curY);
                }
            }
        }

        Draw_Information_of_Character(renderer);
        Draw_Character_Show_Bar(renderer);
        Draw_Stage_Bar(renderer, main_font);

        if (isBackdropMenuOpen) DrawBackdropSubMenu(renderer);

        Draw_Stage_Content(renderer);
        Draw_Character(renderer, now_sprite);
        Draw_size_report(renderer, main_font, now_sprite);

        if (isSaveModalOpen) {
            DrawSaveModal(renderer, main_font);
        }
    }
    SDL_RenderPresent(renderer);
}

void Render(){
        SDL_RenderPresent(renderer);
}

void Clean(){
        TTF_CloseFont(main_font);
        TTF_Quit();
        SDL_DestroyWindow(main_window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
}

void SDL_wait_key(){
        auto *e = new SDL_Event();
        while(e->type != SDL_KEYDOWN){
                SDL_PollEvent(e);
        }
}





#include "Game.h"
#include "TextureManager.h"
#include "iostream"
#include "SDL2/SDL_ttf.h"
#include "Graphic_Element.h"
#include "constants.h"
#include "Entity.h"
#include <Asset_Loader.h>
#include <vector>
#include <map>
using namespace std;



SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* Scratch_logo = nullptr;
SDL_Texture* File_Text = nullptr;
TTF_Font* loading_font = nullptr;
TTF_Font* main_font = nullptr;
TTF_Font* code_bar_font = nullptr;
SDL_Texture* currentBackdropTexture = nullptr;
character* currentSprite = nullptr;
SDL_Texture* globalDrawingLayer = nullptr;
std::vector<character> allCharacters;
int lastMouseX = -1;
int lastMouseY = -1;
int lineStartX = -1, lineStartY = -1;
bool isDrawingLine = false;

string textInput = "";
bool isTyping = false;
int textX = 0, textY = 0;

bool stop = false;


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
            bd.name = "Uploaded Backdrop";

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
    if (type == NUMBER) {
        return (c >= '0' && c <= '9') || c == '-';
    }
    if (type ==TEXT) {
        return (c >= 32 && c <= 126);
    }
    return false;
}

void HandleKeyboardInput(SDL_Event& e) {
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
            }
            else if (e.type == SDL_TEXTINPUT) {
                char c = e.text.text[0];
                if (IsValidChar(c, currentType)) {
                    size_t maxLen = (currentType == NUMBER) ? 6 : 20;
                    if (str.length() < maxLen) {
                        str += c;
                    }
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
                    AddBlock(mb.id, mx - blockMap[mb.id].width/2, my - blockMap[mb.id].height/2, mb.image);
                    draggedBlock = &active_blocks.back();
                    offsetX = mx - draggedBlock->rect.x;
                    offsetY = my - draggedBlock->rect.y;
                    clickedOnMenu = true;
                    break;
                }
            }
        }
        if (!clickedOnMenu) {
            for (int i = active_blocks.size() - 1; i >= 0; i--) {
                if (SDL_PointInRect(&mPos, &active_blocks[i].rect)) {
                    draggedBlock = &active_blocks[i];
                    offsetX = mx - active_blocks[i].rect.x;
                    offsetY = my - active_blocks[i].rect.y;
                    Blocks temp = active_blocks[i];
                    active_blocks.erase(active_blocks.begin() + i);
                    active_blocks.push_back(temp);
                    draggedBlock = &active_blocks.back();
                    break;
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
        if(main_font== nullptr){
                std::cout << "Font could not be found! Error: " << TTF_GetError() << std::endl;
                return false;
        }
        Init_Button();

        File_Text = LoadText(renderer,main_font,"File",white);
        Scratch_logo = LoadTexture(renderer,"asset/images/scratch.png");
        SetWindowIcon(main_window,"asset/images/icon.png");

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

void Get_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) stop = true;

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

                int toolX = 115, toolY = 220, btnS = 45;
                for (int i = 0; i < 8; i++) {
                    SDL_Rect r = { toolX + (i % 2) * 55, toolY + (i / 2) * 55, btnS, btnS };
                    if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
                        if (i == 1) globalEditor.activeTool = TOOL_PEN;
                        else if (i == 3) globalEditor.activeTool = TOOL_LINE;
                        else if (i == 7) globalEditor.activeTool = TOOL_ERASER;
                    }
                }

                if (currentTab == BACKDROPS && globalEditor.activeTool == TOOL_LINE && selectedBackdropIndex >= 0) {
                    if (mx >= 330 && mx <= 930 && my >= 280 && my <= 660) {
                        lineStartX = mx;
                        lineStartY = my;
                        isDrawingLine = true;
                    }
                }
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            if (isDrawingLine && globalEditor.activeTool == TOOL_LINE && selectedBackdropIndex >= 0) {
                SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
                if (target != nullptr) {
                    int tw, th;
                    SDL_QueryTexture(target, NULL, NULL, &tw, &th);
                    int x1 = (lineStartX - 330) * tw / 600;
                    int y1 = (lineStartY - 280) * th / 380;
                    int x2 = (mx - 330) * tw / 600;
                    int y2 = (my - 280) * th / 380;
                    DrawLineOnTexture(target, x1, y1, x2, y2, renderer, false);
                }
                isDrawingLine = false;
                lineStartX = -1; lineStartY = -1;
            }
            lastMouseX = -1; lastMouseY = -1;
        }

        if (isLeftPressed && !isLibraryOpen && currentTab == BACKDROPS && !isDrawingLine) {
            if (globalEditor.activeTool == TOOL_PEN || globalEditor.activeTool == TOOL_ERASER) {
                if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
                    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;

                    if (target != nullptr) {
                        int tw, th;
                        SDL_QueryTexture(target, NULL, NULL, &tw, &th);
                        int lx = mx - 330, ly = my - 280;
                        if (lx >= 0 && lx <= 600 && ly >= 0 && ly <= 380) {
                            int fx = lx * tw / 600;
                            int fy = ly * th / 380;
                            if (lastMouseX != -1 && lastMouseY != -1) {
                                DrawLineOnTexture(target, lastMouseX, lastMouseY, fx, fy, renderer, (globalEditor.activeTool == TOOL_ERASER));
                            } else {
                                if (globalEditor.activeTool == TOOL_PEN) ApplyPen(target, fx, fy, renderer);
                                else ApplyEraser(target, fx, fy, renderer);
                            }
                            lastMouseX = fx; lastMouseY = fy;
                        } else { lastMouseX = -1; lastMouseY = -1; }
                    } else {
                        lastMouseX = -1; lastMouseY = -1;
                    }
                }
            }
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
        if (ch.show && !ch.costumes.empty()) {
            int centerX = stageArea.x + (stageArea.w / 2);
            int centerY = stageArea.y + (stageArea.h / 2);
            int rSize = (ch.size > 0) ? ch.size : 100;
            SDL_Rect charPos = {
                    centerX + ch.x - (rSize / 2),
                    centerY - ch.y - (rSize / 2),
                    rSize,
                    rSize
            };
            SDL_RenderCopyEx(renderer, ch.costumes[ch.currentCostumeIndex], NULL, &charPos, (double)ch.degree, NULL, SDL_FLIP_NONE);
        }
    }
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
            Draw_Menu_Blocks(renderer);
            DrawALLBlocks(renderer, code_bar_font);
        } else if (currentTab == BACKDROPS || currentTab == COSTUMES) {
            Draw_Backdrop_List_Sidebar(renderer, main_font);

            SDL_Texture* baseTex = nullptr;
            string bName = "Backdrop";
            if (!projectBackdrops.empty() && selectedBackdropIndex >= 0) {
                baseTex = projectBackdrops[selectedBackdropIndex].texture;
                bName = projectBackdrops[selectedBackdropIndex].name;
            }

            Draw_Image_Editor(renderer, main_font, baseTex, bName);

            if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
                SDL_Rect imgPos = { 330, 280, 600, 380 };
                SDL_SetTextureBlendMode(projectBackdrops[selectedBackdropIndex].drawingLayer, SDL_BLENDMODE_BLEND);
                SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].drawingLayer, NULL, &imgPos);

                if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) {
                    int curX, curY;
                    SDL_GetMouseState(&curX, &curY);
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
                    SDL_RenderDrawLine(renderer, lineStartX, lineStartY, curX, curY);
                }
            }
        }

        Draw_Information_of_Character(renderer);
        Draw_Character_Show_Bar(renderer);
        Draw_Stage_Bar(renderer, main_font);
        DrawBackdropCircleButton(renderer);
        if (isBackdropMenuOpen) DrawBackdropSubMenu(renderer);
        Draw_Stage_Content(renderer);
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





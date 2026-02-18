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
std::vector<character> allCharacters;

bool stop = false;


void AddBackdropToProject(SDL_Texture *tex, string name, bool forceSwitch, bool b) {
    if (!tex) return;
    Backdrop newBD;
    newBD.texture = tex;
    newBD.name = name;
    projectBackdrops.push_back(newBD);
    selectedBackdropIndex = projectBackdrops.size() - 1;
    currentBackdropTexture = projectBackdrops[selectedBackdropIndex].texture;
    if (forceSwitch) {
        currentTab = BACKDROPS;
    }
}


void CreateNewPaintBackdrop() {
    SDL_Surface* surf = SDL_CreateRGBSurface(0, 480, 360, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (surf) {
        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 255, 255));
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (tex) {
            AddBackdropToProject(tex, "backdrop" + std::to_string(projectBackdrops.size() + 1), true, false);
            currentTab = BACKDROPS;
            printf("New Blank Backdrop Created and switched to Edit mode.\n");
        }
    }
}

void SetRandomBackdrop() {
    if (libraryItems.empty()) return;
    int r = rand() % libraryItems.size();
    bool shouldSwitch = (currentTab == BACKDROPS);
    AddBackdropToProject(libraryItems[r].texture, "Surprise", shouldSwitch, false);
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

void UploadBackdrop() {
    std::string filePath = OpenFileDialog();
    if (filePath.empty()) return;
    SDL_Texture* newTex = IMG_LoadTexture(renderer, filePath.c_str());

    if (newTex) {
        bool shouldSwitch = (currentTab == BACKDROPS);
        AddBackdropToProject(newTex, "Uploaded", shouldSwitch, false);
        printf("File Uploaded: %s\n", filePath.c_str());
    } else {
        printf("Upload Error: %s\n", IMG_GetError());
    }
}

void Get_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) stop = true;

        int mx, my;
        SDL_GetMouseState(&mx, &my);
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
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (isLibraryOpen) {
                if (mx >= 0 && mx <= 120 && my >= 0 && my <= 60) {
                    isLibraryOpen = false;
                } else {
                    HandleLibraryClick(mx, my);
                }
                continue;
            }
            if (my >= 50 && my <= 90) {
                if (mx >= 100 && mx <= 200) currentTab = CODE;
                else if (mx >= 200 && mx <= 300) {
                    currentTab = BACKDROPS;
                    backdropScrollY = 0;
                }
            }
            if (currentTab == BACKDROPS && mx < 105 && my > 95) {
                bool clickedAny = false;
                for (int i = 0; i < (int)projectBackdrops.size(); i++) {
                    int yPos = 110 + (i * 110) - backdropScrollY;
                    if (my >= yPos && my <= yPos + 90) {
                        selectedBackdropIndex = i;
                        currentBackdropTexture = projectBackdrops[i].texture;
                        clickedAny = true;
                        break;
                    }
                }
                if (clickedAny) continue;
            }

            int cx = Get_width() - 53;
            int cy = 760;

            if (isBackdropMenuOpen) {
                if (IsCircleClicked(mx, my, cx, cy, 20)) {
                    isLibraryOpen = true;
                    isBackdropMenuOpen = false;
                }
                else if (IsCircleClicked(mx, my, cx, cy - 50, 20)) {
                    SetRandomBackdrop();
                    isBackdropMenuOpen = false;
                }
                else if (IsCircleClicked(mx, my, cx, cy - 100, 20)) {
                    CreateNewPaintBackdrop();
                    isBackdropMenuOpen = false;
                }
                else if (IsCircleClicked(mx, my, cx, cy - 150, 20)) {
                    UploadBackdrop();
                    isBackdropMenuOpen = false;
                }
                else {
                    isBackdropMenuOpen = false;
                }
            } else {
                if (IsCircleClicked(mx, my, cx, cy, 30)) {
                    isBackdropMenuOpen = true;
                }
            }
        }

        HandleBlockEvent(e);
        HandleKeyboardInput(e);
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
            SDL_Texture* target = (!projectBackdrops.empty()) ? projectBackdrops[selectedBackdropIndex].texture : nullptr;
            Draw_Image_Editor(renderer, main_font, target, "Backdrop");
        }

        Draw_Information_of_Character(renderer);
        Draw_Character_Show_Bar(renderer);
        Draw_Stage_Bar(renderer, main_font);
        DrawBackdropCircleButton(renderer);
        if (isBackdropMenuOpen) DrawBackdropSubMenu(renderer);

        int sw = Get_width();
        int stageW = 486;
        int stageH = 352;
        int stageX = sw - stageW - 10;
        int stageY = 95;
        SDL_Rect stageArea = { stageX, stageY, stageW, stageH };

        if (!projectBackdrops.empty() && selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
            SDL_Texture* activeTex = projectBackdrops[selectedBackdropIndex].texture;
            if (activeTex) {
                SDL_RenderCopy(renderer, activeTex, NULL, &stageArea);
            }
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





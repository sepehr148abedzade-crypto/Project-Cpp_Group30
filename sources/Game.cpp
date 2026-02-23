#include "Game.h"
#include "iomanip"
#include "TextureManager.h"
#include "iostream"
#include "SDL2/SDL_ttf.h"
#include "Graphic_Element.h"
#include "motion_functions.h"
#include "sound_functions.h"
#include "sensing_functions.h"
#include "looks_functions.h"
#include "constants.h"
#include "Entity.h"
#include <Asset_Loader.h>
#include <vector>
#include <map>
#include <cmath>
#include "Paint_Editor.h"
#include "SDL2/SDL_mixer.h"
#include "Paint_Editor.h"
#include <algorithm>
using namespace std;

SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* Scratch_logo = nullptr;
SDL_Texture* File_Text = nullptr;
SDL_Texture* Sound_text = nullptr;
SDL_Texture* Backdrop_text = nullptr;
SDL_Texture* Back_text = nullptr;
SDL_Texture* code_text = nullptr;
SDL_Texture* green_flag = nullptr;
SDL_Texture* stop_sign = nullptr;
SDL_Texture* X_text = nullptr;
SDL_Texture* Y_text = nullptr;
SDL_Texture* size_text = nullptr;
SDL_Texture* sprite_text = nullptr;
SDL_Texture* direction_text = nullptr;
SDL_Texture* positionX_text = nullptr;
SDL_Texture* positionY_text = nullptr;
SDL_Texture* size_of_sprite_text = nullptr;
SDL_Texture* name_of_sprite_text = nullptr;
SDL_Texture* direction_of_sprite_text = nullptr;;
SDL_Texture* show_text = nullptr;
SDL_Texture* S_text = nullptr;
SDL_Texture* H_text = nullptr;
SDL_Texture* Run_text = nullptr;
SDL_Texture* volumeUp_text = nullptr;
SDL_Texture* volumeDown_text = nullptr;
SDL_Texture* increase_frequency_text = nullptr;
SDL_Texture* decrease_frequency_text = nullptr;
SDL_Texture* Timer_text = nullptr;
SDL_Texture* size_button_text = nullptr;
SDL_Texture* next_costume_text = nullptr;
SDL_Texture* costume_number_text = nullptr;
SDL_Texture* volume_text = nullptr;
SDL_Texture* volume_value = nullptr;
SDL_Texture* frequency_text = nullptr;
SDL_Texture* frequency_value = nullptr;
TTF_Font* loading_font = nullptr;
TTF_Font* main_font = nullptr;
TTF_Font* report_font = nullptr;
TTF_Font* talking_font = nullptr;
TTF_Font* thinking_font = nullptr;
TTF_Font* edit_font = nullptr;
TTF_Font* code_bar_font = nullptr;
SDL_Texture* currentBackdropTexture = nullptr;
Character* currentSprite = nullptr;
SDL_Texture* globalDrawingLayer = nullptr;

std::vector<Character> allCharacters;
vector<vector<Blocks>> blockChains;
 int draggedChainIndex= -1;
int executingChainIndex = -1;
int executingBlockIndex = -1;
Uint32 executionStartTime = 0;
bool isExecuting = false;

Uint32 clickStartTime = 0;
bool isClickAndHold = false;
const Uint32 CLICK_THRESHOLD = 200;

int lastMouseX = -1;
int lastMouseY = -1;
int lineStartX = -1, lineStartY = -1;
int textClickX = -1, textClickY = -1;
int textX = 0, textY = 0;
int circleStartX = -1, circleStartY = -1;

bool isTypingText = false;
bool isDrawingLine = false;
bool isDrawingCircle = false;
bool isSaveModalOpen = false;
bool isTyping = false;
bool stop = false;

string saveInputText;
string textToDraw;
string textInput;

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
void Executing_Motion_Blocks(Blocks& block,Character* sprite ) {
    string ID = block.id;
    if (ID=="move") {
        double steps=stod(block.values[0]);
        move_steps_character(steps,sprite);
    }
    else if (ID=="tern_left") {
        double angel = stod(block.values[0]);
        turn_clockwise_character(angel,sprite);
    }
    else if (ID=="tern_right") {
        double angel = stod(block.values[0]);
        turn_clockwise_character(angel,sprite);
    }
    else if (ID=="change_x") {
        double new_x = stod(block.values[0]);
        change_x_by(new_x,sprite);
    }
    else if (ID=="change_y") {
        double new_y = stod(block.values[0]);
        change_y_by(new_y,sprite);
    }
    else if (ID=="go_to_x_y") {
        double new_x = stod(block.values[0]);
        double new_y = stod(block.values[1]);
        go_to_x_y(new_x,new_y,sprite);
    }
    else if (ID=="set_x") {
        double new_x = stod(block.values[0]);
        set_x_to(new_x,sprite);
    }
    else if (ID=="set_y") {
        double new_y = stod(block.values[0]);
        set_y_to(new_y,sprite);
    }

}
void UpdateExecution() {
    if (!isExecuting) return;
    if (executingChainIndex < 0 || executingChainIndex >= blockChains.size()) {
        isExecuting = false;
        return;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - executionStartTime > 500) {
        executingBlockIndex++;

        if (executingBlockIndex >= blockChains[executingChainIndex].size()) {
            isExecuting = false;
            executingChainIndex = -1;
            executingBlockIndex = -1;
        } else {
            Blocks& block = blockChains[executingChainIndex][executingBlockIndex];
            Executing_Motion_Blocks(block, now_sprite);
            executionStartTime = currentTime;
        }
    }
}

bool FindClickedBlock(int mx, int my, int& chainIdx, int& blockIdx) {
    for (int c = 0; c < blockChains.size(); c++) {
        for (int b = 0; b < blockChains[c].size(); b++) {
            SDL_Rect rect = blockChains[c][b].rect;
            if (mx >= rect.x && mx <= rect.x + rect.w &&
                my >= rect.y && my <= rect.y + rect.h) {
                chainIdx = c;
                blockIdx = b;
                return true;
                }
        }
    }
    return false;
}
void SplitChain(int chainIdx, int blockIdx) {
    if (chainIdx < 0 || chainIdx >= blockChains.size()) return;
    if (blockIdx <= 0 || blockIdx >= blockChains[chainIdx].size()) return;

    vector<Blocks> firstPart, secondPart;
    for (int i = 0; i < blockIdx; i++) {
        firstPart.push_back(blockChains[chainIdx][i]);
    }
    for (int i = blockIdx; i < blockChains[chainIdx].size(); i++) {
        secondPart.push_back(blockChains[chainIdx][i]);
    }
    blockChains.erase(blockChains.begin() + chainIdx);
    if (!firstPart.empty()) {
        blockChains.push_back(firstPart);
    }
    if (!secondPart.empty()) {
        blockChains.push_back(secondPart);
    }
}
void MergeChains(int targetIdx, int sourceIdx, bool toTop) {
    if (targetIdx < 0 || targetIdx >= blockChains.size()) return;
    if (sourceIdx < 0 || sourceIdx >= blockChains.size()) return;
    if (targetIdx == sourceIdx) return;

    if (toTop) {
        blockChains[targetIdx].insert(
            blockChains[targetIdx].begin(),
            blockChains[sourceIdx].begin(),
            blockChains[sourceIdx].end()
        );
    } else {
        blockChains[targetIdx].insert(
            blockChains[targetIdx].end(),
            blockChains[sourceIdx].begin(),
            blockChains[sourceIdx].end()
        );
    }

    blockChains.erase(blockChains.begin() + sourceIdx);
}
bool IsNearForSnap(Blocks& block1, Blocks& block2) {
    int verticalGap1 = block2.rect.y - (block1.rect.y + block1.rect.h);
    bool horizontallyAligned1 = (abs(block1.rect.x - block2.rect.x) < 10);
    int verticalGap2 = block1.rect.y - (block2.rect.y + block2.rect.h);
    bool horizontallyAligned2 = (abs(block1.rect.x - block2.rect.x) < 10);

    return (horizontallyAligned1 && abs(verticalGap1) < 10) ||
           (horizontallyAligned2 && abs(verticalGap2) < 10);
}
void SnapBlocks(Blocks& upperBlock, Blocks& lowerBlock) {

    lowerBlock.rect.x = upperBlock.rect.x;
    lowerBlock.rect.y = upperBlock.rect.y + upperBlock.rect.h - 5;

    upperBlock.next = &lowerBlock;
    lowerBlock.prev = &upperBlock;
}

void SnapBlocksToTop(Blocks& newTopBlock, Blocks& existingChain) {

    newTopBlock.rect.x = existingChain.rect.x;
    newTopBlock.rect.y = existingChain.rect.y - newTopBlock.rect.h + 5;

    newTopBlock.next = &existingChain;
    existingChain.prev = &newTopBlock;
}
void ExecuteChain(int chainIndex) {
    if (chainIndex < 0 || chainIndex >= blockChains.size()) return;

    executingChainIndex = chainIndex;
    executingBlockIndex = 0;
    executionStartTime = SDL_GetTicks();
    isExecuting = true;
    if (!blockChains[executingChainIndex].empty()) {
        Blocks& block = blockChains[executingChainIndex][0];
        Executing_Motion_Blocks(block, now_sprite);
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
    IMG_Init(IMG_INIT_PNG);
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
    thinking_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",20);
    if(thinking_font== nullptr){
        std::cout << "Thinking Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    File_Text = LoadText(renderer,main_font,"File",white);
    Sound_text = LoadText(renderer,main_font,"Sounds",{120,147,149});
    Backdrop_text = LoadText(renderer,main_font,"Backdrop",{120,147,149});
    Back_text = LoadText(renderer,main_font,"Back",{120,147,149});
    code_text = LoadText(renderer,main_font,"Code",{120,147,149});
    Timer_text = LoadText(renderer,main_font,"Timer",{100,100,100});
    size_button_text = LoadText(renderer,main_font,"Size",{100,100,100});
    volume_value = LoadText(renderer,main_font,to_string(volume),{100,100,100});
    volume_text = LoadText(renderer,main_font,"Volume : ",{100,100,100});
    frequency_value = LoadText(renderer,main_font,to_string(frequency),{100,100,100});
    frequency_text = LoadText(renderer,main_font,"Frequency : ",{100,100,100});
    next_costume_text = LoadText(renderer,main_font,"next costume",{100,100,100});
    costume_number_text = LoadText(renderer,main_font,"costume number",{100,100,100});
    Scratch_logo = LoadTexture(renderer,"asset/images/logo/scratch.png");
    SetWindowIcon(main_window,"asset/images/logo/icon.png");
    green_flag = LoadTexture(renderer,"asset/images/logo/flag.png");
    stop_sign = LoadTexture(renderer,"asset/images/logo/stop.png");
    X_text = LoadText(renderer,report_font,"X",{100,100,100});
    Y_text = LoadText(renderer,report_font,"Y",{100,100,100});
    size_text = LoadText(renderer,report_font,"Size",{100,100,100});
    direction_text = LoadText(renderer,report_font,"Direction",{100,100,100});
    show_text = LoadText(renderer,report_font,"Show",{100,100,100});
    sprite_text = LoadText(renderer,report_font,"Sprite",{100,100,100});
    positionX_text = LoadText(renderer,report_font,to_string(now_sprite->x),{100,100,100});
    positionY_text = LoadText(renderer,report_font, to_string(now_sprite->y),{100,100,100});
    size_of_sprite_text = LoadText(renderer,report_font, to_string(now_sprite->size),{100,100,100});
    direction_of_sprite_text = LoadText(renderer,report_font, to_string(now_sprite->degree),{100,100,100});
    name_of_sprite_text = LoadText(renderer,report_font,now_sprite->name,{100,100,100});
    S_text = LoadText(renderer,report_font,"S",{100,100,100});
    H_text = LoadText(renderer,report_font,"H",{100,100,100});
    Run_text = LoadText(renderer,main_font,"Run",white);
    volumeUp_text = LoadText(renderer,main_font,"Volume Up",white);
    volumeDown_text = LoadText(renderer,main_font,"Volume Down",white);
    increase_frequency_text = LoadText(renderer,main_font,"Increase Frequency",white);
    decrease_frequency_text = LoadText(renderer,main_font,"Decrease Frequency",white);
    code_bar_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf", 10);
    if(code_bar_font == nullptr){
        std::cout << "Code_bar Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    Init_structOfCharacter();
    Init_costume();
    Init_code_button(renderer,code_bar_font);
    LoadAllAssets(renderer);
    Init_Menu_Blocks();
    Init_flag_button();
    Init_stop_button();
    Init_sound_button();
    Init_Back_button();
    Init_Backdrop_button();
    Init_code_button();
    Init_sprite_box(*now_sprite);
    Init_positionX_box(*now_sprite);
    Init_positionY_box(*now_sprite);
    Init_size_box(*now_sprite);
    Init_direction_box(*now_sprite);
    Init_show_button();
    Init_hide_button();
    Init_run_sound_button();
    Init_volumeUp_button();
    Init_volumeDown_button();
    Init_increase_frequency_button();
    Init_decrease_frequency_button();
    Init_timer_button();
    Init_size_button();
    Init_next_costume_button();
    Init_volume_button();
    Init_frequency_button();
    Init_costume_number_button();
    Load_Character(renderer,&cat,&cat1);
    Load_Character(renderer,&cat,&cat2);
    LoadBackdropLibraryManual(renderer);
    SDL_StartTextInput();
    return true;
}

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

    for (auto& chain : blockChains) {
        for (auto& block : chain) {
            block.is_editing = false;
            block.active_value_index = -1;
        }
    }
    for (int c = blockChains.size() - 1; c >= 0; c--) {
        for (int b = blockChains[c].size() - 1; b >= 0; b--) {
            Blocks& block = blockChains[c][b];

            if (SDL_PointInRect(&mPos, &block.rect)) {
                for (size_t j = 0; j < block.values.size(); j++) {
                    int px = blockMap[block.id].inputs[j].posX;
                    int centerY = block.rect.h / 2;
                    SDL_Rect inputArea = {
                            block.rect.x + px - 20,
                            block.rect.y + centerY - 10,
                            40, 20
                    };

                    if (SDL_PointInRect(&mPos, &inputArea)) {
                        block.is_editing = true;
                        block.active_value_index = (int)j;
                        block.values[j] = "";
                        SDL_StartTextInput();
                        foundFocus = true;
                        break;
                    }
                }
                if (foundFocus) break;
            }
            if (foundFocus) break;
        }
        if (foundFocus) break;
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
    if (isSaveModalOpen) {
        if (e.type == SDL_TEXTINPUT) {
            saveInputText += e.text.text;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !saveInputText.empty()) {
                saveInputText.pop_back();
            } else if (e.key.keysym.sym == SDLK_RETURN) {
                if (!saveInputText.empty()) SaveToLibrary(saveInputText, renderer);
            } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                isSaveModalOpen = false;
                saveInputText = "";
                SDL_StopTextInput();
            }
        }
        return;
    }

    if (isTyping && currentTab == BACKDROPS) {
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

    for (auto& chain : blockChains) {
        for (auto& block : chain) {
            if (block.is_editing && block.active_value_index != -1 &&
                block.active_value_index < (int)block.values.size()) {

                string& str = block.values[block.active_value_index];
                InputType currentType = blockMap[block.id].inputs[block.active_value_index].type;

                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && !str.empty()) {
                        str.pop_back();
                        UpdateBlockWidth(block, code_bar_font);
                    } else if (e.key.keysym.sym == SDLK_RETURN) {
                        int Last_Index = block.active_value_index;
                        block.is_editing = false;
                        block.active_value_index = -1;
                        SDL_StopTextInput();

                        if (str.empty() && Last_Index != -1) {
                            str = blockMap[block.id].inputs[Last_Index].defaultValue;
                            UpdateBlockWidth(block, code_bar_font);
                        }
                    }
                } else if (e.type == SDL_TEXTINPUT) {
                    bool valid_char = false;
                    if (currentType == INPUT_NUMBER) {
                        valid_char = (e.text.text[0] >= '0' && e.text.text[0] <= '9') ||
                                     e.text.text[0] == '-' || e.text.text[0] == '.';
                    } else if (currentType == INPUT_TEXT) {
                        valid_char = (e.text.text[0] >= 32 && e.text.text[0] <= 126);
                    } else {
                        valid_char = true;
                    }

                    if (valid_char && str.length() < 20) {
                        str += e.text.text;
                        UpdateBlockWidth(block, code_bar_font);
                    }
                    break;
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

    static int draggedChainIndex = -1;
    static int offsetX = 0, offsetY = 0;
    if (e.type == SDL_MOUSEWHEEL) {
        if (mx > 60 && mx < 310) {
            sidebar_scroll_y += e.wheel.y * 25;
            if (sidebar_scroll_y > 0) sidebar_scroll_y = 0;
            if (sidebar_scroll_y < -3000) sidebar_scroll_y = -3000;
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        clickStartTime = SDL_GetTicks();
        isClickAndHold = false;
        bool clickedOnInput = false;
        bool clickedOnMenu = false;
        CheckInputClick(mx, my);
        for (auto& chain : blockChains) {
            for (auto& block : chain) {
                if (block.is_editing) {
                    clickedOnInput = true;
                    break;
                }
            }
            if (clickedOnInput) break;
        }
        if (clickedOnInput) return;

        if (mx > 60 && mx < 310 && my > 95) {
            for (auto& mb : menu_blocks) {
                SDL_Rect actual_pos = mb.rect;
                actual_pos.y += sidebar_scroll_y;
                if (SDL_PointInRect(&mPos, &actual_pos)) {
                    Blocks new_block = mb;
                    new_block.values.clear();
                    if (blockMap.count(mb.id)) {
                        for (auto& Input : blockMap[mb.id].inputs) {
                            new_block.values.push_back(Input.defaultValue);
                        }
                    }
                    int correct_width = calculatingWidthBlock(blockMap[mb.id], new_block.values, code_bar_font);
                    new_block.rect.w = correct_width;
                    new_block.rect.h = blockMap[mb.id].height;
                    new_block.rect.x = mx - new_block.rect.w/2;
                    new_block.rect.y = my - new_block.rect.h/2;

                    blockChains.push_back({new_block});
                    draggedChainIndex = blockChains.size() - 1;
                    offsetX = mx - blockChains[draggedChainIndex][0].rect.x;
                    offsetY = my - blockChains[draggedChainIndex][0].rect.y;
                    clickedOnMenu = true;
                    break;
                }
            }
        }

        if (!clickedOnMenu) {
            int chainIdx, blockIdx;
            if (FindClickedBlock(mx, my, chainIdx, blockIdx)) {
                if (blockIdx > 0) {
                    SplitChain(chainIdx, blockIdx);
                    FindClickedBlock(mx, my, chainIdx, blockIdx);
                }

                draggedChainIndex = chainIdx;
                offsetX = mx - blockChains[draggedChainIndex][0].rect.x;
                offsetY = my - blockChains[draggedChainIndex][0].rect.y;
                // isClickAndHold = true;
            }
        }
    }

    if (e.type == SDL_MOUSEMOTION && draggedChainIndex != -1) {
        if (SDL_GetTicks() - clickStartTime > CLICK_THRESHOLD ||
            abs(mx - offsetX - blockChains[draggedChainIndex][0].rect.x) > 5) {
            int deltaX = mx - offsetX - blockChains[draggedChainIndex][0].rect.x;
            int deltaY = my - offsetY - blockChains[draggedChainIndex][0].rect.y;

            for (auto& block : blockChains[draggedChainIndex]) {
                block.rect.x += deltaX;
                block.rect.y += deltaY;
            }
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        if (draggedChainIndex != -1 && SDL_GetTicks() - clickStartTime < CLICK_THRESHOLD && !isClickAndHold) {
            ExecuteChain(draggedChainIndex);
        }
        else if (draggedChainIndex != -1) {
            int safeZoneX_Start = 310;
            int safeZoneY_Start = 95;

            bool shouldDelete = false;
            for (auto& block : blockChains[draggedChainIndex]) {
                if (block.rect.x < safeZoneX_Start || block.rect.y < safeZoneY_Start || block.rect.x > 900) {
                    shouldDelete = true;
                    break;
                }
            }

            if (shouldDelete) {
                blockChains.erase(blockChains.begin() + draggedChainIndex);
            }
            else {
                bool snapped = false;

                for (int i = 0; i < blockChains.size(); i++) {
                    if (i == draggedChainIndex) continue;
                    if (blockChains[i].empty()) continue;

                    Blocks& firstBlock = blockChains[i].front();
                    Blocks& lastBlock = blockChains[i].back();
                    Blocks& draggedFirst = blockChains[draggedChainIndex].front();
                    Blocks& draggedLast = blockChains[draggedChainIndex].back();

                    if (IsNearForSnap(lastBlock, draggedFirst)) {
                        int deltaY = (lastBlock.rect.y + lastBlock.rect.h - 5) - draggedFirst.rect.y;
                        for (auto& block : blockChains[draggedChainIndex]) {
                            block.rect.y += deltaY;
                            block.rect.x = lastBlock.rect.x;
                        }

                        MergeChains(i, draggedChainIndex, false);
                        snapped = true;
                        break;
                    }

                    if (IsNearForSnap(draggedLast, firstBlock)) {
                        int deltaY = (firstBlock.rect.y - draggedLast.rect.h + 5) - draggedFirst.rect.y;
                        for (auto& block : blockChains[draggedChainIndex]) {
                            block.rect.y += deltaY;
                            block.rect.x = firstBlock.rect.x;
                        }

                        MergeChains(i, draggedChainIndex, true);
                        snapped = true;
                        break;
                    }
                }

            }
        }

        draggedChainIndex = -1;
        isClickAndHold = false;
    }
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
void HandleToolSelection(int mx, int my) {
    int toolX = 115, toolY = 220, btnS = 45;
    for (int i = 0; i < 9; i++) {
        SDL_Rect r = { toolX + (i % 2) * 55, toolY + (i / 2) * 55, btnS, btnS };
        if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
            if (i == 8) {
                currentTab = CODE;
            }
            else if (i == 7) {
                if (selectedBackdropIndex >= 0) {
                    isSaveModalOpen = true;
                    saveInputText = projectBackdrops[selectedBackdropIndex].name;
                    SDL_StartTextInput();
                }
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
void MapMouseToCanvas(int mx, int my, int* outX, int* outY, SDL_Texture* target) {
    int tw, th;
    SDL_QueryTexture(target, NULL, NULL, &tw, &th);
    *outX = (mx - 330) * tw / 600;
    *outY = (my - 280) * th / 380;
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

void SaveToLibrary(string name, SDL_Renderer* renderer) {
    if (selectedBackdropIndex < 0 || selectedBackdropIndex >= (int)projectBackdrops.size()) return;

    Backdrop& bd = projectBackdrops[selectedBackdropIndex];
    int w, h;
    SDL_QueryTexture(bd.texture, NULL, NULL, &w, &h);

    SDL_Texture* combinedTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

    SDL_SetRenderTarget(renderer, combinedTex);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, bd.texture, NULL, NULL);
    if (bd.drawingLayer) {
        SDL_RenderCopy(renderer, bd.drawingLayer, NULL, NULL);
    }

    SDL_SetRenderTarget(renderer, oldTarget);

    BackdropItem newItem;
    newItem.texture = combinedTex;
    newItem.name = name;
    libraryItems.push_back(newItem);

    isSaveModalOpen = false;
    saveInputText = "";
    SDL_StopTextInput();
}

void Get_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) stop = true;
        Handle_event_for_code_button(e);
        Handle_event_for_motion_sprite(e,now_sprite);
        Handle_event_for_flag_button(e,flag_button);
        Handle_event_for_stop_button(e,stop_button);
        Handle_event_for_show_button(e,show_button,now_sprite);
        Handle_event_for_hide_button(e,hide_button,now_sprite);
        Handle_event_for_Back_button(e,&Back_button);
        Handle_event_for_Backdrop_button(e,&Backdrop_button);
        Handle_event_for_sound_button(e,&Sounds_button);
        Handle_event_for_Code_button(e,&code_button);
        if(currentTab == SOUNDS) {
            Handle_event_for_run_button(e, &run_sound_button);
            Handle_event_for_volumeUp_button(e, &volumeUp_button);
            Handle_event_for_volumeDown_button(e, &volumeDown_button);
            Handle_event_for_increaseFrequency_button(e, &increase_frequency_button);
            Handle_event_for_decreaseFrequency_button(e, &decrease_frequency_button);
        }
        if(currentTab == CODE) {
            Handle_event_for_timer_button(e, &Timer_button);
            Handle_event_for_next_costume_button(e, renderer, &next_costume_button, now_sprite);
            Handle_event_for_costume_number_button(e, &costume_number_button);
            Handle_event_for_size_button(e, &size_button);
        }
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
            HandleCanvasMouseDown(mx,my);
        }

        if (isLeftPressed && !isLibraryOpen && !isSaveModalOpen) {
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
    int mW = 400, mH = 200;
    int mX = (Get_width() - mW) / 2;
    int mY = (Get_height() - mH) / 2;

    SDL_Rect bg = { mX, mY, mW, mH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);

    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Texture* title = LoadText(renderer, font, "Save Backdrop As:", black);
    SDL_Rect titleRect = { mX + 20, mY + 20, 0, 0 };
    SDL_QueryTexture(title, NULL, NULL, &titleRect.w, &titleRect.h);
    SDL_RenderCopy(renderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

    SDL_Rect inputArea = { mX + 20, mY + 60, mW - 40, 40 };
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &inputArea);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &inputArea);

    if (!saveInputText.empty()) {
        SDL_Texture* inputTex = LoadText(renderer, font, saveInputText, black);
        SDL_Rect txtR = { mX + 30, mY + 70, 0, 0 };
        SDL_QueryTexture(inputTex, NULL, NULL, &txtR.w, &txtR.h);
        SDL_RenderCopy(renderer, inputTex, NULL, &txtR);
        SDL_DestroyTexture(inputTex);
    }

    SDL_Rect btnSave = { mX + 280, mY + 140, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderFillRect(renderer, &btnSave);
    SDL_Texture* sText = LoadText(renderer, font, "Save", { 255, 255, 255, 255 });
    SDL_Rect sTextR = { mX + 310, mY + 150, 40, 20 };
    SDL_RenderCopy(renderer, sText, NULL, &sTextR);
    SDL_DestroyTexture(sText);

    SDL_Rect btnCancel = { mX + 170, mY + 140, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &btnCancel);
    SDL_Texture* cText = LoadText(renderer, font, "Cancel", black);
    SDL_Rect cTextR = { mX + 195, mY + 150, 50, 20 };
    SDL_RenderCopy(renderer, cText, NULL, &cTextR);
    SDL_DestroyTexture(cText);
}

void Update() {
    UpdateMenuState();
    UpdateExecution();

    SDL_SetRenderDrawColor(renderer, 229, 240, 255, 255);
    SDL_RenderClear(renderer);
    Draw_flag_and_stop_button(renderer, flag_button, stop_button, green_flag, stop_sign);
    if (isLibraryOpen) {
        DrawBackdropLibrary(renderer, main_font);
    } else {
        Draw_BlueBar_Top(renderer, Get_width(), Scratch_logo);
        Draw_Top_Button(renderer, Top_button, File_Text);
        if (currentTab == SOUNDS) {
            Draw_sound_panel(renderer);
            Draw_sounds_functions_button(renderer, run_sound_button, Run_text);
            Draw_sounds_functions_button(renderer, volumeUp_button, volumeUp_text);
            Draw_sounds_functions_button(renderer, volumeDown_button, volumeDown_text);
            Draw_sounds_functions_button(renderer, increase_frequency_button, increase_frequency_text);
            Draw_sounds_functions_button(renderer, decrease_frequency_button, decrease_frequency_text);
            Draw_report_button(renderer,&volume_button,volume_value);
            Draw_volume_text(renderer,volume_text);
            Draw_report_button(renderer,&frequency_button,frequency_value);
            Draw_frequency_text(renderer,frequency_text);
        }
        if (currentTab == CODE) {
            Draw_RunningBar(renderer);
            Draw_CodeBar(renderer);
            Draw_CodeBar_Item(renderer, categories);
            Draw_Menu_Blocks(renderer, code_bar_font);
            DrawALLBlocks(renderer, code_bar_font);
            Draw_report_button(renderer, &Timer_button, Timer_text);
            Draw_report_button(renderer, &size_button, size_button_text);
            Draw_report_button(renderer, &next_costume_button, next_costume_text);
            Draw_report_button(renderer, &costume_number_button, costume_number_text);
        } else if (currentTab == BACKDROPS || currentTab == COSTUMES) {
            Draw_Backdrop_List_Sidebar(renderer, main_font);

            SDL_Texture* baseTex = nullptr;
            string bName = "Backdrop";
            if (!projectBackdrops.empty() && selectedBackdropIndex >= 0) {
                baseTex = projectBackdrops[selectedBackdropIndex].texture;
                bName = projectBackdrops[selectedBackdropIndex].name;
            }

            Draw_Image_Editor(renderer, main_font, baseTex, bName);

            if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int) projectBackdrops.size()) {
                if (isDrawingCircle && globalEditor.activeTool == TOOL_CIRCLE) {
                    int curX, curY;
                    SDL_GetMouseState(&curX, &curY);
                    int radius = (int) sqrt(pow(curX - circleStartX, 2) + pow(curY - circleStartY, 2));
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g,
                                           globalEditor.currentColor.b, 255);
                    for (int a = 0; a < 360; a++) {
                        float r1 = a * M_PI / 180.0f;
                        float r2 = (a + 1) * M_PI / 180.0f;
                        SDL_RenderDrawLine(renderer, circleStartX + (int) (radius * cos(r1)),
                                           circleStartY + (int) (radius * sin(r1)),
                                           circleStartX + (int) (radius * cos(r2)),
                                           circleStartY + (int) (radius * sin(r2)));
                    }
                } else if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) {
                    int curX, curY;
                    SDL_GetMouseState(&curX, &curY);
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g,
                                           globalEditor.currentColor.b, 255);
                    SDL_RenderDrawLine(renderer, lineStartX, lineStartY, curX, curY);
                }
            }
        }
        Draw_Information_of_Character(renderer);
        Draw_Code_button(renderer, code_button, code_text);
        Draw_sound_button(renderer, Sounds_button, Sound_text);
        Draw_Backdrop_button(renderer, Backdrop_button, Backdrop_text);
        Draw_Back_button(renderer, Back_button, Back_text);
        Draw_Character_Show_Bar(renderer);
        Draw_Stage_Bar(renderer, main_font);
        Draw_Stage_Content(renderer);
        Draw_report_informationOfCharacter_box(renderer, name_of_sprite);
        Draw_report_informationOfCharacter_box(renderer, direction);
        Draw_report_informationOfCharacter_box(renderer, size);
        Draw_report_informationOfCharacter_box(renderer, positionX);
        Draw_report_informationOfCharacter_box(renderer, positionY);
        Draw_X_text(renderer, X_text);
        Draw_Y_text(renderer, Y_text);
        Draw_size_text(renderer, size_text);
        Draw_direction_text(renderer, direction_text);
        Draw_sprite_text(renderer, sprite_text);
        Draw_show_text(renderer, show_text);
        volume_value = LoadText(renderer,main_font,to_string(volume),{100,100,100});
        frequency_value = LoadText(renderer,main_font,to_string(frequency),{100,100,100});
        Draw_show_and_hide_button(renderer, show_button, hide_button, S_text, H_text, now_sprite);
        name_of_sprite_text = LoadText(renderer, report_font, now_sprite->name, {100, 100, 100});
        Draw_name_of_character(renderer, name_of_sprite, name_of_sprite_text);
        positionX_text = LoadText(renderer, report_font, to_string((int)now_sprite->x), {100, 100, 100});
        Draw_positionX(renderer, positionX, positionX_text);
        positionY_text = LoadText(renderer, report_font, to_string(-(int)now_sprite->y), {100, 100, 100});
        Draw_positionX(renderer, positionY, positionY_text);
        size_of_sprite_text = LoadText(renderer, report_font, to_string((int)(now_sprite->size *500)), {100, 100, 100});
        Draw_size(renderer, size, size_of_sprite_text);
        direction_of_sprite_text = LoadText(renderer, report_font, to_string((int)now_sprite->degree), {100, 100, 100});
        Draw_direction(renderer, direction, direction_of_sprite_text);
        Draw_Character(renderer, now_sprite);
        if (is_timer) timer(renderer, report_font);
        if (is_size_on) Draw_size_report(renderer, report_font, now_sprite);
        if (is_costume_number_on) Draw_costume_report(renderer, report_font, now_sprite);
    }
    if (isSaveModalOpen) {
        DrawSaveModal(renderer, main_font);
    }
    SDL_RenderPresent(renderer);
}
void Render(){
        SDL_RenderPresent(renderer);
}

void Clean(){
        TTF_CloseFont(main_font);
        TTF_Quit();
        Mix_CloseAudio();
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





#include "Game.h"
#include "TextureManager.h"
#include "iostream"
#include "SDL2/SDL_ttf.h"
#include "Graphic_Element.h"
#include "constants.h"
#include "Entity.h"
#include <Asset_Loader.h>
#include <map>
#include <vector>
using namespace std;


SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* Scratch_logo = nullptr;
SDL_Texture* File_Text = nullptr;
TTF_Font* main_font = nullptr;

vector<Blocks> active_blocks;
bool stop = false;

void AddBlock(string type, int x, int y){
    Blocks block;
    block.image = blockLibrary[type];
    block.id = type;
    block.rect = {x, y, blockMap[type].width, blockMap[type].height};
    if (blockMap.count(type)) {
        for (auto & i : blockMap[type].inputs) {
            block.values.push_back(i.default_value);
        }
    } else {
        block.values.emplace_back("0");
    }
    block.active_value_index = 0;
    block.is_editing = false;
    block.is_menu_open = false;
    active_blocks.push_back(block);
}

void HandleEvents(SDL_Event& e){
    if (e.type == SDL_MOUSEBUTTONDOWN){
        int mx = e.button.x;
        int my = e.button.y;
        bool click = false;

        for (auto &block : active_blocks) {
            bool foundInThisBlock = false;

            if (blockMap.count(block.id)) {
                for (size_t i = 0; i < block.values.size(); i++) {
                    int px = blockMap[block.id].inputs[i].posx;
                    SDL_Rect inputRect = { block.rect.x + px - 20, block.rect.y + 10, 40, 25 };

                    SDL_Point mPos = { mx, my };

                    if (SDL_PointInRect(&mPos, &inputRect)) {
                        for(auto &other : active_blocks) other.is_editing = false;

                        block.is_editing = true;
                        block.active_value_index = (int)i;
                        SDL_StartTextInput();
                        click = true;
                        foundInThisBlock = true;
                        break;
                    }
                }
            }
            if (!foundInThisBlock) block.is_editing = false;
            if (click) break;
        }
        if (!click) SDL_StopTextInput();
    }
    if (e.type == SDL_TEXTINPUT) {
        for (auto& b : active_blocks) {
            if (b.is_editing) {
                if (isdigit(e.text.text[0]) || e.text.text[0] == '-') {
                    b.values[b.active_value_index] += e.text.text;
                }
            }
        }
    }

    if (e.type == SDL_KEYDOWN) {
        for (auto& b : active_blocks) {
            if (b.is_editing && e.key.keysym.sym == SDLK_BACKSPACE) {
                string& s = b.values[b.active_value_index];
                if (!s.empty()) s.pop_back();
            }
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
        SDL_SetRenderDrawColor(renderer,77,151,255,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(3000);
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
        File_Text = LoadText(renderer,main_font,"File",{255,255,255});
        Scratch_logo = LoadTexture(renderer,"asset/images/scratch.png");
        SetWindowIcon(main_window,"asset/images/icon.png");

        Init_Button();
        main_font = TTF_OpenFont("fonts/Montserrat-Bold.ttf", 10);
        if (main_font != nullptr) {
            Init_Categories(renderer, main_font);
        }
        LoadAllAssets(renderer);
        AddBlock("go_to_x_y", 300, 200);
        AddBlock("turn_left", 300, 260);
        AddBlock("turn_right", 300, 320);
        AddBlock("set_x", 300, 380);


    return true;
}



void Update(){
        Draw_BlueBar_Top(renderer,Get_width(),Scratch_logo);
        Draw_Button(renderer,file_button,File_Text);
        Draw_CodeBar(renderer, categories);
        DrawALLBlocks(renderer, main_font);
}

void Get_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) stop = true;
        HandleEvents(e);
    }
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
        SDL_Event *e = new SDL_Event();
        while(e->type != SDL_KEYDOWN){
                SDL_PollEvent(e);
        }
}





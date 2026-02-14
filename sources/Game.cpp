#include "Game.h"
#include "TextureManager.h"
#include "iostream"
#include "SDL2/SDL_ttf.h"
#include "Graphic_Element.h"
#include "constants.h"
#include "Entity.h"
#include <Asset_Loader.h>
#include <map>
using namespace std;


SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* Scratch_logo = nullptr;
SDL_Texture* File_Text = nullptr;
TTF_Font* loading_font = nullptr;
TTF_Font* main_font = nullptr;
TTF_Font* code_bar_font = nullptr;

bool stop = false;

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

        return true;
}

void Get_event() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) stop = true;
                if (e.type == SDL_MOUSEBUTTONDOWN){
                        if(e.button.button == SDL_BUTTON_LEFT){
                                for (int i=0;i<8;i++){
                                        if(Is_mouse_on(categories[i].rect.x,categories[i].rect.y,categories[i].rect.w,categories[i].rect.h)){
                                                for(int j=0;j<8;j++){
                                                        categories[j].is_mouse_click_on = false;
                                                }
                                                categories[i].is_mouse_click_on = true;
                                        }

                                }
                        }
                }
        }
}

void Update(){
        Draw_BlueBar_Top(renderer,Get_width(),Scratch_logo);
        Draw_Top_Button(renderer,Top_button,File_Text);
        Draw_CodeBar(renderer, categories);
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





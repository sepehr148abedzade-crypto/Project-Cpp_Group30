#include "Game.h"

bool stop = false;

SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;

bool Init_Game(){
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                std::cout << "SDL could not initialize! SDL_Error : " << SDL_GetError() << std::endl;
                return false;
        }

        main_window= SDL_CreateWindow(
                "Scratch",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                0,
                0,
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

        return true;
}

void Give_event(){
        SDL_Event e;
        while(SDL_PollEvent(&e)!=0){
                if(e.type == SDL_QUIT) stop = true;
        }
}
void clean(){
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





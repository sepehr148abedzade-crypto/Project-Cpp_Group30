#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>

int main( int argc, char *argv[] ){
    std::cout << "hello my name is Sepehr" << std::endl;
    std :: cout << "hello my name is Amirali" << std::endl;
    //please print your name like me and me{AmirAli} :)

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Window * W1 = SDL_CreateWindow(
        "WINDOW 1",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        400,300,
        SDL_WINDOW_SHOWN
    );
    if (W1 == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }
    return 0;
}
#include "first_page.h"


int main(int argc, char * argv[])
{
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");



    SDL_Window* window = SDL_CreateWindow(
            "Scratch",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //vizegi haye dokmeh file
    Button file_button{};
    file_button.rect = {140, 0, 50, 50};
    file_button.first_color = {77, 151, 255, 150};
    file_button.second_color = {66, 128, 217};
    file_button.is_mouse_on = false;



    // iconikeh dar bala dideh mishe.
    SDL_Surface* iconSurface = IMG_Load("images/icon.png");
    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    }


    Code_button categories[8];

    SDL_Texture* Scratch_logo = IMG_LoadTexture(renderer, "images/Scratch.png");
    SDL_Texture* leftbar = IMG_LoadTexture(renderer, "images/code_bar.png");


    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 229, 240, 255, 255);
        SDL_RenderClear(renderer);

        // barresi inkeh age mouse roye file bashe rangesh avaz besheh
        int mX, mY;
        SDL_GetMouseState(&mX, &mY);
        if (mX >= file_button.rect.x && mX<= file_button.rect.x + file_button.rect.w
            && mY >= file_button.rect.y && mY<= file_button.rect.y + file_button.rect.h)
        {
            file_button.is_mouse_on = true;
        }else
            file_button.is_mouse_on = false;
        blue_bar(renderer, Scratch_logo, file_button);
        codebar(renderer, leftbar, categories);

        SDL_RenderPresent(renderer);
    }



    return 0;
}

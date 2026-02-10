#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfx.h>

using namespace std;

struct Button{
    SDL_Rect rect;
    SDL_Color first_color;
    SDL_Color second_color;
    bool is_mouse_on;
};

struct Code_button{
    int x=30, y, r=15;
    SDL_Color color;
    string label;
};

const int width = 1300, height = 800;

//tabeh baraye navar abi
void blue_bar(SDL_Renderer* renderer, SDL_Texture* logo, Button file_button)
{
    SDL_Rect bar = {0, 0, 1300, 50};
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 150);
    SDL_RenderFillRect(renderer, &bar);

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("fonts/Montserrat-Bold.ttf", 15);
    SDL_Color white = {255, 255, 255, 255};


    if (logo != nullptr)
    {
        SDL_Rect logo_rect = {10, 10, 60, 30};
        SDL_RenderCopy(renderer, logo, nullptr, &logo_rect);
    }

    if (file_button.is_mouse_on)
    {
        SDL_SetRenderDrawColor(renderer, file_button.second_color.r, file_button.second_color.g,
                               file_button.second_color.b, 255);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, file_button.first_color.r, file_button.first_color.g,
                               file_button.first_color.b, 255);
    }
    SDL_RenderFillRect(renderer, &file_button.rect);

    // neveshtan matn "file" dar vasat dokmeh
    if (font != nullptr) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* textSurf = TTF_RenderText_Blended(font, "File", white);
        if (textSurf) {
            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
            int tw, th;
            SDL_QueryTexture(textTex, nullptr, nullptr, &tw, &th);
            SDL_Rect textPos = {
                    file_button.rect.x + (file_button.rect.w - tw) / 2,
                    file_button.rect.y + (file_button.rect.h - th) / 2,
                    tw, th
            };
            SDL_RenderCopy(renderer, textTex, nullptr, &textPos);

            SDL_FreeSurface(textSurf);
            SDL_DestroyTexture(textTex);
        }
    }
}

void codebar(SDL_Renderer* renderer, SDL_Texture* image, Code_button* codeButton)
{
    SDL_Rect left_bar = {0, 120, 60, 700};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &left_bar);
    SDL_Color colors[] = {
            {76, 151, 255},
            {153, 102, 255},
            {214, 91, 229},
            {255, 191, 0},
            {255, 171, 25},
            {76, 191, 230},
            {89, 192, 89},
            {255, 140, 26},
    };
    string label[] = {"Motion", "Looks", "Sound", "Events", "Control", "Sensing", "Operators", "Variables"};

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("fonts/Montserrat-Bold.ttf", 15);
    SDL_Color black = {0, 0, 0, 0};

    int x = 30, y = 150, r = 15;
    for (int i = 0; i < 8; ++i)
    {

        codeButton[i].y = y;
        codeButton[i].color = {colors[i].r, colors[i].g, colors[i].b, 255};
        filledCircleRGBA(renderer, x, y, r, colors[i].r, colors[i].g,colors[i].b, 255);
        aacircleRGBA(renderer, codeButton[i].x, codeButton[i].y, codeButton[i].r, 0, 0, 0, 50);
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* textSurf = TTF_RenderText_Blended(font, "File", white);


        y+= 60;
    }
}




int main(int argc, char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO);


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
    SDL_Surface* iconSurface = IMG_Load("asset/images/icon.png");
    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    }


    Code_button categories[8];

    SDL_Texture* Scratch_logo = IMG_LoadTexture(renderer, "asset/images/Scratch.png");
    SDL_Texture* leftbar = IMG_LoadTexture(renderer, "asset/images/code_bar.png");


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

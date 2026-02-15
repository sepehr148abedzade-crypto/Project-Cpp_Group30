#include "Entity.h"
#include "Graphic_Element.h"
//#include "Game.h"

extern SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
Button file_button{};
void Init_Button() {
    file_button.rect = {140, 0, 40, 48};
    file_button.first_color = {77, 151, 255, 150};
    file_button.second_color = {66, 128, 217};
    file_button.is_mouse_on = false;
}

Code_button categories[8];
void Init_Categories(SDL_Renderer* renderer, TTF_Font* font) {
    string labels[] = {"Motion", "Looks", "Sound", "Events", "Control", "Sensing", "Operators", "Variables"};
    uint32_t colors[] = {0x4C97FF, 0x9966FF, 0xD65BE5, 0xFFBF00, 0xFFAB19, 0x4CBFE6, 0x59C059, 0xFF8C1A};

    int currentY = 120;
    for (int i = 0; i < 8; i++) {
        categories[i].x = 30;
        categories[i].y = currentY;
        categories[i].r = 15;
        categories[i].labelTexture = LoadText(renderer, font, labels[i], {255, 255, 255, 255});
        categories[i].color = Hex_To_rgb(colors[i]);
        categories[i].is_mouse_on = false;
        currentY += 60;
    }
}
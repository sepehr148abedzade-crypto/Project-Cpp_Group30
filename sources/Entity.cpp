#include "Entity.h"
#include "Graphic_Element.h"
#include "constants.h"
//#include "Asset_Loader.h"


vector<Blocks> active_blocks;
std::vector<Blocks> menu_blocks;
std::map<std::string, BlockTemplate> blockMap;
std::map<std::string, SDL_Texture*> blockLibrary;
Button Top_button{};
void Init_Button() {
    Top_button.rect = {140, 0, 40, 48};
    Top_button.first_color = {77, 151, 255, 150};
    Top_button.second_color = {66, 128, 217};
    Top_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
}
Button Load_button{};
void Init_Load_button(){
    Load_button.rect = {Get_width()/2,Get_height()/2,0,0};
};
Button categories[8];
void Init_code_button(SDL_Renderer* renderer, TTF_Font* font) {
    string labels[] = {"Motion", "Looks", "Sound", "Events", "Control", "Sensing", "Operators", "Variables"};
    uint32_t colors[] = {0x4C97FF, 0x9966FF, 0xD65BE5, 0xFFBF00, 0xFFAB19, 0x4CBFE6, 0x59C059, 0xFF8C1A};

    int currentY = 95;
    for (int i = 0; i < 8; i++) {
        categories[i].rect={0,currentY,60,60};
        categories[i].radius = 10;
        categories[i].Label_texture = LoadText(renderer, font, labels[i], white);
        categories[i].first_color = Hex_To_rgb(colors[i]);
        categories[i].second_color = {33,64,105,SDL_ALPHA_OPAQUE};
        categories[i].is_mouse_on = Is_mouse_on(categories[i].rect.x,categories[i].rect.y,categories[i].rect.w,categories[i].rect.h);
        categories[i].is_mouse_click_on = false;
        currentY += 60;
    }
}
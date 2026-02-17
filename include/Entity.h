#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;


enum InputType { NUMBER, DROPDOWN, TEXT };

struct InputField {
    int posX;
    InputType type;
    std::string defaultValue;
};

struct BlockTemplate {
    int width;
    int height;
    std::vector<InputField> inputs;
};

struct Character{
    std::string name;
    double x = 0;
    double y = 0;
    double degree = 0;
    double size = 0.25;
    double width=0;
    double height=0;
    bool isvisible = true;
    bool is_mouse_on = false;
    std::string monologue;
    std::string think;
    SDL_Texture* texture= nullptr;
    const char* path;
};

extern Character cat;
extern Character cat_running;
extern Character now_sprite;

struct Blocks{
    string id;
    SDL_Rect rect;
    SDL_Texture* image;
    vector<string> values;
    bool is_editing;int active_value_index = -1;


    Blocks* next = nullptr;
};



struct Button{
    SDL_Rect rect={0,0,0,0};
    int radius ;
    SDL_Texture* Label_texture;
    SDL_Color first_color;
    SDL_Color second_color;
    bool is_mouse_on;
    bool is_mouse_click_on;
};
extern Button Load_button;
extern Button Top_button;
extern Button categories[8];
extern std::vector<Blocks> active_blocks;
extern std::vector<Blocks> menu_blocks;
extern int sidebar_scroll_y;
extern Blocks* draggedBlock;
extern int offsetX, offsetY;
extern std::map<std::string, BlockTemplate> blockMap;
extern std::map<std::string, SDL_Texture*> blockLibrary;

void Init_Button();
void Init_Load_button();
void Init_code_button(SDL_Renderer* renderer, TTF_Font* font);

#endif //ENTITY_H

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

struct character{
    int x;
    int y;
    int degree;
    int size;
    std::string monologue;
    std::string think;
    bool show;

};
struct Blocks{
    string id;
    SDL_Rect rect;
    SDL_Texture* image;
    vector<string> values;
    bool is_editing;
    int active_value_index = -1;


    Blocks* next = nullptr;
};

struct Backdrop {
    string name;
    SDL_Texture* texture;
};

struct BackdropItem{
    SDL_Texture* texture;
    std::string name;
};

struct BackdropMenu{
    SDL_Rect BackDropCircle;
    bool is_open;
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

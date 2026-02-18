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

struct Imaginary_circle {
    int x;
    int y;
    int R;
};


enum InputType {
    INPUT_NUMBER,
    INPUT_DROPDOWN,
    INPUT_TEXT,
    INPUT_BOOLEAN
};

enum Block_category {
    CAT_MOTION,
    CAT_LOOK,
    CAT_SOUND,
    CAT_EVENT,
    CAT_CONTROL,
    CAT_SENSING,
    CAT_OPERATORS,
    CAT_VARIABLES,
    CAT_MY_BLOCK,
};
struct categoryInfo {
    Block_category category;
    bool active;
    int startY;
};

 extern vector<categoryInfo> categoryOrder;

struct InputField {
    int posX;
    InputType type;
    std::string defaultValue;
    std:: vector <std:: string> options;
};
enum BlockType {
    Simple_Block,
    Event_Block,
    C_Block,
    E_Block,
    Bool_Block,
    Expression_Block,
};

struct BlockTemplate {
    int width;
    int height;
    BlockType type;
    Block_category category;
    std::string Back_label;
    std::vector<std::string> labels;
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
    BlockType type;
    SDL_Texture* image;
    vector<string> values;
    bool is_editing;
    int active_value_index = -1;
    vector<Blocks*> children;
    Blocks* parent;

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

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

enum EditTool { TOOL_NONE,
    TOOL_SELECT,
    TOOL_PEN,
    TOOL_FILL,
    TOOL_LINE,
    TOOL_CIRCLE,
    TOOL_RECT,
    TOOL_TEXT,
    TOOL_ERASER};

struct EditorSettings {
    EditTool activeTool = TOOL_PEN;
    SDL_Color currentColor = {0, 0, 0, 255};
    int brushSize = 5;
    bool isDrawing = false;
};

extern EditorSettings globalEditor;

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

struct Costume{
    std::string name;
    SDL_Texture* texture;
    const char* path;
};

extern Costume cat1;
extern Costume cat2;

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
    const char* path;
    std::vector<Costume*> costumes;
    int currentCostumeIndex = 0;
};

extern Character cat;
extern Character cat_running;
extern Character* now_sprite;

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
    Blocks* next;
    Blocks* prev;
};

struct Backdrop {
    string name;
    SDL_Texture* texture;
    SDL_Texture* drawingLayer;
};
extern std::vector<Backdrop> projectBackdrops;
extern int selectedBackdropIndex;
extern SDL_Texture* currentBackdropTexture;

struct BackdropItem{
    SDL_Texture* texture;
    std::string name;
};

struct BackdropMenu{
    SDL_Rect BackDropCircle;
    bool is_open;
};

struct InformationOfCharacter{
    SDL_Rect rect;
    bool active = false;
    std::string text;
    double* linkedValue;
};
extern InformationOfCharacter name_of_sprite;
extern InformationOfCharacter positionX;
extern InformationOfCharacter positionY;
extern InformationOfCharacter direction;
extern InformationOfCharacter size;
struct Button{
    SDL_Rect rect={0,0,0,0};
    int radius ;
    SDL_Texture* Label_texture;
    SDL_Color first_color;
    SDL_Color second_color;
    SDL_Color third_color;
    bool is_mouse_on;
    bool is_mouse_click_on;
};
extern Button Load_button;
extern Button Top_button;
extern Button categories[9];
extern Button flag_button;
extern Button stop_button;
extern Button Sounds_button;
extern Button Backdrop_button;
extern Button Back_button;
extern Button code_button;
extern Button show_button;
extern Button hide_button;
extern Button run_sound_button;
extern Button volumeUp_button;
extern Button volumeDown_button;
extern Button increase_frequency_button;
extern Button Timer_button;
extern Button next_costume_button;
extern Button size_button;
extern Button costume_number_button;
extern Button volume_button;
extern Button frequency_button;
extern Button decrease_frequency_button;
//extern std::vector<Blocks> active_blocks;
extern vector<vector<Blocks>> blockChains;
extern int draggedChainIndex ;
extern int executingChainIndex;
extern int executingBlockIndex;
extern Uint32 executionStartTime;
extern bool isExecuting;
extern std::vector<Blocks> menu_blocks;
extern int sidebar_scroll_y;
extern Blocks* draggedBlock;
extern int offsetX, offsetY;
extern std::map<std::string, BlockTemplate> blockMap;
extern std::map<std::string, SDL_Texture*> blockLibrary;


void Init_Button();
void Init_flag_button();
void Init_stop_button();
void Init_sound_button();
void Init_Backdrop_button();
void Init_Back_button();
void Init_code_button();
void Init_hide_button();
void Init_show_button();
void Init_sprite_box(Character &sprite);
void Init_positionX_box(Character &sprite);
void Init_positionY_box(Character &sprite);
void Init_direction_box(Character &sprite);
void Init_size_box(Character &sprite);
void Init_Load_button();
void Init_code_button(SDL_Renderer* renderer, TTF_Font* font);
void Init_costume();
void Init_structOfCharacter();
void Init_run_sound_button();
void Init_volumeUp_button();
void Init_volumeDown_button();
void Init_increase_frequency_button();
void Init_decrease_frequency_button();
void Init_timer_button();
void Init_next_costume_button();
void Init_size_button();
void Init_costume_number_button();
void Init_volume_button();
void Init_frequency_button();
#endif //ENTITY_H

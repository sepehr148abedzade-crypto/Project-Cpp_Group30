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
enum DropdownType {
    DROPDOWN_COSTUME,
    DROPDOWN_SOUND,
    DROPDOWN_EFFECT,
    DROPDOWN_TOUCHING,
    DROPDOWN_FUNCTION,
    DROPDOWN_CUSTOM
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
    DropdownType dropdownType;
    bool isOpen = false;
};
struct InputValue {
    enum Type { DIRECT_VALUE, BLOCK_REFERENCE };
    Type type;
    string directValue;
    int refChainIndex;
    int refBlockIndex;
    int selectedOptionIndex = 0;
    bool isOpen = false;

    InputValue() : type(DIRECT_VALUE), directValue(""), refChainIndex(-1), refBlockIndex(-1) {}
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
    SDL_Texture* drawingLayer;
};

extern Costume cat1;
extern Costume cat2;
extern Costume fish1;
extern Costume fish2;
extern Costume fish3;
extern Costume bear1;
extern Costume bear2;
extern Costume dog1;
extern Costume dog2;
extern Costume apple;
extern Costume balloon1;
extern Costume balloon2;
extern Costume balloon3;
extern Costume emoji1;
extern Costume emoji2;
extern Costume emoji3;
extern Costume emoji4;

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
    bool is_draw_button = false;
    std::string monologue;
    std::string think;
    const char* path;
    std::vector<Costume*> costumes;
    int currentCostumeIndex = 0;

    Costume* getCurrentCostume() {
        if (!costumes.empty() && currentCostumeIndex >= 0 && currentCostumeIndex < costumes.size()) {
            return costumes[currentCostumeIndex];
        }
        return nullptr;
    }
};

extern Character cat;
extern Character bear;
extern Character fish;
extern Character balloon;
extern Character red_apple;
extern Character dog;
extern Character emoji;
extern Character* now_sprite;

struct sprite_button{
    SDL_Rect rect = {0,0,0,0};
    Character* sprite;
    bool active;
    bool is_now_sprite;
};

extern sprite_button cat_buttonUnderstage;
extern sprite_button dog_buttonUnderstage;
extern sprite_button apple_buttonUnderstage;
extern sprite_button balloon_buttonUnderstage;
extern sprite_button bear_buttonUnderstage;
extern sprite_button fish_buttonUnderstage;
extern sprite_button emoji_buttonUnderstage;

struct Blocks{
    string id;
    SDL_Rect rect;
    BlockType type;
    SDL_Texture* image;
    vector<InputValue> inputs;
    bool is_editing;
    int active_value_index = -1;
    Blocks* parent;
    Blocks* next;
    Blocks* prev;
    vector<Blocks*> childBlocks;
    int parentChainIndex = -1;
    int parentBlockIndex = -1;
    int parentInputIndex = -1;
    struct {
        int parentChainIndex = -1;
        int parentBlockIndex = -1;
        int parentInputIndex = -1;
    } prevRelationship;
    string output;
    bool outputBool;
    bool hasOutput = false;
    Uint32 outputDisplayTime = 0;
    bool isDisplayingOutput = false;
    bool needsEvaluation = true;
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
};struct HitResult {
    int chainIndex;
    int blockIndex;
    bool isOnInput;
    int inputIndex;
    bool isOnChildBlock;
    int childBlockIndex;
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
extern Button Character_button;
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
extern Button drag_button;
extern Button go_to_front_layer_button;
extern Button volume_button;
extern Button frequency_button;
extern Button decrease_frequency_button;
extern Button cat_button;
extern Button bear_button;
extern Button apple_button;
extern Button fish_button;
extern Button dog_button;
extern Button balloon_button;
extern Button emoji_button;
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
extern std::vector<sprite_button*> active_character;

void Init_Button();
void Init_flag_button();
void Init_stop_button();
void Init_Character_button();
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
void Init_drag_button();
void Init_go_to_front_layer_button();
void Init_size_button();
void Init_costume_number_button();
void Init_volume_button();
void Init_frequency_button();
void Init_cat_button();
void Init_bear_button();
void Init_fish_button();
void Init_balloon_button();
void Init_apple_button();
void Init_dog_button();
void Init_emoji_button();
void Init_sprite_buttonUnderStage();
void choose_now_sprite();
#endif //ENTITY_H

#include "Entity.h"
#include "Graphic_Element.h"
#include "constants.h"
//#include "Asset_Loader.h"


vector<Blocks> active_blocks;
std::vector<Blocks> menu_blocks;
std::map<std::string, BlockTemplate> blockMap;
std::map<std::string, SDL_Texture*> blockLibrary;
vector<categoryInfo> categoryOrder {
        {CAT_MOTION,true,100},
        {CAT_LOOK,true,0},
        {CAT_SOUND,true,0},
        {CAT_EVENT,true,0},
        {CAT_CONTROL,true,0},
        {CAT_SENSING,true,0},
        {CAT_OPERATORS,true,0},
        {CAT_VARIABLES,true,0},
    };

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
Button categories[9];
void Init_code_button(SDL_Renderer* renderer, TTF_Font* font) {
    string labels[] = {"Motion", "Looks", "Sound", "Events", "Control", "Sensing", "Operators", "Variables","My Blocks"};
    uint32_t colors[] = {0x4C97FF, 0x9966FF, 0xD65BE5, 0xFFBF00, 0xFFAB19, 0x4CBFE6, 0x59C059, 0xFF8C1A,0xFF6680};

    int currentY = 95;
    for (int i = 0; i < 9; i++) {
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
Button flag_button{};
void Init_flag_button(){
    flag_button.rect = {stage.x, stage.y-45, 40, 40};
    flag_button.first_color = {229,240,249};
    flag_button.second_color = {179, 210, 253};
    flag_button.third_color = {160,180,255};
    flag_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
}
Button stop_button{};
void Init_stop_button(){
    stop_button.rect = {stage.x+flag_button.rect.w + 10, stage.y-45, 40, 40};
    stop_button.first_color = {229,240,249};
    stop_button.second_color = {179, 210, 253};
    stop_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};

Button Sounds_button{};
void Init_sound_button(){
    Sounds_button.rect = {310, 55, 80, 40};
    Sounds_button.first_color = {217,227,242};
    Sounds_button.second_color = white;
    Sounds_button.third_color = {122,177,255};
    Sounds_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
}
Button Backdrop_button{};
void Init_Backdrop_button(){
    Backdrop_button.rect = {200, 55, 110, 40};
    Backdrop_button.first_color = {217,227,242};
    Backdrop_button.second_color = white;
    Backdrop_button.third_color = {122,177,255};
    Backdrop_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
}
Button Back_button{};
void Init_Back_button(){
    Back_button.rect = {80, 55, 120, 40};
    Back_button.first_color = {217,227,242};
    Back_button.second_color = white;
    Back_button.third_color = {122,177,255};
    Back_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
}
Button code_button{};
void Init_code_button(){
    code_button.rect = {0, 55, 80, 40};
    code_button.first_color = {217,227,242};
    code_button.second_color = white;
    code_button.third_color = {122,177,255};
    code_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
}
Button show_button{};
void Init_show_button(){
    show_button.rect = {stage.x+50,stage.y+stage.h+80,20,20};
    show_button.first_color = white;
    show_button.second_color = {122, 175, 255};
    show_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button hide_button{};
void Init_hide_button(){
    hide_button.rect = {stage.x+70,stage.y+stage.h+80,20,20};
    hide_button.first_color = white;
    hide_button.second_color = {122, 175, 255};
    hide_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button run_sound_button{};
void Init_run_sound_button(){
    run_sound_button.rect = {380,Get_height()/2-200,200,50};
    run_sound_button.first_color = {222,145,222};
    run_sound_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button volumeUp_button{};
void Init_volumeUp_button(){
    volumeUp_button.rect = {380,Get_height()/2-140,200,50};
    volumeUp_button.first_color = {222,145,222};
    volumeUp_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button volumeDown_button{};
void Init_volumeDown_button(){
    volumeDown_button.rect = {380,Get_height()/2-80,200,50};
    volumeDown_button.first_color = {222,145,222};
    volumeDown_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button increase_frequency_button{};
void Init_increase_frequency_button(){
    increase_frequency_button.rect = {380,Get_height()/2-20,200,50};
    increase_frequency_button.first_color = {222,145,222};
    increase_frequency_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button decrease_frequency_button{};
void Init_decrease_frequency_button(){
    decrease_frequency_button.rect = {380,Get_height()/2 + 40,200,50};
    decrease_frequency_button.first_color = {222,145,222};
    decrease_frequency_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button Timer_button{};
void Init_timer_button(){
    Timer_button.rect = {310,Get_height() - 120,100,30};
    Timer_button.first_color = {249,249,249};
    Timer_button.second_color = {122,175,255};
    Timer_button.third_color = {122,175,255};
    Timer_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button next_costume_button{};
void Init_next_costume_button(){
    next_costume_button.rect = {410,Get_height() - 120,120,30};
    next_costume_button.first_color = {249,249,249};
    next_costume_button.second_color = {122,175,255};
    next_costume_button.third_color = {122,175,255};
    next_costume_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button size_button{};
void Init_size_button(){
    size_button.rect = {530,Get_height() - 120,100,30};
    size_button.first_color = {249,249,249};
    size_button.second_color = {122,175,255};
    size_button.third_color = {122,175,255};
    size_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button costume_number_button{};
void Init_costume_number_button(){
    costume_number_button.rect = {630,Get_height() - 120,150,30};
    costume_number_button.first_color = {249,249,249};
    costume_number_button.second_color = {122,175,255};
    costume_number_button.third_color = {122,175,255};
    costume_number_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button volume_button{};
void Init_volume_button(){
    volume_button.rect = {300,Get_height() - 320,100,30};
    volume_button.first_color = {249,249,249};
    volume_button.second_color = {122,175,255};
    volume_button.third_color = {122,175,255};
    volume_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
Button frequency_button{};
void Init_frequency_button(){
    frequency_button.rect = {600,Get_height() - 320,100,30};
    frequency_button.first_color = {249,249,249};
    frequency_button.second_color = {122,175,255};
    frequency_button.third_color = {122,175,255};
    frequency_button.is_mouse_on = Is_mouse_on(Top_button.rect.x,Top_button.rect.y,Top_button.rect.w,Top_button.rect.h);
};
InformationOfCharacter name_of_sprite;
void Init_sprite_box(Character &sprite){
    name_of_sprite.rect ={stage.x+50,stage.y+stage.h+30,120,30};
    name_of_sprite.text = sprite.name;
}

InformationOfCharacter positionX;
void Init_positionX_box(Character &sprite){
    positionX.rect ={stage.x+200,stage.y+stage.h+30,70,30};
    positionX.linkedValue = &sprite.x;
}
InformationOfCharacter positionY;
void Init_positionY_box(Character &sprite){
    positionY.rect ={stage.x+300,stage.y+stage.h+30,70,30};
    positionY.linkedValue = &sprite.y;
}
InformationOfCharacter direction;
void Init_direction_box(Character &sprite){
    direction.rect ={stage.x+305,stage.y+stage.h+75,70,30};
    direction.linkedValue = &sprite.degree;
}
InformationOfCharacter size;
void Init_size_box(Character &sprite){
    size.rect ={stage.x+160,stage.y+stage.h+75,70,30};
    size.linkedValue = &sprite.size;
}

Character cat;
Character* now_sprite = &cat;
void Init_structOfCharacter(){
    cat.name = "cat";
}
Costume cat1;
Costume cat2;
void Init_costume(){
    cat1.name = "costume1";
    cat1.path = "asset/images/sprite/cat.png";
    cat2.name = "costume2";
    cat2.path = "asset/images/sprite/cat_running.png";
}
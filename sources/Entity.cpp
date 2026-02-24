#include "Entity.h"
#include "Graphic_Element.h"
#include "constants.h"
//#include "Asset_Loader.h"


vector<Blocks> active_blocks;
std::vector<Blocks> menu_blocks;
std::map<std::string, BlockTemplate> blockMap;
std::map<std::string, SDL_Texture*> blockLibrary;
std::vector<sprite_button*> active_character;
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
    flag_button.is_mouse_on = Is_mouse_on(flag_button.rect.x,flag_button.rect.y,flag_button.rect.w,flag_button.rect.h);
}
Button stop_button{};
void Init_stop_button(){
    stop_button.rect = {stage.x+flag_button.rect.w + 10, stage.y-45, 40, 40};
    stop_button.first_color = {229,240,249};
    stop_button.second_color = {179, 210, 253};
    stop_button.is_mouse_on = Is_mouse_on(stop_button.rect.x,stop_button.rect.y,stop_button.rect.w,stop_button.rect.h);
};
Button Character_button{};
void Init_Character_button(){
    Character_button.rect = {390, 55, 80, 40};
    Character_button.first_color = {217,227,242};
    Character_button.second_color = white;
    Character_button.third_color = {122,177,255};
    Character_button.is_mouse_on = Is_mouse_on(Character_button.rect.x,Character_button.rect.y,Character_button.rect.w,Character_button.rect.h);
}
Button Sounds_button{};
void Init_sound_button(){
    Sounds_button.rect = {310, 55, 80, 40};
    Sounds_button.first_color = {217,227,242};
    Sounds_button.second_color = white;
    Sounds_button.third_color = {122,177,255};
    Sounds_button.is_mouse_on = Is_mouse_on(Sounds_button.rect.x,Sounds_button.rect.y,Sounds_button.rect.w,Sounds_button.rect.h);
}
Button Backdrop_button{};
void Init_Backdrop_button(){
    Backdrop_button.rect = {200, 55, 110, 40};
    Backdrop_button.first_color = {217,227,242};
    Backdrop_button.second_color = white;
    Backdrop_button.third_color = {122,177,255};
    Backdrop_button.is_mouse_on = Is_mouse_on(Backdrop_button.rect.x,Backdrop_button.rect.y,Backdrop_button.rect.w,Backdrop_button.rect.h);
}
Button Back_button{};
void Init_Back_button(){
    Back_button.rect = {80, 55, 120, 40};
    Back_button.first_color = {217,227,242};
    Back_button.second_color = white;
    Back_button.third_color = {122,177,255};
    Back_button.is_mouse_on = Is_mouse_on(Back_button.rect.x,Back_button.rect.y,Back_button.rect.w,Back_button.rect.h);
}
Button code_button{};
void Init_code_button(){
    code_button.rect = {0, 55, 80, 40};
    code_button.first_color = {217,227,242};
    code_button.second_color = white;
    code_button.third_color = {122,177,255};
    code_button.is_mouse_on = Is_mouse_on(code_button.rect.x,code_button.rect.y,code_button.rect.w,code_button.rect.h);
}
Button show_button{};
void Init_show_button(){
    show_button.rect = {stage.x+50,stage.y+stage.h+80,20,20};
    show_button.first_color = white;
    show_button.second_color = {122, 175, 255};
    show_button.is_mouse_on = Is_mouse_on(show_button.rect.x,show_button.rect.y,show_button.rect.w,show_button.rect.h);
};
Button hide_button{};
void Init_hide_button(){
    hide_button.rect = {stage.x+70,stage.y+stage.h+80,20,20};
    hide_button.first_color = white;
    hide_button.second_color = {122, 175, 255};
    hide_button.is_mouse_on = Is_mouse_on(hide_button.rect.x,hide_button.rect.y,hide_button.rect.w,hide_button.rect.h);
};
Button run_sound_button{};
void Init_run_sound_button(){
    run_sound_button.rect = {380,Get_height()/2-200,200,50};
    run_sound_button.first_color = {222,145,222};
    run_sound_button.is_mouse_on = Is_mouse_on(run_sound_button.rect.x,run_sound_button.rect.y,run_sound_button.rect.w,run_sound_button.rect.h);
};
Button volumeUp_button{};
void Init_volumeUp_button(){
    volumeUp_button.rect = {380,Get_height()/2-140,200,50};
    volumeUp_button.first_color = {222,145,222};
    volumeUp_button.is_mouse_on = Is_mouse_on(volumeUp_button.rect.x,volumeUp_button.rect.y,volumeUp_button.rect.w,volumeUp_button.rect.h);
};
Button volumeDown_button{};
void Init_volumeDown_button(){
    volumeDown_button.rect = {380,Get_height()/2-80,200,50};
    volumeDown_button.first_color = {222,145,222};
    volumeDown_button.is_mouse_on = Is_mouse_on(volumeDown_button.rect.x,volumeDown_button.rect.y,volumeDown_button.rect.w,volumeDown_button.rect.h);
};
Button increase_frequency_button{};
void Init_increase_frequency_button(){
    increase_frequency_button.rect = {380,Get_height()/2-20,200,50};
    increase_frequency_button.first_color = {222,145,222};
    increase_frequency_button.is_mouse_on = Is_mouse_on(increase_frequency_button.rect.x,increase_frequency_button.rect.y,
                                                        increase_frequency_button.rect.w,increase_frequency_button.rect.h);
};
Button decrease_frequency_button{};
void Init_decrease_frequency_button(){
    decrease_frequency_button.rect = {380,Get_height()/2 + 40,200,50};
    decrease_frequency_button.first_color = {222,145,222};
    decrease_frequency_button.is_mouse_on = Is_mouse_on(decrease_frequency_button.rect.x,decrease_frequency_button.rect.y,
                                                        decrease_frequency_button.rect.w,decrease_frequency_button.rect.h);
};
Button Timer_button{};
void Init_timer_button(){
    Timer_button.rect = {310,Get_height() - 120,100,30};
    Timer_button.first_color = {249,249,249};
    Timer_button.second_color = {122,175,255};
    Timer_button.third_color = {122,175,255};
    Timer_button.is_mouse_on = Is_mouse_on(Timer_button.rect.x,Timer_button.rect.y,Timer_button.rect.w,Timer_button.rect.h);
};
Button next_costume_button{};
void Init_next_costume_button(){
    next_costume_button.rect = {410,Get_height() - 120,120,30};
    next_costume_button.first_color = {249,249,249};
    next_costume_button.second_color = {122,175,255};
    next_costume_button.third_color = {122,175,255};
    next_costume_button.is_mouse_on = Is_mouse_on(next_costume_button.rect.x,next_costume_button.rect.y,next_costume_button.rect.w,next_costume_button.rect.h);
};
Button size_button{};
void Init_size_button(){
    size_button.rect = {530,Get_height() - 120,100,30};
    size_button.first_color = {249,249,249};
    size_button.second_color = {122,175,255};
    size_button.third_color = {122,175,255};
    size_button.is_mouse_on = Is_mouse_on(size_button.rect.x,size_button.rect.y,size_button.rect.w,size_button.rect.h);
};
Button costume_number_button{};
void Init_costume_number_button(){
    costume_number_button.rect = {630,Get_height() - 120,150,30};
    costume_number_button.first_color = {249,249,249};
    costume_number_button.second_color = {122,175,255};
    costume_number_button.third_color = {122,175,255};
    costume_number_button.is_mouse_on = Is_mouse_on(costume_number_button.rect.x,costume_number_button.rect.y,costume_number_button.rect.w,costume_number_button.rect.h);
};
Button drag_button{};
void Init_drag_button(){
    drag_button.rect = {780,Get_height() - 120,60,30};
    drag_button.first_color = {249,249,249};
    drag_button.second_color = {122,175,255};
    drag_button.third_color = {122,175,255};
    drag_button.is_mouse_on = Is_mouse_on(costume_number_button.rect.x,costume_number_button.rect.y,costume_number_button.rect.w,costume_number_button.rect.h);
};
Button go_to_front_layer_button{};
void Init_go_to_front_layer_button(){
    go_to_front_layer_button.rect = {840,Get_height() - 120,170,30};
    go_to_front_layer_button.first_color = {249,249,249};
    go_to_front_layer_button.second_color = {122,175,255};
    go_to_front_layer_button.third_color = {122,175,255};
    go_to_front_layer_button.is_mouse_on = Is_mouse_on(costume_number_button.rect.x,costume_number_button.rect.y,costume_number_button.rect.w,costume_number_button.rect.h);
};
Button volume_button{};
void Init_volume_button(){
    volume_button.rect = {300,Get_height() - 320,100,30};
    volume_button.first_color = {249,249,249};
    volume_button.second_color = {122,175,255};
    volume_button.third_color = {122,175,255};
    volume_button.is_mouse_on = Is_mouse_on(volume_button.rect.x,volume_button.rect.y,volume_button.rect.w,volume_button.rect.h);
};
Button frequency_button{};
void Init_frequency_button(){
    frequency_button.rect = {600,Get_height() - 320,100,30};
    frequency_button.first_color = {249,249,249};
    frequency_button.second_color = {122,175,255};
    frequency_button.third_color = {122,175,255};
    frequency_button.is_mouse_on = Is_mouse_on(frequency_button.rect.x,frequency_button.rect.y,frequency_button.rect.w,frequency_button.rect.h);
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
Button cat_button{};
void Init_cat_button(){
    cat_button.rect = {60,120,150,150};
    cat_button.first_color = {249,249,249};
    cat_button.is_mouse_on = Is_mouse_on(cat_button.rect.x,cat_button.rect.y,cat_button.rect.w,cat_button.rect.h);
};
Button bear_button{};
void Init_bear_button(){
    bear_button.rect = {220,120,150,150};
    bear_button.first_color = {249,249,249};
    bear_button.is_mouse_on = Is_mouse_on(bear_button.rect.x,bear_button.rect.y,bear_button.rect.w,bear_button.rect.h);
};
Button fish_button{};
void Init_fish_button(){
    fish_button.rect = {390,120,150,150};
    fish_button.first_color = {249,249,249};
    fish_button.is_mouse_on = Is_mouse_on(fish_button.rect.x,fish_button.rect.y,fish_button.rect.w,fish_button.rect.h);
};
Button balloon_button{};
void Init_balloon_button(){
    balloon_button.rect = {560,120,150,150};
    balloon_button.first_color = {249,249,249};
    balloon_button.is_mouse_on = Is_mouse_on(balloon_button.rect.x,balloon_button.rect.y,balloon_button.rect.w,balloon_button.rect.h);
};
Button apple_button{};
void Init_apple_button(){
    apple_button.rect = {730,120,150,150};
    apple_button.first_color = {249,249,249};
    apple_button.is_mouse_on = Is_mouse_on(apple_button.rect.x,apple_button.rect.y,apple_button.rect.w,apple_button.rect.h);
};
Button dog_button{};
void Init_dog_button(){
    dog_button.rect = {60,320,150,150};
    dog_button.first_color = {249,249,249};
    dog_button.is_mouse_on = Is_mouse_on(dog_button.rect.x,dog_button.rect.y,dog_button.rect.w,dog_button.rect.h);
};

Button emoji_button{};
void Init_emoji_button(){
    emoji_button.rect = {220,320,150,150};
    emoji_button.first_color = {249,249,249};
    emoji_button.is_mouse_on = Is_mouse_on(cat_button.rect.x,cat_button.rect.y,cat_button.rect.w,cat_button.rect.h);
};
Character cat;
Character bear;
Character fish;
Character balloon;
Character red_apple;
Character dog;
Character emoji;
Character* now_sprite = &cat;

void choose_now_sprite(){
    if(active_character.size() ==1 ) now_sprite = active_character.front()->sprite;
    else{
        for(int i=0;i<active_character.size();i++){
            if(active_character[i]->is_now_sprite) now_sprite = active_character[i]->sprite;
            return;
        }
    }
}
void Init_structOfCharacter(){
    cat.name = "cat";
    dog.name = "Dog";
    bear.name = "Bear";
    fish.name = "Fish";
    balloon.name = "Balloon";
    red_apple.name = "Apple";
    emoji.name = "Emoji";
}
Costume cat1;
Costume cat2;
Costume fish1;
Costume fish2;
Costume fish3;
Costume bear1;
Costume bear2;
Costume dog1;
Costume dog2;
Costume apple;
Costume balloon1;
Costume balloon2;
Costume balloon3;
Costume emoji1;
Costume emoji2;
Costume emoji3;
Costume emoji4;

void Init_costume(){
    cat1.name = "costume1";
    cat1.path = "asset/images/sprite/cat.png";
    cat2.name = "costume2";
    cat2.path = "asset/images/sprite/cat_running.png";
    balloon1.name = "balloon_a";
    balloon1.path = "asset/images/sprite/balloon1.png";
    balloon2.name = "balloon_b";
    balloon2.path = "asset/images/sprite/balloon2.png";
    balloon3.name = "balloon_c";
    balloon3.path = "asset/images/sprite/balloon3.png";
    fish1.name = "fish_a";
    fish1.path = "asset/images/sprite/fish1.png";
    fish2.name = "fish_b";
    fish2.path = "asset/images/sprite/fish2.png";
    fish3.name = "fish_c";
    fish3.path = "asset/images/sprite/fish3.png";
    bear1.name = "bear_a";
    bear1.path = "asset/images/sprite/bear1.png";
    bear2.name = "bear_b";
    bear2.path = "asset/images/sprite/bear2.png";
    dog1.name = "dog_a";
    dog1.path = "asset/images/sprite/dog1.png";
    dog2.name = "dog_b";
    dog2.path = "asset/images/sprite/dog2.png";
    apple.name = "apple";
    apple.path = "asset/images/sprite/apple.png";
    emoji1.name = "Emoji1";
    emoji1.path = "asset/images/sprite/emoji1.png";
    emoji2.name = "Emoji2";
    emoji2.path = "asset/images/sprite/emoji2.png";
    emoji3.name = "Emoji3";
    emoji3.path = "asset/images/sprite/emoji3.png";
    emoji4.name = "Emoji4";
    emoji4.path = "asset/images/sprite/emoji4.png";
}
sprite_button cat_buttonUnderstage;
sprite_button dog_buttonUnderstage;
sprite_button apple_buttonUnderstage;
sprite_button balloon_buttonUnderstage;
sprite_button bear_buttonUnderstage;
sprite_button fish_buttonUnderstage;
sprite_button emoji_buttonUnderstage;
void Init_sprite_buttonUnderStage(){
    cat_buttonUnderstage.sprite = &cat;
    balloon_buttonUnderstage.sprite = &balloon;
    dog_buttonUnderstage.sprite = &dog;
    bear_buttonUnderstage.sprite = &bear;
    fish_buttonUnderstage.sprite = &fish;
    apple_buttonUnderstage.sprite = &red_apple;
    emoji_buttonUnderstage.sprite = &emoji;
}
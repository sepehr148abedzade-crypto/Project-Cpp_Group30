#include "looks_functions.h"
#include "constants.h"
#include "TextureManager.h"
#include "Graphic_Element.h"
#include "Entity.h"

void change_size_by(SDL_Renderer *renderer,double size,Character* sprite){
    if(Limit_CharacterX(*sprite) && Limit_CharacterY(*sprite)) {
        sprite->size += size / 1000;
        Load_Character(renderer,sprite,sprite->costumes[sprite->currentCostumeIndex]);
    }
}

void set_size_to(SDL_Renderer* renderer,double size,Character* sprite){
    static double last_size = sprite->size;
    static double next_size = (size/100) * sprite->size;
    sprite->size = next_size;
    Load_Character(renderer,sprite,sprite->costumes[sprite->currentCostumeIndex]);
    if(!Limit_CharacterX(*sprite) || !Limit_CharacterY(*sprite)){
        sprite->size=last_size;
        Load_Character(renderer,sprite,sprite->costumes[sprite->currentCostumeIndex]);
        while (Limit_CharacterX(*sprite) && Limit_CharacterY(*sprite)) {
            sprite->size += 0.01;
            Load_Character(renderer,sprite,sprite->costumes[sprite->currentCostumeIndex]);
        }
    }
}

void show_character(Character* sprite){
    sprite->isvisible = true;
}

void hide_character(Character* sprite){
    sprite->isvisible = false;
}

void say_a_for_t_seconds(std::string message,Character* sprite){
    sprite->monologue = message;
}

void say_a(std::string message,Character* sprite){
    sprite->monologue = message;
}

void think_a_for_t_second(std::string message, Character* sprite){
    sprite->think = message;

}

void switch_costume_to(SDL_Renderer* renderer,double n,Character* sprite){
    if(n<=1) {
        sprite->currentCostumeIndex = 0;
        return;
    }
    if(n>sprite->costumes.size()){
        sprite->currentCostumeIndex = sprite->costumes.size()-1;
        return;
    }
    sprite->currentCostumeIndex = n-1;
}
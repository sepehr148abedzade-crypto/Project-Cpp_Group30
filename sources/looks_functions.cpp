#include "looks_functions.h"
#include "constants.h"
#include "TextureManager.h"
#include "Entity.h"

void change_size_by(SDL_Renderer *renderer,double size,Character &sprite){
    if(Limit_CharacterX(sprite) && Limit_CharacterY(sprite)) {
        sprite.size += size / 1000;
        Load_Character(renderer,sprite.name, sprite, sprite.path);
    }
}

void set_size_to(SDL_Renderer* renderer,double size,Character &sprite){
    static double last_size = sprite.size;
    static double next_size = (size/100) * sprite.size;
    sprite.size = next_size;
    Load_Character(renderer,sprite.name,sprite,sprite.path);
    if(!Limit_CharacterX(sprite) || !Limit_CharacterY(sprite)){
        sprite.size=last_size;
        Load_Character(renderer,sprite.name,sprite,sprite.path);
        while (Limit_CharacterX(sprite) && Limit_CharacterY(sprite)) {
            sprite.size += 0.01;
            Load_Character(renderer,sprite.name, sprite, sprite.path);
        }
    }
}

void show_character(Character &sprite){
    sprite.isvisible = true;
}

void hide_character(Character &sprite){
    sprite.isvisible = false;
}
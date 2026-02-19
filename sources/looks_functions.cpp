#include "looks_functions.h"
#include "constants.h"
#include "TextureManager.h"
#include "Graphic_Element.h"
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

void say_a_for_t_seconds(SDL_Renderer* renderer,TTF_Font* font,std::string message,double time,Character &sprite){
    static Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = SDL_GetTicks();
    if(currentTime-startTime <= 1000 * time) {
        sprite.monologue = message;
        Draw_talking_box(renderer, font, sprite);
    }
}

void say_a(SDL_Renderer* renderer,TTF_Font* font,std::string message,Character &sprite){
    sprite.monologue = message;
    Draw_talking_box(renderer,font,now_sprite);
}

void think_a_for_t_second(SDL_Renderer* renderer,TTF_Font* font,std::string message,double time, Character &sprite){
    static Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = SDL_GetTicks();
    if(currentTime-startTime <= 1000 * time) {
        sprite.think = message;
        Draw_thinking_box(renderer, font, sprite);
    }
}

void think_a(SDL_Renderer* renderer,TTF_Font* font,std::string message,Character &sprite){
    sprite.think = message;
    Draw_thinking_box(renderer,font,now_sprite);
}

void switch_costume_to(SDL_Renderer* renderer,double n,Character &sprite){
    sprite.texture = sprite.costumes[n-1];
}
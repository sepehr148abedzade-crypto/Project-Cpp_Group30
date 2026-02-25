#include "sensing_functions.h"
#include "constants.h"
#include "Entity.h"
#include "Graphic_Element.h"
#include "event_system.h"
#include "SDL2/SDL.h"
#include "cmath"

bool draggable = true;
bool is_timer = false;
bool touching_mouse_pointer(SDL_Event &e,Character &sprite){
    double stage_centerX = stage.x + (stage.w/2);
    double stage_centerY = stage.y + (stage.h/2);
    if(Is_mouse_on((int)(sprite.x+stage_centerX-(sprite.width/2)),(int)(sprite.y+stage_centerY-(sprite.height/2)),
                   (int)sprite.width,(int)sprite.height)) {
        return true;
    }
    return false;
}

bool is_key_space_pressed(SDL_Event &e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_SPACE){
            return true;
        }
    }
    return false;
}

bool is_key_up_arrow_pressed(SDL_Event &e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_UP){
            return true;
        }
    }
    return false;
}

bool is_key_down_arrow_pressed(SDL_Event &e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_DOWN){
            return true;
        }
    }
    return false;
}

bool is_key_right_arrow_pressed(SDL_Event &e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_RIGHT){
            return true;
        }
    }
    return false;
}

bool is_key_left_arrow_pressed(SDL_Event &e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_LEFT){
            return true;
        }
    }
    return false;
}

bool is_key_1_pressed(SDL_Event &e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_1){
            return true;
        }
    }
    return false;
}

bool is_mouse_down(SDL_Event &e,Character &sprite){
    if(e.type == SDL_MOUSEBUTTONDOWN) {
        double stage_centerX = stage.x + (stage.w / 2);
        double stage_centerY = stage.y + (stage.h / 2);
        if (Is_mouse_on((int) (sprite.x + stage_centerX - (sprite.width / 2)),
                        (int) (sprite.y + stage_centerY - (sprite.height / 2)),
                        (int) sprite.width, (int) sprite.height)) {
            return true;
        }
    }
    return false;
}

double distance_to_mouse_pointer(Character &sprite){
    int mouseX,mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    int center_spriteX = (int)sprite.x + stage.x + stage.w/2;
    int center_spriteY = -(int)sprite.y + stage.y + stage.h/2;
    double distance = sqrt(pow(mouseX-center_spriteX,2)+pow(mouseY-center_spriteY,2));
    return distance;
}

void timer(SDL_Renderer* renderer,TTF_Font* font){
    static Uint32 timer_start=0;
    if(flag_active) timer_start=SDL_GetTicks();
    Draw_time_report(renderer, font,SDL_GetTicks()-timer_start);
}
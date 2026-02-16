#include "SDL2/SDL.h"
#include "Entity.h"
#include "cmath"
#include "constants.h"
#include "ctime"
#include "cstdlib"

void move_steps_character(double step,Character &sprite){
    bool stop = false;
    double nextX = sprite.x + cos((sprite.degree*PI)/180) * step;
    double nextY = sprite.y - sin((sprite.degree*PI)/180) * step;
        if (Limit_CharacterX(sprite) && Limit_CharacterY(sprite)) {
            sprite.x = nextX;
            sprite.y = nextY;
        }
    }

void turn_clockwise_character(double angle,Character &sprite){
    sprite.degree += angle;
}

void turn_counterclockwise_character(double angle, Character &sprite){
    sprite.degree += - angle;
}

void go_to_random_position(Character &sprite){
    sprite.x = rand()%(stage.w)-(double)stage.w/2;
    sprite.y = rand()%(stage.h)-(double)stage.h/2 ;
}
#include "SDL2/SDL.h"
#include "Entity.h"
#include "cmath"
#include "constants.h"

void move_steps(double step,Character &sprite){
    bool stop = false;
    double nextX = sprite.x + cos((sprite.degree*PI)/180) * step;
    double nextY = sprite.y - sin((sprite.degree*PI)/180) * step;
        if (Limit_CharacterX(sprite) && Limit_CharacterY(sprite)) {
            sprite.x = nextX;
            sprite.y = nextY;
        }
    }
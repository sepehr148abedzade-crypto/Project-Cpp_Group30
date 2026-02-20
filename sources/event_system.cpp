#include "event_system.h"
#include "Graphic_Element.h"
#include "constants.h"
#include "SDL2/SDL.h"

EventStartType currentEvent = NONE;
bool flag_active = false;
void update_events(SDL_Event &e,Character &sprite){
    if(flag_active) currentEvent = GREEN_FLAG;
    if(e.type == SDL_KEYDOWN) {
        if(e.key.keysym.sym == SDLK_SPACE) currentEvent = SPACE;
    }
    if(e.type == SDL_MOUSEBUTTONDOWN){
        double stage_centerX = stage.x + (stage.w / 2);
        double stage_centerY = stage.y + (stage.h / 2);
        if (Is_mouse_on((int) (sprite.x + stage_centerX - (sprite.width / 2)),
                        (int) (sprite.y + stage_centerY - (sprite.height / 2)),
                        (int) sprite.width, (int) sprite.height))
            currentEvent = SPRITE_CLICK;
    }
}
#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H
#include "SDL2/SDL.h"
struct Character;

enum EventStartType {NONE,GREEN_FLAG,SPACE,SPRITE_CLICK,RECEIVE_MESSAGE};
extern EventStartType currentEvent;
extern bool flag_active;

void update_events(SDL_Event &e,Character &sprite);
#endif //EVENT_SYSTEM_H

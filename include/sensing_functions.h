#ifndef SENSING_FUNCTIONS_H
#define SENSING_FUNCTIONS_H
#include "SDL2/SDL.h"
struct Character;
struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

extern bool draggable;
extern bool is_timer;
bool touching_mouse_pointer(SDL_Event &e,Character &sprite);
bool is_key_space_pressed(SDL_Event &e);
bool is_key_up_arrow_pressed(SDL_Event &e);
bool is_key_down_arrow_pressed(SDL_Event &e);
bool is_key_right_arrow_pressed(SDL_Event &e);
bool is_key_left_arrow_pressed(SDL_Event &e);
bool is_key_1_pressed(SDL_Event &e);
double distance_to_mouse_pointer(Character &sprite);
bool is_mouse_down(SDL_Event &e,Character &sprite);
void timer(SDL_Renderer* renderer,TTF_Font* font);
#endif //SENSING_FUNCTIONS_H

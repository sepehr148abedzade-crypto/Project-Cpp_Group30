#ifndef LOOKS_FUNCTIONS_H
#define LOOKS_FUNCTIONS_H
#include "SDL2/SDL.h"
struct Character;

void change_size_by(SDL_Renderer *renderer,double size,Character &sprite);
void set_size_to(SDL_Renderer* renderer,double size,Character &sprite);
void show_character(Character &sprite);
void hide_character(Character &sprite);
#endif //OOKS_FUNCTIONS_H

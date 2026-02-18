#ifndef LOOKS_FUNCTIONS_H
#define LOOKS_FUNCTIONS_H
#include "SDL2/SDL.h"
#include "string"
struct Character;
struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

void change_size_by(SDL_Renderer *renderer,double size,Character &sprite);
void set_size_to(SDL_Renderer* renderer,double size,Character &sprite);
void show_character(Character &sprite);
void hide_character(Character &sprite);
void say_a_for_t_seconds(SDL_Renderer* renderer,TTF_Font* font,std::string message,double time,Character &sprite);
void say_a(SDL_Renderer* renderer,TTF_Font* font,std::string message,Character &sprite);
#endif //OOKS_FUNCTIONS_H

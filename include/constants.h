#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <SDL2/SDL.h>
#include <vector>
constexpr double PI = 3.1415926;
constexpr double e = 2.78;
struct Character;

int Get_width();
int Get_height();
extern SDL_Rect stage;
extern std::vector<Character> allCharacters;
extern bool isFileMenuOpen;
void Update_Sprite_Dimensions(Character &sprite);
bool Limit_CharacterX(Character &sprite);
bool Limit_CharacterY(Character &sprite);
#endif //CONSTANTS_H

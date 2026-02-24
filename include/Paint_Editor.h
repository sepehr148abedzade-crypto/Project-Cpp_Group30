#ifndef PAINTEDITOR_H
#define PAINTEDITOR_H

#include <SDL2/SDL.h>
#include "Entity.h"
#include "Graphic_Element.h"


extern int lastMouseX;
extern int lastMouseY;
extern int lineStartX, lineStartY;
extern int textClickX, textClickY;
extern int textX, textY;
extern int circleStartX, circleStartY;

extern bool isTypingText;
extern bool isDrawingLine ;
extern bool isDrawingCircle;
extern bool isSaveModalOpen;
extern bool isTyping ;

extern bool isDrawingRect ;



void ApplyPen(SDL_Texture* target, int x, int y, SDL_Renderer* renderer);

[[maybe_unused]] void ApplyEraser(SDL_Texture* target, int x, int y, SDL_Renderer* renderer);
void ApplyFill(SDL_Texture* target, SDL_Renderer* renderer);
void DrawLineOnTexture(SDL_Texture* target, int x1, int y1, int x2, int y2, SDL_Renderer* renderer, bool isEraser);
void DrawCircleOnTexture(SDL_Texture* target, int centerX, int centerY, int radius, SDL_Renderer* renderer, bool fill);
void ClearCurrentDrawingLayer(SDL_Renderer* renderer);
void Draw_Image_Editor(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* currentTex, string itemName);


#endif
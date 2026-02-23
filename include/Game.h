#pragma once
#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <string>
#include "Entity.h"
#include <SDL2/SDL_image.h>

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

extern bool stop ;
extern bool isBackdropMenuOpen;

SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color);
SDL_Texture* ConvertToEditable(SDL_Texture* source, SDL_Renderer* renderer);

int Get_text_width(TTF_Font* font,std ::string text );
bool Loading();
bool Init_Game();
std::string GetUniqueName(std::string baseName);
void AddUploadedBackdrop(SDL_Texture* tex, std::string fileName);
void AddBlock(std::string type, int x, int y);
void DrawALLBlocks(SDL_Renderer* renderer, TTF_Font* font);
void HandleBlockEvent(SDL_Event& e);
void SetRandomBackdrop();
bool IsCircleClicked(int mx, int my, int cx, int cy, int radius);
void AddBackdropToProject(SDL_Texture *tex, std::string name, bool forceSwitch, bool b);
void CheckInputClick(int mx, int my);
void CreateNewPaintBackdrop();
void UploadBackdrop();
void HandleColorSelection(int mx, int my);
void HandleBrushSizeSelection(int mx, int my);
bool IsValidChar(char c, InputType type);
void DrawSaveModal(SDL_Renderer* renderer, TTF_Font* font);
void HandleKeyboardInput(SDL_Event& e);
void ApplyTextToLayer();
void HandleContinuousDrawing(int mx, int my);
void HandleCanvasMouseUp(int mx, int my);
void HandleCanvasMouseDown(int mx, int my);
void Handle_Scroll_Events(int mx, int my, const SDL_Event& e);
void HandleSaveModalClick(int mx, int my);
void Handle_Tab_Switch(int mx, int my);
void Handle_Backdrop_Selection(int mx, int my);
void Handle_Backdrop_Menu_Clicks(int mx, int my);
void SaveToLibrary(string name, SDL_Renderer* renderer);
void Get_event();
void Draw_Stage_Content(SDL_Renderer* renderer);
void Update();
void Render();
void Clean();
void SDL_wait_key();
void Executing_Motion_Blocks(Blocks& block,Character& sprite );

#endif //GAME_H

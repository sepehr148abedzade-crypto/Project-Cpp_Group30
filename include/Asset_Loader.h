#ifndef PROJECT_CPP_GROUP30_ASSET_LOADER_H
#define PROJECT_CPP_GROUP30_ASSET_LOADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <string>
#include "Entity.h"

extern std::map<std::string, SDL_Texture*> blockLibrary;
extern std::map<std::string, BlockTemplate> blockMap;
bool LoadAllAssets(SDL_Renderer* renderer);
SDL_Texture* GetBlockTexture(std::string name);
void AddBlock(const string& id, int x, int y, SDL_Texture* tex);
void Init_Menu_Blocks();
void CleanUpAssets();
void CreateTestBlock();

#endif //PROJECT_CPP_GROUP30_ASSET_LOADER_H

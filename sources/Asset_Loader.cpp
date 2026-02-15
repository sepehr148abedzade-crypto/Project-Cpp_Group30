#include "Asset_Loader.h"
using namespace std;

map<string, SDL_Texture*>blockLibrary;
map<std::string, BlockTemplate> blockMap;

bool LoadAllAssets(SDL_Renderer* renderer){
    blockMap["move"] = {160, 45, {{72, NUMBER, "10"}}};
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");

    blockMap["turn_left"] = {160, 45, {{80, NUMBER, "0"}}};
    blockLibrary["turn_left"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_left.png");

    blockMap["turn_right"] = {160, 45, {{80, NUMBER, "0"}}};
    blockLibrary["turn_right"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_right.png");

    blockMap["change_x"] = {160, 45,{{130, NUMBER, "0"}}};
    blockLibrary["change_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_x.png");

    blockMap["change_y"] = {160, 45, {{130, NUMBER, "0"}}};
    blockLibrary["change_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_y.png");

    blockMap["go_to_x_y"] = {160, 45, {{77, NUMBER, "0"}, {135, NUMBER, "0"}}};
    blockLibrary["go_to_x_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/go_to_x_y.png");

    blockMap["set_x"] = {92, 45,{{70, NUMBER, "0"}}};
    blockLibrary["set_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_x.png");

    blockMap["set_y"] = {92, 45,{{70, NUMBER, "0"}}};
    blockLibrary["set_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_y.png");

    return true;
}


SDL_Texture* GetBlockTexture(std::string name) {
    if (blockLibrary.count(name)) {
        return blockLibrary[name];
    }
    return nullptr;
}


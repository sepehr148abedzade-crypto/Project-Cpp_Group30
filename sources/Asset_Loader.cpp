#include "Asset_Loader.h"
using namespace std;

map<string, SDL_Texture*>blockLibrary;

bool LoadAllAssets(SDL_Renderer* renderer){
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");
    blockLibrary["turn_left"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_left.png");
    blockLibrary["turn_right"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_right.png");
    blockLibrary["change_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_x.png");
    blockLibrary["change_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_y.png");
    blockLibrary["go_to_x_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/go_to_x_y.png");
    blockLibrary["set_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_x.png");
    blockLibrary["set_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_y.png");
}


SDL_Texture* GetBlockTexture(std::string name) {
    if (blockLibrary.count(name)) {
        return blockLibrary[name];
    }
    return nullptr;
}


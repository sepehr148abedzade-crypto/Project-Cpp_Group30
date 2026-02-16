#include "Asset_Loader.h"
using namespace std;


void AddBlock(const string& id, int x, int y, SDL_Texture* tex) {
    Blocks newBlock;
    newBlock.id = id;
    newBlock.rect = {x, y, blockMap[id].width, blockMap[id].height};
    newBlock.image = tex;
    newBlock.is_editing = false;
    newBlock.active_value_index = -1;

    if (blockMap.count(id)) {
        for (auto & input : blockMap[id].inputs) {
            newBlock.values.push_back(input.defaultValue);
        }
    }
    active_blocks.push_back(newBlock);
}

void Init_Menu_Blocks() {
    int startY = 100;
    int spacing = 60;
    string motion_ids[] = {"move", "turn_left", "turn_right", "go_to_x_y", "change_x", "change_y", "set_x", "set_y"};
    for (int i = 0; i < 8; i++) {
        Blocks b;
        b.id = motion_ids[i];
        b.image = blockLibrary[b.id];
        b.rect.w = blockMap[b.id].width;
        b.rect.h = blockMap[b.id].height;
        b.rect.x = 80;
        b.rect.y = startY + (i * spacing);
        menu_blocks.push_back(b);
    }
}

bool LoadAllAssets(SDL_Renderer* renderer){
    blockMap["move"] = {160, 45, {{72, NUMBER, "10"}}};
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");

    blockMap["turn_left"] = {160, 45, {{80, NUMBER, "0"}}};
    blockLibrary["turn_left"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_left.png");

    blockMap["turn_right"] = {160, 45, {{80, NUMBER, "0"}}};
    blockLibrary["turn_right"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_right.png");

    blockMap["change_x"] = {145, 45,{{115, NUMBER, "0"}}};
    blockLibrary["change_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_x.png");

    blockMap["change_y"] = {145, 45, {{115, NUMBER, "0"}}};
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


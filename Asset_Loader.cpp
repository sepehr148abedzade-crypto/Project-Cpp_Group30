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
    menu_blocks.clear();
    int startY = 100;
    int spacing = 60;
    string motion_ids[] = {"move", "turn_left", "turn_right", "go_to_x_y", "change_x", "change_y", "set_x", "set_y"};
    for (int i = 0; i < 8; i++) {
        Blocks b;
        b.id = motion_ids[i];
        b.image = nullptr;
        b.type =blockMap[b.id].type;
        b.rect.w = blockMap[b.id].width;
        b.rect.h = blockMap[b.id].height;
        b.rect.x = 80;
        b.rect.y = startY + (i * spacing);
        menu_blocks.push_back(b);
        startY += spacing;
    }
}

bool LoadAllAssets(SDL_Renderer* renderer){
    blockMap["move"] = {160, 45, Simple_Block,"move",{"steps"},{{72,INPUT_NUMBER,"10"}}};

    blockMap["turn_left"] = {160, 45,Simple_Block,"turn left" ,{"degrees"},{{80, INPUT_NUMBER, "15"}}};

    blockMap["turn_right"] = {160, 45,Simple_Block,"tern right",{"degrees"}, {{80, INPUT_NUMBER, "0"}}};

    blockMap["change_x"] = {145, 45,Simple_Block,"change x by ",{},{{115, INPUT_NUMBER, "0"}}};

    blockMap["change_y"] = {145, 45,Simple_Block,"change y by ",{}, {{115, INPUT_NUMBER, "0"}}};

    blockMap["go_to_x_y"] = {160, 45,Simple_Block,"go to x:",{"y:"}, {{77, INPUT_NUMBER, "0"}, {135, INPUT_NUMBER, "0"}}};

    blockMap["set_x"] = {92, 45,Simple_Block,"set x to ",{},{{70, INPUT_NUMBER, "0"}}};

    blockMap["set_y"] = {92, 45,Simple_Block,"set y to ",{},{{70, INPUT_NUMBER, "0"}}};


    return true;
}


SDL_Texture* GetBlockTexture(std::string name) {
    if (blockLibrary.count(name)) {
        return blockLibrary[name];
    }
    return nullptr;
}


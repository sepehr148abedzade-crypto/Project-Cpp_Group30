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
    int currentY = 100;
    int spacing = 60;
    for (auto& catInfo : categoryOrder) {
        if (!catInfo.active) continue;
        if (catInfo.startY!=0) {
            currentY = catInfo.startY;
            }
            for (auto& pair:blockMap) {
                if (pair.second.category == catInfo.category) {
                    Blocks b;
                    b.id = pair.first;
                    b.image = nullptr;
                    b.type =blockMap[b.id].type;
                    b.rect.w = blockMap[b.id].width;
                    b.rect.h = blockMap[b.id].height;
                    b.rect.x = 80;
                    b.rect.y = currentY;
                    menu_blocks.push_back(b);
                    currentY += spacing;
            }
        }
    }

}

bool LoadAllAssets(SDL_Renderer* renderer){
    blockMap["move"] = {160, 45, Simple_Block,CAT_MOTION,"move",{"steps"},{{72,INPUT_NUMBER,"10"}}};

    blockMap["turn_left"] = {160, 45,Simple_Block,CAT_MOTION,"turn left" ,{"degrees"},{{80, INPUT_NUMBER, "15"}}};

    blockMap["turn_right"] = {160, 45,Simple_Block,CAT_MOTION,"tern right",{"degrees"}, {{80, INPUT_NUMBER, "0"}}};

    blockMap["change_x"] = {145, 45,Simple_Block,CAT_MOTION,"change x by ",{},{{115, INPUT_NUMBER, "0"}}};

    blockMap["change_y"] = {145, 45,Simple_Block,CAT_MOTION,"change y by ",{}, {{115, INPUT_NUMBER, "0"}}};

    blockMap["go_to_x_y"] = {160, 45,Simple_Block,CAT_MOTION,"go to x:",{"y:"}, {{77, INPUT_NUMBER, "0"}, {135, INPUT_NUMBER, "0"}}};

    blockMap["set_x"] = {92, 45,Simple_Block,CAT_MOTION,"set x to ",{},{{70, INPUT_NUMBER, "0"}}};

    blockMap["set_y"] = {92, 45,Simple_Block,CAT_MOTION,"set y to ",{},{{70, INPUT_NUMBER, "0"}}};

    blockMap["say"] = {165,45,Simple_Block,CAT_LOOK,"say",{},{{80,INPUT_TEXT,"2"}}};

    blockMap["say_seconds"]= {200,45,Simple_Block,CAT_LOOK,"say",{"for","saconds"},{{70,INPUT_TEXT,"hello!"},{140,INPUT_NUMBER,"2"}}};

    blockMap["think"] = {160,45,Simple_Block,CAT_LOOK,"think",{},{{80,INPUT_TEXT,"Hmm..."}}};

    blockMap["think_seconds"] = {200 ,45,Simple_Block,CAT_LOOK,"think",{"for","saconds"},{{70,INPUT_TEXT,"Hmm..."},{140,INPUT_NUMBER,"2"}}};

    blockMap["show"]={120,45,Simple_Block,CAT_LOOK,"show",{},{}};

    blockMap["hide"]={120,45,Simple_Block,CAT_LOOK,"hide",{},{}};

    blockMap["switch_costume"]={160 , 45 ,Simple_Block,CAT_LOOK,"Switch custom to",{},{{100,INPUT_DROPDOWN,"costume1"}}};

    blockMap ["next_costume"] = {140 , 45 , Simple_Block,CAT_LOOK,"next costume",{},{}};

    blockMap["change_size"]={160,45,Simple_Block,CAT_LOOK,"change size by",{},{{100,INPUT_NUMBER,"10"}}};

    blockMap["set_size"] = {140, 45 ,Simple_Block,CAT_LOOK,"set size to",{"%"},{{100,INPUT_NUMBER,"100"}}};

    blockMap ["change_effect"] = {180, 45 , Simple_Block,CAT_LOOK,"change",{"effect by"},{{70,INPUT_DROPDOWN,"color"},{140 , INPUT_NUMBER,"25"}}};

    return true;
}


SDL_Texture* GetBlockTexture(std::string name) {
    if (blockLibrary.count(name)) {
        return blockLibrary[name];
    }
    return nullptr;
}


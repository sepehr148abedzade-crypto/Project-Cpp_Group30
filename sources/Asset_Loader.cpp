#include "Asset_Loader.h"
#include "Graphic_Element.h"
#include <SDL2/SDL_image.h>
#include <iostream>
using namespace std;

extern SDL_Renderer* renderer;
extern SDL_Texture* icon_gallery;
extern SDL_Texture* icon_paint;
extern SDL_Texture* icon_upload;
extern SDL_Texture* icon_surprise;
extern SDL_Texture* currentBackdropTexture;
extern void AddBackdropToProject(SDL_Texture *tex, std::string name, bool forceSwitch, bool b);

string OpenFileDialog() {
    char fileName[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg;*.bmp\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "png";
    if (GetOpenFileNameA(&ofn)) {
        return string(fileName);
    }
    return "";
}

void LoadBackdropLibraryManual(SDL_Renderer* renderer) {
    libraryItems.clear();
    std::vector<std::string> fileNames = {"Castle 2.png", "Castle 3.png", "Basketball 3.png", "Bench With View.png"};

    for (const std::string& name : fileNames) {
        std::string fullPath = "asset/images/" + name;
        SDL_Texture* tex = IMG_LoadTexture(renderer, fullPath.c_str());
        if (tex) {
            libraryItems.push_back({tex, name});
        }
    }

    if (!libraryItems.empty() && projectBackdrops.empty()) {
        AddBackdropToProject(libraryItems[0].texture, libraryItems[0].name, false, false);    }
}

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
    icon_gallery = IMG_LoadTexture(renderer, "asset/images/icon_gallery.png");
    icon_paint = IMG_LoadTexture(renderer, "asset/images/icon_paint.png");
    icon_upload = IMG_LoadTexture(renderer, "asset/images/icon_upload.png");
    icon_surprise = IMG_LoadTexture(renderer, "asset/images/icon_surprise.png");

    blockMap["move"] = {160, 45, Simple_Block,CAT_MOTION,"move",{"steps"},{{72,INPUT_NUMBER,"10"}}};
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");

    blockMap["turn_left"] = {160, 45,Simple_Block,CAT_MOTION,"turn left" ,{"degrees"},{{80, INPUT_NUMBER, "15"}}};
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");

    blockMap["turn_right"] = {160, 45,Simple_Block,CAT_MOTION,"tern right",{"degrees"}, {{80, INPUT_NUMBER, "0"}}};
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");

    blockMap["change_x"] = {145, 45,Simple_Block,CAT_MOTION,"change x by ",{},{{115, INPUT_NUMBER, "0"}}};
    blockLibrary["change_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_x.png");

    blockMap["change_y"] = {145, 45,Simple_Block,CAT_MOTION,"change y by ",{}, {{115, INPUT_NUMBER, "0"}}};
    blockLibrary["change_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_y.png");

    blockMap["go_to_x_y"] = {160, 45,Simple_Block,CAT_MOTION,"go to x:",{"y:"}, {{77, INPUT_NUMBER, "0"}, {135, INPUT_NUMBER, "0"}}};
    blockLibrary["go_to_x_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/go_to_x_y.png");

    blockMap["set_x"] = {92, 45,Simple_Block,CAT_MOTION,"set x to ",{},{{70, INPUT_NUMBER, "0"}}};
    blockLibrary["set_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_x.png");

    blockMap["set_y"] = {92, 45,Simple_Block,CAT_MOTION,"set y to ",{},{{70, INPUT_NUMBER, "0"}}};
    blockLibrary["set_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_y.png");

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

    /*blockMap["go_to_random_position"] = {153, 40,{{64, DROPDOWN, ""}}};

    blockMap["point_in_direction"] = {127, 40,{{107, NUMBER, "0"}}};

    blockMap["point_towards"] = {193, 40,{{64, DROPDOWN, ""}}};

    blockMap["say_for_seconds"] = {168, 40,{{48, TEXT, ""}, {101, TEXT, ""}}};

    blockMap["say"] = {68, 40,{{47, TEXT, ""}}};

    blockMap["think_for_seconds"] = {168, 40,{{48, TEXT, ""}, {101, TEXT, ""}}};

    blockMap["think"] = {68, 40,{{50, TEXT, ""}}};

    blockMap["switch_costume_to"] = {184, 40,{{50, DROPDOWN, ""}}};

    blockMap["switch_costume"] = {78, 40,{{50, DROPDOWN, ""}}};

    blockMap["next_costume"] = {78, 40,{{50, DROPDOWN, ""}}};
    */
    return true;
}


SDL_Texture* GetBlockTexture(std::string name) {
    if (blockLibrary.count(name)) {
        return blockLibrary[name];
    }
    return nullptr;
}


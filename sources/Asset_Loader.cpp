#include "Asset_Loader.h"
#include "Graphic_Element.h"
#include <SDL2/SDL_image.h>
using namespace std;

extern SDL_Renderer* renderer;
extern SDL_Texture* icon_gallery;
extern SDL_Texture* icon_paint;
extern SDL_Texture* icon_upload;
extern SDL_Texture* icon_surprise;




void LoadBackdropLibraryManual(SDL_Renderer* renderer) {
    std::vector<std::string> fileNames = {"Castle 2.png", "Castle 3.png"};
    for (const std::string& name : fileNames) {
        std::string fullPath = "asset/images/" + name;
        SDL_Texture* tex = IMG_LoadTexture(renderer, fullPath.c_str());
        if (tex) {
            libraryItems.push_back({tex, name});
        }
    }
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
    int startY = 100;
    int spacing = 60;
    vector<string> motion_ids = {"move", "turn_left", "turn_right", "go_to_x_y", "change_x", "change_y",
                                 "set_x", "set_y", "go_to_random_position", "point_in_direction", "point_towards",
                                 "say_for_seconds", "say", "think_for_seconds", "think", "switch_costume_to",
                                 "switch_costume","next_costume"};
    for (int i = 0; i < motion_ids.size(); i++) {
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

    icon_gallery = IMG_LoadTexture(renderer, "asset/images/icon_gallery.png");
    icon_paint = IMG_LoadTexture(renderer, "asset/images/icon_paint.png");
    icon_upload = IMG_LoadTexture(renderer, "asset/images/icon_upload.png");
    icon_surprise = IMG_LoadTexture(renderer, "asset/images/icon_surprise.png");

    blockMap["move"] = {105, 40, {{55, NUMBER, "10"}}};
    blockLibrary["move"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/move.png");

    blockMap["turn_left"] = {133, 40, {{65, NUMBER, "0"}}};
    blockLibrary["turn_left"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_left.png");

    blockMap["turn_right"] = {133, 40, {{65, NUMBER, "0"}}};
    blockLibrary["turn_right"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/turn_right.png");

    blockMap["change_x"] = {106, 40,{{86, NUMBER, "0"}}};
    blockLibrary["change_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_x.png");

    blockMap["change_y"] = {106, 40, {{86, NUMBER, "0"}}};
    blockLibrary["change_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/change_y.png");

    blockMap["go_to_x_y"] = {130, 40, {{64, NUMBER, "0"}, {111, NUMBER, "0"}}};
    blockLibrary["go_to_x_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/go_to_x_y.png");

    blockMap["set_x"] = {82, 40,{{64, NUMBER, "0"}}};
    blockLibrary["set_x"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_x.png");

    blockMap["set_y"] = {82, 40,{{64, NUMBER, "0"}}};
    blockLibrary["set_y"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/set_y.png");

    blockMap["go_to_random_position"] = {153, 40,{{64, DROPDOWN, ""}}};
    blockLibrary["go_to_random_position"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/go_to_random_position.png");

    blockMap["point_in_direction"] = {127, 40,{{107, NUMBER, "0"}}};
    blockLibrary["point_in_direction"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/point_in_direction.png");

    blockMap["point_towards"] = {193, 40,{{64, DROPDOWN, ""}}};
    blockLibrary["point_towards"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Motion/point_towards.png");

    blockMap["say_for_seconds"] = {168, 40,{{48, TEXT, ""}, {101, TEXT, ""}}};
    blockLibrary["say_for_seconds"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/say_for_seconds.png");

    blockMap["say"] = {68, 40,{{47, TEXT, ""}}};
    blockLibrary["say"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/say.png");

    blockMap["think_for_seconds"] = {168, 40,{{48, TEXT, ""}, {101, TEXT, ""}}};
    blockLibrary["think_for_seconds"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/think_for_seconds.png");

    blockMap["think"] = {68, 40,{{50, TEXT, ""}}};
    blockLibrary["think"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/think.png");

    blockMap["switch_costume_to"] = {184, 40,{{50, DROPDOWN, ""}}};
    blockLibrary["switch_costume_to"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/switch_costume_to.png");

    blockMap["switch_costume"] = {78, 40,{{50, DROPDOWN, ""}}};
    blockLibrary["switch_costume"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/switch_costume.png");

    blockMap["next_costume"] = {78, 40,{{50, DROPDOWN, ""}}};
    blockLibrary["next_costume"] = IMG_LoadTexture(renderer, "asset/images/Blocks/Looks/next_costume.png");



    return true;
}


SDL_Texture* GetBlockTexture(std::string name) {
    if (blockLibrary.count(name)) {
        return blockLibrary[name];
    }
    return nullptr;
}


#include "Game.h"
#include "iomanip"
#include <windows.h>
#include "TextureManager.h"
#include "iostream"
#include "SDL2/SDL_ttf.h"
#include "Graphic_Element.h"
#include "motion_functions.h"
#include "sound_functions.h"
#include "sensing_functions.h"
#include "looks_functions.h"
#include "constants.h"
#include "Entity.h"
#include <Asset_Loader.h>
#include <vector>
#include <map>
#include <cmath>
#include "Paint_Editor.h"
#include "SDL2/SDL_mixer.h"
#include "Paint_Editor.h"
#include <algorithm>
using namespace std;

SDL_Window* main_window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* Scratch_logo = nullptr;
SDL_Texture* File_Text = nullptr;
SDL_Texture* Sound_text = nullptr;
SDL_Texture* Character_text = nullptr;
SDL_Texture* Backdrop_text = nullptr;
SDL_Texture* Back_text = nullptr;
SDL_Texture* code_text = nullptr;
SDL_Texture* green_flag = nullptr;
SDL_Texture* stop_sign = nullptr;
SDL_Texture* X_text = nullptr;
SDL_Texture* Y_text = nullptr;
SDL_Texture* size_text = nullptr;
SDL_Texture* sprite_text = nullptr;
SDL_Texture* direction_text = nullptr;
SDL_Texture* positionX_text = nullptr;
SDL_Texture* positionY_text = nullptr;
SDL_Texture* size_of_sprite_text = nullptr;
SDL_Texture* name_of_sprite_text = nullptr;
SDL_Texture* direction_of_sprite_text = nullptr;
SDL_Texture* show_text = nullptr;
SDL_Texture* S_text = nullptr;
SDL_Texture* H_text = nullptr;
SDL_Texture* Run_text = nullptr;
SDL_Texture* volumeUp_text = nullptr;
SDL_Texture* volumeDown_text = nullptr;
SDL_Texture* increase_frequency_text = nullptr;
SDL_Texture* decrease_frequency_text = nullptr;
SDL_Texture* Timer_text = nullptr;
SDL_Texture* size_button_text = nullptr;
SDL_Texture* next_costume_text = nullptr;
SDL_Texture* costume_number_text = nullptr;
SDL_Texture* drag_text = nullptr;
SDL_Texture* go_to_front_layer_text = nullptr;
SDL_Texture* volume_text = nullptr;
SDL_Texture* volume_value = nullptr;
SDL_Texture* frequency_text = nullptr;
SDL_Texture* frequency_value = nullptr;
SDL_Texture* cat_text = nullptr;
SDL_Texture* dog_text = nullptr;
SDL_Texture* fish_text = nullptr;
SDL_Texture* bear_text = nullptr;
SDL_Texture* balloon_text = nullptr;
SDL_Texture* apple_text = nullptr;
SDL_Texture* emoji_text = nullptr;
TTF_Font* loading_font = nullptr;
TTF_Font* main_font = nullptr;
TTF_Font* report_font = nullptr;
TTF_Font* talking_font = nullptr;
TTF_Font* thinking_font = nullptr;
TTF_Font* edit_font = nullptr;
TTF_Font* code_bar_font = nullptr;
SDL_Texture* currentBackdropTexture = nullptr;
Character* currentSprite = nullptr;
SDL_Texture* globalDrawingLayer = nullptr;

std::vector<Character> allCharacters;
vector<vector<Blocks>> blockChains;
 int draggedChainIndex= -1;
int executingChainIndex = -1;
int executingBlockIndex = -1;
Uint32 executionStartTime = 0;
bool isExecuting = false;

Uint32 clickStartTime = 0;
bool isClickAndHold = false;
const Uint32 CLICK_THRESHOLD = 200;

int lastMouseX = -1;
int lastMouseY = -1;
int lineStartX = -1, lineStartY = -1;
int textClickX = -1, textClickY = -1;
int textX = 0, textY = 0;
int circleStartX = -1, circleStartY = -1;
string saveInputText;
string textToDraw;
string textInput;

std::string GetUniqueBackdropName(std::string baseName) {
    int counter = 1;
    std::string finalName = baseName;
    bool exists = true;

    while (exists) {
        exists = false;
        for (size_t i = 0; i < projectBackdrops.size(); i++) {
            if (projectBackdrops[i].name == finalName) {
                exists = true;
                break;
            }
        }
        if (exists) {
            finalName = baseName + std::to_string(++counter);
        }
    }
    return finalName;
}
HitResult CheckBlockHit(int mx, int my, int chainIdx, int blockIdx) {
    HitResult result = {-1, -1, false, -1, false, -1};
    Blocks& block = blockChains[chainIdx][blockIdx];

    for (size_t i = 0; i < block.childBlocks.size(); i++) {
        Blocks* child = block.childBlocks[i];
        if (mx >= child->rect.x && mx <= child->rect.x + child->rect.w &&
            my >= child->rect.y && my <= child->rect.y + child->rect.h) {

            for (int c = 0; c < blockChains.size(); c++) {
                for (int b = 0; b < blockChains[c].size(); b++) {
                    if (&blockChains[c][b] == child) {
                        result.chainIndex = c;
                        result.blockIndex = b;
                        result.isOnChildBlock = true;
                        return result;
                    }
                }
            }
        }
    }

    int current_x = block.rect.x + 5;
    current_x += Get_text_width(code_bar_font, blockMap[block.id].Back_label) + 5;

    for (size_t i = 0; i < block.inputs.size(); i++) {
        int input_width = 40;

        SDL_Rect input_rect = {
                current_x,
                block.rect.y + 9,
                input_width - 3,
                20
        };

        if (mx >= input_rect.x && mx <= input_rect.x + input_rect.w &&
            my >= input_rect.y && my <= input_rect.y + input_rect.h) {
            result.chainIndex = chainIdx;
            result.blockIndex = blockIdx;
            result.isOnInput = true;
            result.inputIndex = i;
            return result;
        }

        current_x += input_width + 5;
        if (i < blockMap[block.id].labels.size()) {
            current_x += Get_text_width(code_bar_font, blockMap[block.id].labels[i]) + 5;
        }
    }

    return result;
}
void UpdateParentWidth(int chainIndex, int blockIndex) {
    if (chainIndex == -1 || blockIndex == -1) return;

    Blocks& block = blockChains[chainIndex][blockIndex];
    BlockTemplate& bt = blockMap[block.id];

    int newWidth = calculatingWidthBlock(bt, block.inputs, code_bar_font, chainIndex, blockIndex);
    block.rect.w = newWidth;

    if (block.parentChainIndex != -1 && block.parentBlockIndex != -1) {
        UpdateParentWidth(block.parentChainIndex, block.parentBlockIndex);
    }
}
void PlaceBlockInInput(int targetChain, int targetBlock, int targetInput,
                       int sourceChain, int sourceBlock) {
    Blocks& target = blockChains[targetChain][targetBlock];
    Blocks& source = blockChains[sourceChain][sourceBlock];

    int oldParentChain = source.parentChainIndex;
    int oldParentBlock = source.parentBlockIndex;
    int oldParentInput = source.parentInputIndex;

    if (oldParentChain != -1) {
        Blocks& oldParent = blockChains[oldParentChain][oldParentBlock];

        auto it = find(oldParent.childBlocks.begin(), oldParent.childBlocks.end(), &source);
        if (it != oldParent.childBlocks.end()) {
            oldParent.childBlocks.erase(it);
        }

        if (oldParentInput >= 0 && oldParentInput < oldParent.inputs.size()) {
            oldParent.inputs[oldParentInput].type = InputValue::DIRECT_VALUE;
            oldParent.inputs[oldParentInput].directValue = "";
        }

        UpdateParentWidth(oldParentChain, oldParentBlock);
    }

    source.parentChainIndex = targetChain;
    source.parentBlockIndex = targetBlock;
    source.parentInputIndex = targetInput;

    target.childBlocks.push_back(&source);
    target.inputs[targetInput].type = InputValue::BLOCK_REFERENCE;
    target.inputs[targetInput].refChainIndex = sourceChain;
    target.inputs[targetInput].refBlockIndex = sourceBlock;

    source.needsEvaluation = true;
    EvaluateBlock(source);

    UpdateAllChildPositions(targetChain, targetBlock);
    UpdateParentWidth(targetChain, targetBlock);
}

void UpdateAllChildPositions(int parentChain, int parentBlock) {
    Blocks& parent = blockChains[parentChain][parentBlock];

    int current_x = parent.rect.x + 5;
    current_x += Get_text_width(code_bar_font, blockMap[parent.id].Back_label) + 5;

    for (size_t i = 0; i < parent.inputs.size(); i++) {

        if (parent.inputs[i].type == InputValue::BLOCK_REFERENCE) {
            if (parent.inputs[i].refChainIndex >= 0) {
                Blocks& childBlock = blockChains[parent.inputs[i].refChainIndex][parent.inputs[i].refBlockIndex];

                childBlock.rect.x = current_x;
                childBlock.rect.y = parent.rect.y + 3;

                if (!childBlock.childBlocks.empty()) {
                    UpdateAllChildPositions(parent.inputs[i].refChainIndex, parent.inputs[i].refBlockIndex);
                }

                current_x += childBlock.rect.w + 5;
            }
        } else {
            int inputWidth = max(40, Get_text_width(code_bar_font, parent.inputs[i].directValue) + 10);
            current_x += inputWidth + 5;
        }

        if (i < blockMap[parent.id].labels.size()) {
            current_x += Get_text_width(code_bar_font, blockMap[parent.id].labels[i]) + 5;
        }
    }
}
void DrawBlockOutput(SDL_Renderer* renderer, TTF_Font* font, Blocks& block) {
    if (!block.isDisplayingOutput || block.output.empty()) return;

    string displayText = block.output;
    if (displayText.length() > 10) {
        displayText = displayText.substr(0, 8) + "...";
    }
    int textW, textH;
    TTF_SizeText(font, displayText.c_str(), &textW, &textH);
    int padding = 8;
    int boxW = textW + padding * 2;
    int boxH = textH + padding * 2;

    SDL_Rect box = {
            block.rect.x + (block.rect.w - boxW) / 2,
            block.rect.y + block.rect.h + 5,
            boxW,
            boxH
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &box);

    int triangleX = block.rect.x + block.rect.w / 2;
    int triangleY = block.rect.y + block.rect.h;

    SDL_Texture* textTex = LoadText(renderer, font, displayText, {0, 0, 0, 255});
    if (textTex) {
        SDL_Rect textRect = {
                box.x + padding,
                box.y + padding,
                textW,
                textH
        };
        SDL_RenderCopy(renderer, textTex, nullptr, &textRect);
        SDL_DestroyTexture(textTex);
    }
}
void Executing_Motion_Blocks(Blocks& block, Character* sprite) {
    string ID = block.id;

    if (ID == "move") {
        if (block.inputs.size() > 0) {
            double steps = GetInputAsNumber(block.inputs[0],blockChains);
            move_steps_character(steps, sprite);
        }
    }
    else if (ID == "turn_left") {
        if (block.inputs.size() > 0) {
            double angel = GetInputAsNumber(block.inputs[0],blockChains);
            turn_clockwise_character(angel, sprite);
        }
    }
    else if (ID == "turn_right") {
        if (block.inputs.size() > 0) {
            double angel = GetInputAsNumber(block.inputs[0],blockChains);
            turn_clockwise_character(angel, sprite);
        }
    }
    else if (ID == "change_x") {
        if (block.inputs.size() > 0) {
            double new_x = GetInputAsNumber(block.inputs[0],blockChains);
            change_x_by(new_x, sprite);
        }
    }
    else if (ID == "change_y") {
        if (block.inputs.size() > 0) {
            double new_y = GetInputAsNumber(block.inputs[0],blockChains);
            change_y_by(new_y, sprite);
        }
    }
    else if (ID == "go_to_x_y") {
        if (block.inputs.size() > 1) {
            double new_x = GetInputAsNumber(block.inputs[0],blockChains);
            double new_y = GetInputAsNumber(block.inputs[1],blockChains);
            go_to_x_y(new_x, new_y, sprite);
        }
    }
    else if (ID == "set_x") {
        if (block.inputs.size() > 0) {
            double new_x = GetInputAsNumber(block.inputs[0],blockChains);
            set_x_to(new_x, sprite);
        }
    }
    else if (ID == "set_y") {
        if (block.inputs.size() > 0) {
            double new_y = GetInputAsNumber(block.inputs[0],blockChains);
            set_y_to(new_y, sprite);
        }
    }
}

void ExecuteLooksBlock(Blocks& block, Character* sprite) {
    string id = block.id;

    if (id == "say") {
        if (block.inputs.size() > 0) {
            string msg = GetInputAsString(block.inputs[0],blockChains);
            say_a(msg, sprite);
            Draw_talking_box(renderer, report_font, sprite);
        }
    }
    else if (id == "say_seconds") {
        if (block.inputs.size() > 0) {
            string msg = GetInputAsString(block.inputs[0],blockChains);
            say_a_for_t_seconds(msg, sprite);
        }
    }
    else if (id == "think") {
        if (block.inputs.size() > 0) {
            string msg = GetInputAsString(block.inputs[0],blockChains);
            think_a_for_t_second(msg, sprite);
        }
    }
    else if (id == "show") {
        show_character(sprite);
    }
    else if (id == "hide") {
        hide_character(sprite);
    }
    else if (id == "change_size") {
        if (block.inputs.size() > 0) {
            double size = GetInputAsNumber(block.inputs[0],blockChains);
            change_size_by(renderer, size, sprite);
        }
    }
    else if (id == "set_size") {
        if (block.inputs.size() > 0) {
            double size = GetInputAsNumber(block.inputs[0],blockChains);
            set_size_to(renderer, size, sprite);
        }
    }
    else if (id == "next_costume") {
        switch_costume_to(renderer, sprite);
    }
}
void ExecuteSoundBlock(Blocks& block, Character* sprite) {
    string id = block.id;

    if (id == "play_sound") {
        if (block.inputs.size() > 0) {
            string soundName = GetInputAsString(block.inputs[0], blockChains);
            play_sound();
        }
    }
    else if (id == "start_sound") {
        if (block.inputs.size() > 0) {
            string soundName = GetInputAsString(block.inputs[0], blockChains);
            play_sound();
        }
    }
    else if (id == "stop_all") {
        Mix_HaltChannel(-1);
    }
    else if (id == "change_volume") {
        if (block.inputs.size() > 0) {
            double vol = GetInputAsNumber(block.inputs[0], blockChains);
            change_volume_by((int)vol);
        }
    }
    else if (id == "set_volume") {
        if (block.inputs.size() > 0) {
            double vol = GetInputAsNumber(block.inputs[0], blockChains);
            set_volume_to((int)vol);
        }
    }
}
string EvaluateExpression(Blocks& block) {
    string id = block.id;
    string result = "";

    if (id == "add") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);
        result = to_string(a + b);
    }
    else if (id == "subtract") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);
        result = to_string(a - b);
    }
    else if (id == "multiply") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);
        result = to_string(a * b);
    }
    else if (id == "divide") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);
        if (b != 0) result = to_string(a / b);
        else result = "Infinity";
    }
    else if (id == "pick_random") {
        int a = (int)GetInputAsNumber(block.inputs[0], blockChains);
        int b = (int)GetInputAsNumber(block.inputs[1], blockChains);
        int r = a + rand() % (b - a + 1);
        result = to_string(r);
    }
    else if (id == "round") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        result = to_string((int)round(a));
    }
    else if (id == "mod") {
        int a = (int)GetInputAsNumber(block.inputs[0], blockChains);
        int b = (int)GetInputAsNumber(block.inputs[1], blockChains);
        if (b != 0) result = to_string(a % b);
        else result = "0";
    }
    else if (id == "join") {
        string a = GetInputAsString(block.inputs[0], blockChains);
        string b = GetInputAsString(block.inputs[1], blockChains);
        result = a + b;
    }
    else if (id == "letter_of") {
        int n = (int)GetInputAsNumber(block.inputs[0], blockChains) - 1;
        string str = GetInputAsString(block.inputs[1], blockChains);
        if (n >= 0 && n < str.length()) {
            result = string(1, str[n]);
        } else {
            result = "";
        }
    }
    else if (id == "length") {
        string str = GetInputAsString(block.inputs[0], blockChains);
        result = to_string(str.length());
    }
    else if (id == "function") {
        string func = "abs";
        if (block.inputs.size() > 0) {
            vector<string> options = GetDropdownOptions(DROPDOWN_FUNCTION);
            int idx = block.inputs[0].selectedOptionIndex;
            if (idx >= 0 && idx < options.size()) {
                func = options[idx];
            }
        }

        double num = 0;
        if (block.inputs.size() > 1) {
            num = GetInputAsNumber(block.inputs[1], blockChains);
        }

        if (func == "abs") result = to_string(abs(num));
        else if (func == "floor") result = to_string(floor(num));
        else if (func == "ceil") result = to_string(ceil(num));
        else if (func == "sqrt") {
            if (num >= 0) result = to_string(sqrt(num));
            else result = "NaN";
        }
        else if (func == "sin") result = to_string(sin(num * M_PI / 180.0));
        else if (func == "cos") result = to_string(cos(num * M_PI / 180.0));
        else if (func == "tan") {
            double cosVal = cos(num * M_PI / 180.0);
            if (abs(cosVal) > 0.0001)
                result = to_string(tan(num * M_PI / 180.0));
            else result = "Infinity";
        }
        else if (func == "log") {
            if (num > 0) result = to_string(log10(num));
            else result = "NaN";
        }
        else if (func == "ln") {
            if (num > 0) result = to_string(log(num));
            else result = "NaN";
        }
        else if (func == "e^") result = to_string(exp(num));
        else if (func == "10^") result = to_string(pow(10, num));
        else result = to_string(num);
    }

    if (result.find('.') != string::npos) {
        result.erase(result.find_last_not_of('0') + 1, string::npos);
        if (result.back() == '.') result.pop_back();
    }

    return result;
}
bool EvaluateBoolean(Blocks& block) {
    string id = block.id;
    bool result = false;

    if (id == "greater_than") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);
        result = (a > b);
    }
    else if (id == "less_than") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);
        result = (a < b);
    }
    else if (id == "equal") {
        double a = GetInputAsNumber(block.inputs[0], blockChains);
        double b = GetInputAsNumber(block.inputs[1], blockChains);

        result = (abs(a - b) < 0.0001);
    }
    else if (id == "and") {
        bool a = (GetInputAsString(block.inputs[0], blockChains) == "true" ||
                  GetInputAsNumber(block.inputs[0], blockChains) != 0);
        bool b = (GetInputAsString(block.inputs[1], blockChains) == "true" ||
                  GetInputAsNumber(block.inputs[1], blockChains) != 0);
        result = (a && b);
    }
    else if (id == "or") {
        bool a = (GetInputAsString(block.inputs[0], blockChains) == "true" ||
                  GetInputAsNumber(block.inputs[0], blockChains) != 0);
        bool b = (GetInputAsString(block.inputs[1], blockChains) == "true" ||
                  GetInputAsNumber(block.inputs[1], blockChains) != 0);
        result = (a || b);
    }
    else if (id == "not") {
        bool a = (GetInputAsString(block.inputs[0], blockChains) == "true" ||
                  GetInputAsNumber(block.inputs[0], blockChains) != 0);
        result = !a;
    }
    else if (id == "contains") {
        string str = GetInputAsString(block.inputs[1], blockChains);
        string sub = GetInputAsString(block.inputs[0], blockChains);
        result = (str.find(sub) != string::npos);
    }
    else if (id == "touching") {
        result = false;
    }

    return result;
}
void EvaluateBlock(Blocks& block) {
    Block_category cat = blockMap[block.id].category;

    if (cat == CAT_OPERATORS) {
        if (block.type == Expression_Block) {
            block.output = EvaluateExpression(block);
            block.hasOutput = true;
            block.needsEvaluation = false;
        }
        else if (block.type == Bool_Block) {
            block.outputBool = EvaluateBoolean(block);
            block.output = block.outputBool ? "1" : "0";
            block.hasOutput = true;
            block.needsEvaluation = false;
        }
    }
    else if (cat == CAT_SENSING) {
        if (block.id == "touching") {
            block.outputBool = false;
            block.output = "0";
            block.hasOutput = true;
            block.needsEvaluation = false;
        }
    }
}
void ExecuteBlock(Blocks& block, Character* sprite) {
    for (Blocks* child : block.childBlocks) {
        if (child->needsEvaluation) {
            EvaluateBlock(*child);
        }
    }
    EvaluateBlock(block);
    if (block.hasOutput) {
        block.isDisplayingOutput = true;
        block.outputDisplayTime = SDL_GetTicks();
    }
    Block_category cat = blockMap[block.id].category;

    switch (cat) {
        case CAT_MOTION:
            Executing_Motion_Blocks(block, sprite);
            break;
        case CAT_LOOK:
            ExecuteLooksBlock(block, sprite);
            break;
        case CAT_SOUND:
            ExecuteSoundBlock(block, sprite);
            break;
        default:
            break;
    }
}
void UpdateOutputDisplay() {
    Uint32 currentTime = SDL_GetTicks();

    for (auto& chain : blockChains) {
        for (auto& block : chain) {
            if (block.isDisplayingOutput &&
                currentTime - block.outputDisplayTime > 3000) {
                block.isDisplayingOutput = false;
            }
        }
    }
}
void UpdateExecution() {
    if (!isExecuting) return;
    if (executingChainIndex < 0 || executingChainIndex >= blockChains.size()) {
        isExecuting = false;
        return;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - executionStartTime > 500) {
        executingBlockIndex++;

        if (executingBlockIndex >= blockChains[executingChainIndex].size()) {
            isExecuting = false;
            executingChainIndex = -1;
            executingBlockIndex = -1;
        } else {
            Blocks& block = blockChains[executingChainIndex][executingBlockIndex];
            ExecuteBlock(block,now_sprite);
            executionStartTime = currentTime;
        }
    }
}

bool FindClickedBlock(int mx, int my, int& chainIdx, int& blockIdx) {
    for (int c = 0; c < blockChains.size(); c++) {
        for (int b = 0; b < blockChains[c].size(); b++) {
            SDL_Rect rect = blockChains[c][b].rect;
            if (mx >= rect.x && mx <= rect.x + rect.w &&
                my >= rect.y && my <= rect.y + rect.h) {
                chainIdx = c;
                blockIdx = b;
                return true;
                }
        }
    }
    return false;
}
void SplitChain(int chainIdx, int blockIdx) {
    if (chainIdx < 0 || chainIdx >= blockChains.size()) return;
    if (blockIdx <= 0 || blockIdx >= blockChains[chainIdx].size()) return;

    vector<Blocks> firstPart, secondPart;
    for (int i = 0; i < blockIdx; i++) {
        firstPart.push_back(blockChains[chainIdx][i]);
    }
    for (int i = blockIdx; i < blockChains[chainIdx].size(); i++) {
        secondPart.push_back(blockChains[chainIdx][i]);
    }
    blockChains.erase(blockChains.begin() + chainIdx);
    if (!firstPart.empty()) {
        blockChains.push_back(firstPart);
    }
    if (!secondPart.empty()) {
        blockChains.push_back(secondPart);
    }
}
void MergeChains(int targetIdx, int sourceIdx, bool toTop) {
    if (targetIdx < 0 || targetIdx >= blockChains.size()) return;
    if (sourceIdx < 0 || sourceIdx >= blockChains.size()) return;
    if (targetIdx == sourceIdx) return;

    if (toTop) {
        blockChains[targetIdx].insert(
            blockChains[targetIdx].begin(),
            blockChains[sourceIdx].begin(),
            blockChains[sourceIdx].end()
        );
    } else {
        blockChains[targetIdx].insert(
            blockChains[targetIdx].end(),
            blockChains[sourceIdx].begin(),
            blockChains[sourceIdx].end()
        );
    }

    blockChains.erase(blockChains.begin() + sourceIdx);
}
bool IsNearForSnap(Blocks& block1, Blocks& block2) {
    int verticalGap1 = block2.rect.y - (block1.rect.y + block1.rect.h);
    bool horizontallyAligned1 = (abs(block1.rect.x - block2.rect.x) < 10);
    int verticalGap2 = block1.rect.y - (block2.rect.y + block2.rect.h);
    bool horizontallyAligned2 = (abs(block1.rect.x - block2.rect.x) < 10);

    return (horizontallyAligned1 && abs(verticalGap1) < 10) ||
           (horizontallyAligned2 && abs(verticalGap2) < 10);
}
void SnapBlocks(Blocks& upperBlock, Blocks& lowerBlock) {

    lowerBlock.rect.x = upperBlock.rect.x;
    lowerBlock.rect.y = upperBlock.rect.y + upperBlock.rect.h - 5;

    upperBlock.next = &lowerBlock;
    lowerBlock.prev = &upperBlock;
}

void SnapBlocksToTop(Blocks& newTopBlock, Blocks& existingChain) {

    newTopBlock.rect.x = existingChain.rect.x;
    newTopBlock.rect.y = existingChain.rect.y - newTopBlock.rect.h + 5;

    newTopBlock.next = &existingChain;
    existingChain.prev = &newTopBlock;
}
void ExecuteChain(int chainIndex) {
    if (chainIndex < 0 || chainIndex >= blockChains.size()) return;

    executingChainIndex = chainIndex;
    executingBlockIndex = 0;
    executionStartTime = SDL_GetTicks();
    isExecuting = true;
    if (!blockChains[executingChainIndex].empty()) {
        Blocks& block = blockChains[executingChainIndex][0];
        ExecuteBlock(block,now_sprite);
    }
}

SDL_Texture* LoadText(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color){
    if(!font) {
        std::cout << "font is not find! SDL_Error : " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Surface* text_surface = TTF_RenderText_Blended(font,text.c_str(),color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,text_surface);
    SDL_FreeSurface(text_surface);
    return texture;
}

int Get_text_width(TTF_Font* font,string text ) {
    int width ;
    TTF_SizeText(font,text.c_str(),&width,nullptr);
    return width;
}

bool Loading(){
    if(TTF_Init()==-1){
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    loading_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",50);
    if(loading_font== nullptr){
        std::cout << "Loading Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL could not initialize! SDL_Error : " << SDL_GetError() << std::endl;
        return false;
    }
    main_window= SDL_CreateWindow(
            "Scratch",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            Get_width(),
            Get_width(),
            SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED |SDL_WINDOW_RESIZABLE
    );

    if(main_window == nullptr) {
        std::cout << "main_window could not be created! SDL_Error : " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(main_window,-1,SDL_RENDERER_ACCELERATED);

    if(renderer == nullptr) {
        std::cout << "renderer could not be created! SDL_Error : " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_Texture* Loading_text = LoadText(renderer,loading_font,"Scratch is loading...",white);

    SDL_SetRenderDrawColor(renderer,77,151,255,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    Init_Load_button();
    Draw_loading_window(renderer,Load_button,Loading_text);
    SDL_RenderPresent(renderer);
    //SDL_Delay(3000);
}

bool Init_Game(){
    IMG_Init(IMG_INIT_PNG);
    SDL_SetRenderDrawColor(renderer, 229, 240, 255, 255);
    SDL_RenderClear(renderer);

    if(TTF_Init()==-1){
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    main_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",15);
    edit_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",40);
    if(main_font== nullptr){
        std::cout << "Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    Init_Button();
    report_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",10);
    if(report_font== nullptr){
        std::cout << "Report Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    talking_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",20);
    if(talking_font== nullptr){
        std::cout << "Talking Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    thinking_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf",20);
    if(thinking_font== nullptr){
        std::cout << "Thinking Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    File_Text = LoadText(renderer,main_font,"File",white);
    Scratch_logo = LoadTexture(renderer,"asset/images/logo/scratch.png");
    File_Text = LoadText(renderer,main_font,"File",white);
    Character_text = LoadText(renderer,main_font,"Character",{120,147,149});
    Sound_text = LoadText(renderer,main_font,"Sounds",{120,147,149});
    Backdrop_text = LoadText(renderer,main_font,"Backdrop",{120,147,149});
    Back_text = LoadText(renderer,main_font,"Back",{120,147,149});
    code_text = LoadText(renderer,main_font,"Code",{120,147,149});
    Timer_text = LoadText(renderer,main_font,"Timer",{100,100,100});
    size_button_text = LoadText(renderer,main_font,"Size",{100,100,100});
    volume_value = LoadText(renderer,main_font,to_string(volume),{100,100,100});
    volume_text = LoadText(renderer,main_font,"Volume : ",{100,100,100});
    frequency_value = LoadText(renderer,main_font,to_string(frequency),{100,100,100});
    frequency_text = LoadText(renderer,main_font,"Frequency : ",{100,100,100});
    next_costume_text = LoadText(renderer,main_font,"next costume",{100,100,100});
    costume_number_text = LoadText(renderer,main_font,"costume number",{100,100,100});
    drag_text = LoadText(renderer,main_font,"Drag",{100,100,100});
    go_to_front_layer_text = LoadText(renderer,main_font,"Go_to_front_layer",{100,100,100});
    Scratch_logo = LoadTexture(renderer,"asset/images/logo/scratch.png");
    SetWindowIcon(main_window,"asset/images/logo/icon.png");
    green_flag = LoadTexture(renderer,"asset/images/logo/flag.png");
    stop_sign = LoadTexture(renderer,"asset/images/logo/stop.png");
    X_text = LoadText(renderer,report_font,"X",{100,100,100});
    Y_text = LoadText(renderer,report_font,"Y",{100,100,100});
    size_text = LoadText(renderer,report_font,"Size",{100,100,100});
    direction_text = LoadText(renderer,report_font,"Direction",{100,100,100});
    show_text = LoadText(renderer,report_font,"Show",{100,100,100});
    sprite_text = LoadText(renderer,report_font,"Sprite",{100,100,100});
    positionX_text = LoadText(renderer,report_font,to_string(now_sprite->x),{100,100,100});
    positionY_text = LoadText(renderer,report_font, to_string(now_sprite->y),{100,100,100});
    size_of_sprite_text = LoadText(renderer,report_font, to_string(now_sprite->size),{100,100,100});
    direction_of_sprite_text = LoadText(renderer,report_font, to_string(now_sprite->degree),{100,100,100});
    name_of_sprite_text = LoadText(renderer,report_font,now_sprite->name,{100,100,100});
    S_text = LoadText(renderer,report_font,"S",{100,100,100});
    H_text = LoadText(renderer,report_font,"H",{100,100,100});
    cat_text = LoadText(renderer,main_font,"Cat",{100,100,100});
    dog_text = LoadText(renderer,main_font,"Dog",{100,100,100});
    bear_text = LoadText(renderer,main_font,"Bear",{100,100,100});
    balloon_text = LoadText(renderer,main_font,"Balloon",{100,100,100});
    apple_text = LoadText(renderer,main_font,"Apple",{100,100,100});
    fish_text = LoadText(renderer,main_font,"Fish",{100,100,100});
    emoji_text = LoadText(renderer,main_font,"Emoji",{100,100,100});
    Run_text = LoadText(renderer,main_font,"Run",white);
    volumeUp_text = LoadText(renderer,main_font,"Volume Up",white);
    volumeDown_text = LoadText(renderer,main_font,"Volume Down",white);
    increase_frequency_text = LoadText(renderer,main_font,"Increase Frequency",white);
    decrease_frequency_text = LoadText(renderer,main_font,"Decrease Frequency",white);    SetWindowIcon(main_window,"asset/images/logo/icon.png");

    code_bar_font = TTF_OpenFont("asset/fonts/Montserrat-Bold.ttf", 10);
    if(code_bar_font == nullptr){
        std::cout << "Code_bar Font could not be found! Error: " << TTF_GetError() << std::endl;
        return false;
    }
    Init_structOfCharacter();
    Init_costume();
    Init_code_button(renderer,code_bar_font);
    LoadAllAssets(renderer);
    Init_Menu_Blocks();
    Init_flag_button();
    Init_stop_button();
    Init_Character_button();
    Init_sound_button();
    Init_Back_button();
    Init_Backdrop_button();
    Init_code_button();
    Init_sprite_box(*now_sprite);
    Init_positionX_box(*now_sprite);
    Init_positionY_box(*now_sprite);
    Init_size_box(*now_sprite);
    Init_direction_box(*now_sprite);
    Init_show_button();
    Init_hide_button();
    Init_run_sound_button();
    Init_volumeUp_button();
    Init_volumeDown_button();
    Init_increase_frequency_button();
    Init_decrease_frequency_button();
    Init_timer_button();
    Init_size_button();
    Init_next_costume_button();
    Init_go_to_front_layer_button();
    Init_drag_button();
    Init_volume_button();
    Init_frequency_button();
    Init_costume_number_button();
    Init_cat_button();
    Init_bear_button();
    Init_fish_button();
    Init_dog_button();
    Init_emoji_button();
    Init_apple_button();
    Init_balloon_button();
    Init_sprite_buttonUnderStage();
    Load_Character(renderer,&cat,&cat1);
    Load_Character(renderer,&cat,&cat2);
    Load_Character(renderer,&dog,&dog1);
    Load_Character(renderer,&dog,&dog2);
    Load_Character(renderer,&bear,&bear1);
    Load_Character(renderer,&bear,&bear2);
    Load_Character(renderer,&red_apple,&apple);
    Load_Character(renderer,&fish,&fish1);
    Load_Character(renderer,&fish,&fish2);
    Load_Character(renderer,&fish,&fish3);
    Load_Character(renderer,&balloon,&balloon1);
    Load_Character(renderer,&balloon,&balloon2);
    Load_Character(renderer,&balloon,&balloon3);
    Load_Character(renderer,&emoji,&emoji1);
    Load_Character(renderer,&emoji,&emoji2);
    Load_Character(renderer,&emoji,&emoji3);
    Load_Character(renderer,&emoji,&emoji4);
    LoadBackdropLibraryManual(renderer);
    SDL_StartTextInput();
    return true;
}

void UpdateBlockWidth(Blocks& block , TTF_Font* font) {
    if (blockMap.count(block.id)) {
        int chainIdx = -1, blockIdx = -1;
        for (int c = 0; c < blockChains.size(); c++) {
            for (int b = 0; b < blockChains[c].size(); b++) {
                if (&blockChains[c][b] == &block) {
                    chainIdx = c;
                    blockIdx = b;
                    break;
                }
            }
            if (chainIdx != -1) break;
        }

        int new_width = calculatingWidthBlock(blockMap[block.id], block.inputs, font, chainIdx, blockIdx);
        block.rect.w = new_width;
    }
}

void AddBackdropToProject(SDL_Texture *tex, string name, bool forceSwitch, bool b) {
    if (!tex || !renderer) return;

    int w, h;
    if (SDL_QueryTexture(tex, NULL, NULL, &w, &h) != 0) return;

    Backdrop newBD;
    newBD.name = name;

    newBD.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!newBD.texture) return;

    SDL_SetTextureBlendMode(newBD.texture, SDL_BLENDMODE_BLEND);

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    if (SDL_SetRenderTarget(renderer, newBD.texture) == 0) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, NULL, NULL);
    }

    newBD.drawingLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (newBD.drawingLayer) {
        SDL_SetTextureBlendMode(newBD.drawingLayer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, newBD.drawingLayer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    SDL_SetRenderTarget(renderer, oldTarget);

    projectBackdrops.push_back(newBD);
    selectedBackdropIndex = (int)projectBackdrops.size() - 1;

    if (forceSwitch) {
        currentTab = BACKDROPS;
        currentBackdropTexture = projectBackdrops[selectedBackdropIndex].texture;
    }
}

void AddUploadedBackdrop(SDL_Texture* tex, std::string fileName) {
    size_t lastDot = fileName.find_last_of(".");
    std::string baseName = (lastDot == std::string::npos) ? fileName : fileName.substr(0, lastDot);

    Backdrop newBD;
    newBD.texture = tex;
    newBD.name = GetUniqueBackdropName(baseName);
    projectBackdrops.push_back(newBD);
}

void CreateNewPaintBackdrop() {
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 780, 520);
    if (tex) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, tex);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);

        AddBackdropToProject(tex, "backdrop" + std::to_string(projectBackdrops.size() + 1), true, false);
        currentTab = BACKDROPS;
    }
}

void SetRandomBackdrop() {
    if (libraryItems.empty()) return;
    int r = rand() % libraryItems.size();
    bool shouldSwitch = (currentTab == BACKDROPS);
    AddBackdropToProject(libraryItems[r].texture, libraryItems[r].name, shouldSwitch, false);
}

void UploadBackdrop() {
    string path = OpenFileDialog();
    if (!path.empty()) {
        SDL_Texture* newTex = IMG_LoadTexture(renderer, path.c_str());
        if (newTex) {
            Backdrop bd;
            bd.texture = newTex;

            size_t lastSlash = path.find_last_of("/\\");
            string fileName = (lastSlash == string::npos) ? path : path.substr(lastSlash + 1);
            size_t lastDot = fileName.find_last_of(".");
            bd.name = (lastDot == string::npos) ? fileName : fileName.substr(0, lastDot);

            int w, h;
            SDL_QueryTexture(newTex, NULL, NULL, &w, &h);

            bd.drawingLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
            SDL_SetTextureBlendMode(bd.drawingLayer, SDL_BLENDMODE_BLEND);

            SDL_SetRenderTarget(renderer, bd.drawingLayer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, NULL);

            projectBackdrops.push_back(bd);
            selectedBackdropIndex = projectBackdrops.size() - 1;
        }
    }
}

bool IsCircleClicked(int mx, int my, int cx, int cy, int r) {
    return ((mx - cx) * (mx - cx) + (my - cy) * (my - cy)) <= (r * r);
}


bool IsValidChar(char c, InputType type) {
    if (type == INPUT_NUMBER) {
        return (c >= '0' && c <= '9') || c == '-';
    }
    if (type ==INPUT_BOOLEAN) {
        return (c >= 32 && c <= 126);
    }
    return false;
}

void ApplyTextToLayer() {
    if (textInput.empty() || selectedBackdropIndex < 0) {
        isTyping = false;
        return;
    }

    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    if (!target) return;

    int tw, th;
    SDL_QueryTexture(target, NULL, NULL, &tw, &th);

    int fx = (textX - 330) * tw / 600;
    int fy = (textY - 280) * th / 380;

    SDL_Surface* surf = TTF_RenderText_Blended(edit_font, textInput.c_str(), globalEditor.currentColor);
    if (!surf) return;

    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dest = { fx, fy, surf->w, surf->h };

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);
    SDL_RenderCopy(renderer, textTex, NULL, &dest);
    SDL_SetRenderTarget(renderer, oldTarget);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(textTex);

    isTyping = false;
    textInput = "";
    SDL_StopTextInput();
}

void HandleKeyboardInput(SDL_Event& e) {
    if (isSaveModalOpen) {
        if (e.type == SDL_TEXTINPUT) {
            saveInputText += e.text.text;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !saveInputText.empty()) {
                saveInputText.pop_back();
            } else if (e.key.keysym.sym == SDLK_RETURN) {
                if (!saveInputText.empty()) SaveToLibrary(saveInputText, renderer);
            } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                isSaveModalOpen = false;
                saveInputText = "";
                SDL_StopTextInput();
            }
        }
        return;
    }

    if (isTyping && currentTab == BACKDROPS) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !textInput.empty()) {
                textInput.pop_back();
            } else if (e.key.keysym.sym == SDLK_RETURN) {
                ApplyTextToLayer();
            }
        } else if (e.type == SDL_TEXTINPUT) {
            textInput += e.text.text;
        }
        return;
    }

    for (auto& chain : blockChains) {
        for (auto& block : chain) {
            if (block.is_editing && block.active_value_index != -1 &&
                block.active_value_index < (int)block.inputs.size()) {

                InputValue& input = block.inputs[block.active_value_index];

                if (input.type != InputValue::DIRECT_VALUE) continue;

                string& str = input.directValue;
                InputType currentType = blockMap[block.id].inputs[block.active_value_index].type;

                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && !str.empty()) {
                        str.pop_back();
                        UpdateBlockWidth(block, code_bar_font);
                    } else if (e.key.keysym.sym == SDLK_RETURN) {
                        int Last_Index = block.active_value_index;
                        block.is_editing = false;
                        block.active_value_index = -1;
                        SDL_StopTextInput();

                        if (str.empty() && Last_Index != -1) {
                            str = blockMap[block.id].inputs[Last_Index].defaultValue;
                            UpdateBlockWidth(block, code_bar_font);
                        }
                    }
                } else if (e.type == SDL_TEXTINPUT) {
                    bool valid_char = false;
                    if (currentType == INPUT_NUMBER) {
                        valid_char = (e.text.text[0] >= '0' && e.text.text[0] <= '9') ||
                                     e.text.text[0] == '-' || e.text.text[0] == '.';
                    } else if (currentType == INPUT_TEXT) {
                        valid_char = (e.text.text[0] >= 32 && e.text.text[0] <= 126);
                    } else {
                        valid_char = true;
                    }

                    if (valid_char && str.length() < 20) {
                        str += e.text.text;
                        UpdateBlockWidth(block, code_bar_font);
                    }
                    break;
                }
            }
        }
    }
}

void HandleLibraryClick(int mx, int my) {
    int xStart = 50, yStart = 100, padding = 20, imgW = 150, imgH = 120;

    for (int i = 0; i < (int)libraryItems.size(); i++) {
        SDL_Rect box = { xStart + (i % 5) * (imgW + padding),
                         yStart + (i / 5) * (imgH + padding),
                         imgW, imgH };

        if (mx >= box.x && mx <= box.x + box.w && my >= box.y && my <= box.y + box.h) {
            AddBackdropToProject(libraryItems[i].texture, libraryItems[i].name, (currentTab == BACKDROPS), false);
            isLibraryOpen = false;
            return;
        }
    }
}

void HandleBlockEvent(SDL_Event& e) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mPos = {mx, my};

    static int draggedChainIndex = -1;
    static int draggedBlockIndex = -1;
    static int offsetX = 0, offsetY = 0;
    static bool isDraggingToInput = false;

    if (e.type == SDL_MOUSEWHEEL) {
        if (mx > 60 && mx < 310) {
            sidebar_scroll_y += e.wheel.y * 25;
            if (sidebar_scroll_y > 0) sidebar_scroll_y = 0;
            if (sidebar_scroll_y < -3000) sidebar_scroll_y = -3000;
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        clickStartTime = SDL_GetTicks();
        isClickAndHold = false;
        isDraggingToInput = false;

        bool dropdownClicked = false;

        for (int c = 0; c < blockChains.size(); c++) {
            for (int b = 0; b < blockChains[c].size(); b++) {
                Blocks& block = blockChains[c][b];
                for (size_t i = 0; i < block.inputs.size(); i++) {
                    if (block.inputs[i].isOpen) {
                        int current_x = block.rect.x + 5;
                        current_x += Get_text_width(code_bar_font, blockMap[block.id].Back_label) + 5;

                        for (size_t k = 0; k < i; k++) {
                            if (blockMap[block.id].inputs[k].type == INPUT_DROPDOWN) {
                                current_x += 80 + 5;
                            } else {
                                string val = InputValueToString(block.inputs[k]);
                                int w = max(40, Get_text_width(code_bar_font, val) + 10);
                                current_x += w + 5;
                            }
                            if (k < blockMap[block.id].labels.size()) {
                                current_x += Get_text_width(code_bar_font, blockMap[block.id].labels[k]) + 5;
                            }
                        }

                        int dropdownX = current_x;
                        int dropdownY = block.rect.y + 29;

                        vector<string> options = GetDropdownOptions(blockMap[block.id].inputs[i].dropdownType);

                        for (size_t optIdx = 0; optIdx < options.size(); optIdx++) {
                            SDL_Rect optRect = {dropdownX, dropdownY + (int)optIdx * 20, 80, 20};

                            if (mx >= optRect.x && mx <= optRect.x + optRect.w &&
                                my >= optRect.y && my <= optRect.y + optRect.h) {


                                block.inputs[i].selectedOptionIndex = optIdx;
                                block.inputs[i].directValue = options[optIdx];
                                block.inputs[i].isOpen = false;  // بستن dropdown

                                UpdateBlockWidth(block, code_bar_font);

                                dropdownClicked = true;
                                break;
                            }
                        }

                        if (dropdownClicked) break;
                    }
                    if (dropdownClicked) break;
                }
                if (dropdownClicked) break;
            }
            if (dropdownClicked) break;
        }

        if (!dropdownClicked) {
            for (auto& chain : blockChains) {
                for (auto& block : chain) {
                    for (auto& input : block.inputs) {
                        input.isOpen = false;
                    }
                }
            }
        }

        bool clickedOnInput = false;
        bool clickedOnMenu = false;

        CheckInputClick(mx, my);
        for (auto& chain : blockChains) {
            for (auto& block : chain) {
                if (block.is_editing) {
                    clickedOnInput = true;
                    break;
                }
            }
            if (clickedOnInput) break;
        }
        if (clickedOnInput) return;

        if (mx > 60 && mx < 310 && my > 95) {
            for (auto& mb : menu_blocks) {
                SDL_Rect actual_pos = mb.rect;
                actual_pos.y += sidebar_scroll_y;
                if (SDL_PointInRect(&mPos, &actual_pos)) {
                    Blocks new_block = mb;
                    new_block.inputs.clear();
                    new_block.needsEvaluation = true;

                    if (blockMap.count(mb.id)) {
                        for (auto& Input : blockMap[mb.id].inputs) {
                            InputValue val;
                            val.type = InputValue::DIRECT_VALUE;
                            val.directValue = Input.defaultValue;
                            val.selectedOptionIndex = 0;
                            new_block.inputs.push_back(val);
                        }
                    }

                    int correct_width = calculatingWidthBlock(blockMap[mb.id], new_block.inputs, code_bar_font, -1, -1);
                    new_block.rect.w = correct_width;
                    new_block.rect.h = blockMap[mb.id].height;
                    new_block.rect.x = mx - new_block.rect.w/2;
                    new_block.rect.y = my - new_block.rect.h/2;

                    blockChains.push_back({new_block});
                    draggedChainIndex = blockChains.size() - 1;
                    draggedBlockIndex = 0;
                    offsetX = mx - blockChains[draggedChainIndex][0].rect.x;
                    offsetY = my - blockChains[draggedChainIndex][0].rect.y;
                    clickedOnMenu = true;
                    break;
                }
            }
        }

        if (!clickedOnMenu) {
            bool found = false;

            for (int c = blockChains.size() - 1; c >= 0; c--) {
                for (int b = blockChains[c].size() - 1; b >= 0; b--) {
                    HitResult hit = CheckBlockHit(mx, my, c, b);

                    if (hit.isOnChildBlock) {
                        draggedChainIndex = hit.chainIndex;
                        draggedBlockIndex = hit.blockIndex;
                        offsetX = mx - blockChains[draggedChainIndex][draggedBlockIndex].rect.x;
                        offsetY = my - blockChains[draggedChainIndex][draggedBlockIndex].rect.y;

                        Blocks& child = blockChains[draggedChainIndex][draggedBlockIndex];
                        if (child.parentChainIndex != -1) {
                            Blocks& parent = blockChains[child.parentChainIndex][child.parentBlockIndex];

                            for (auto it = parent.childBlocks.begin(); it != parent.childBlocks.end(); ++it) {
                                if (*it == &child) {
                                    parent.childBlocks.erase(it);
                                    break;
                                }
                            }

                            if (child.parentInputIndex >= 0 && child.parentInputIndex < parent.inputs.size()) {
                                parent.inputs[child.parentInputIndex].type = InputValue::DIRECT_VALUE;
                                parent.inputs[child.parentInputIndex].directValue = "";
                            }

                            child.needsEvaluation = true;

                            child.parentChainIndex = -1;
                            child.parentBlockIndex = -1;
                            child.parentInputIndex = -1;
                        }

                        if (draggedBlockIndex > 0) {
                            SplitChain(draggedChainIndex, draggedBlockIndex);
                            for (int nc = 0; nc < blockChains.size(); nc++) {
                                for (int nb = 0; nb < blockChains[nc].size(); nb++) {
                                    if (&blockChains[nc][nb] == &child) {
                                        draggedChainIndex = nc;
                                        draggedBlockIndex = nb;
                                        break;
                                    }
                                }
                            }
                        }

                        found = true;
                        break;
                    }
                }
                if (found) break;
            }

            if (!found) {
                int chainIdx, blockIdx;
                if (FindClickedBlock(mx, my, chainIdx, blockIdx)) {
                    if (blockIdx > 0) {
                        SplitChain(chainIdx, blockIdx);
                        FindClickedBlock(mx, my, chainIdx, blockIdx);
                    }

                    draggedChainIndex = chainIdx;
                    draggedBlockIndex = blockIdx;
                    offsetX = mx - blockChains[draggedChainIndex][draggedBlockIndex].rect.x;
                    offsetY = my - blockChains[draggedChainIndex][draggedBlockIndex].rect.y;
                }
            }
        }
    }

    if (e.type == SDL_MOUSEMOTION && draggedChainIndex != -1) {
        if (SDL_GetTicks() - clickStartTime > CLICK_THRESHOLD ||
            abs(mx - offsetX - blockChains[draggedChainIndex][draggedBlockIndex].rect.x) > 5) {

            isDraggingToInput = true;

            int deltaX = mx - offsetX - blockChains[draggedChainIndex][draggedBlockIndex].rect.x;
            int deltaY = my - offsetY - blockChains[draggedChainIndex][draggedBlockIndex].rect.y;

            Blocks& currentBlock = blockChains[draggedChainIndex][draggedBlockIndex];

            if (currentBlock.parentChainIndex == -1) {
                for (auto& block : blockChains[draggedChainIndex]) {
                    block.rect.x += deltaX;
                    block.rect.y += deltaY;
                }

                for (auto& block : blockChains[draggedChainIndex]) {
                    if (!block.childBlocks.empty()) {
                        UpdateAllChildPositions(draggedChainIndex, &block - &blockChains[draggedChainIndex][0]);
                    }
                }
            } else {
                currentBlock.rect.x += deltaX;
                currentBlock.rect.y += deltaY;

                if (!currentBlock.childBlocks.empty()) {
                    UpdateAllChildPositions(draggedChainIndex, draggedBlockIndex);
                }

                if (currentBlock.parentChainIndex != -1) {
                    UpdateAllChildPositions(currentBlock.parentChainIndex, currentBlock.parentBlockIndex);
                }
            }
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        if (draggedChainIndex != -1 && SDL_GetTicks() - clickStartTime < CLICK_THRESHOLD && !isDraggingToInput) {
            ExecuteChain(draggedChainIndex);
        }
        else if (draggedChainIndex != -1 && isDraggingToInput) {
            bool placedInInput = false;

            for (int c = 0; c < blockChains.size(); c++) {
                for (int b = 0; b < blockChains[c].size(); b++) {
                    if (c == draggedChainIndex && b == draggedBlockIndex) continue;

                    Blocks& targetBlock = blockChains[c][b];

                    int current_x = targetBlock.rect.x + 5;
                    current_x += Get_text_width(code_bar_font, blockMap[targetBlock.id].Back_label) + 5;

                    for (size_t i = 0; i < targetBlock.inputs.size(); i++) {
                        int input_width = 40;

                        SDL_Rect input_rect = {
                                current_x,
                                targetBlock.rect.y + 9,
                                input_width - 3,
                                20
                        };

                        if (mx >= input_rect.x && mx <= input_rect.x + input_rect.w &&
                            my >= input_rect.y && my <= input_rect.y + input_rect.h) {

                            Blocks& sourceBlock = blockChains[draggedChainIndex][draggedBlockIndex];
                            Block_category sourceCat = blockMap[sourceBlock.id].category;

                            if (sourceCat == CAT_OPERATORS || sourceCat == CAT_SENSING) {
                                PlaceBlockInInput(c, b, i, draggedChainIndex, draggedBlockIndex);
                                placedInInput = true;
                            }
                            break;
                        }

                        current_x += input_width + 5;
                        if (i < blockMap[targetBlock.id].labels.size()) {
                            current_x += Get_text_width(code_bar_font, blockMap[targetBlock.id].labels[i]) + 5;
                        }
                    }
                    if (placedInInput) break;
                }
                if (placedInInput) break;
            }

            if (!placedInInput) {
                int safeZoneX_Start = 310;
                int safeZoneY_Start = 95;

                bool shouldDelete = false;
                for (auto& block : blockChains[draggedChainIndex]) {
                    if (block.rect.x < safeZoneX_Start || block.rect.y < safeZoneY_Start || block.rect.x > 900) {
                        shouldDelete = true;
                        break;
                    }
                }

                if (shouldDelete) {
                    blockChains.erase(blockChains.begin() + draggedChainIndex);
                } else {
                    bool snapped = false;

                    for (int i = 0; i < blockChains.size(); i++) {
                        if (i == draggedChainIndex) continue;
                        if (blockChains[i].empty()) continue;

                        Blocks& firstBlock = blockChains[i].front();
                        Blocks& lastBlock = blockChains[i].back();
                        Blocks& draggedFirst = blockChains[draggedChainIndex].front();
                        Blocks& draggedLast = blockChains[draggedChainIndex].back();

                        if (IsNearForSnap(lastBlock, draggedFirst)) {
                            int deltaY = (lastBlock.rect.y + lastBlock.rect.h - 5) - draggedFirst.rect.y;
                            for (auto& block : blockChains[draggedChainIndex]) {
                                block.rect.y += deltaY;
                                block.rect.x = lastBlock.rect.x;
                            }

                            MergeChains(i, draggedChainIndex, false);
                            snapped = true;
                            break;
                        }

                        if (IsNearForSnap(draggedLast, firstBlock)) {
                            int deltaY = (firstBlock.rect.y - draggedLast.rect.h + 5) - draggedFirst.rect.y;
                            for (auto& block : blockChains[draggedChainIndex]) {
                                block.rect.y += deltaY;
                                block.rect.x = firstBlock.rect.x;
                            }

                            MergeChains(i, draggedChainIndex, true);
                            snapped = true;
                            break;
                        }
                    }
                }
            }
        }

        draggedChainIndex = -1;
        draggedBlockIndex = -1;
        isClickAndHold = false;
        isDraggingToInput = false;
    }
}

void Handle_Scroll_Events(int mx, int my, const SDL_Event& e) {
    if (e.type == SDL_MOUSEWHEEL && !isLibraryOpen) {
        if (mx < 110 && currentTab == BACKDROPS) {
            if (e.wheel.y > 0) backdropScrollY -= 30;
            else if (e.wheel.y < 0) backdropScrollY += 30;

            if (backdropScrollY < 0) backdropScrollY = 0;
            int contentHeight = projectBackdrops.size() * 110;
            int visibleHeight = Get_height() - 200;
            if (backdropScrollY > contentHeight - visibleHeight && contentHeight > visibleHeight)
                backdropScrollY = contentHeight - visibleHeight;
        }
    }
}

void Handle_Tab_Switch(int mx, int my) {
    if (my >= 50 && my <= 90) {
        if (mx >= 100 && mx <= 200) currentTab = CODE;
        else if (mx >= 200 && mx <= 300) {
            currentTab = BACKDROPS;
            backdropScrollY = 0;
        }
    }
}

void Handle_Backdrop_Selection(int mx, int my) {
    if (currentTab == BACKDROPS && mx < 105 && my > 95) {
        for (int i = 0; i < (int)projectBackdrops.size(); i++) {
            int yPos = 110 + (i * 110) - backdropScrollY;
            if (my >= yPos && my <= yPos + 90) {
                selectedBackdropIndex = i;
                currentBackdropTexture = projectBackdrops[i].texture;
                break;
            }
        }
    }
}

void Handle_Backdrop_Menu_Clicks(int mx, int my) {
    int cx = Get_width() - 53;
    int cy = 760;

    if (isBackdropMenuOpen) {
         if (IsCircleClicked(mx, my, cx, cy - 150, 20)) {
            UploadBackdrop();
            isBackdropMenuOpen = false;
        }
         else if (IsCircleClicked(mx, my, cx, cy - 100, 20)) {
            CreateNewPaintBackdrop();
            isBackdropMenuOpen = false;
        }
         else if (IsCircleClicked(mx, my, cx, cy - 50, 20)) {
            SetRandomBackdrop();
            isBackdropMenuOpen = false;
        }
         else if (IsCircleClicked(mx, my, cx, cy, 20)) {
            isLibraryOpen = true;
            isBackdropMenuOpen = false;
        }
    }
}

void HandleToolSelection(int mx, int my) {
    int toolX = 115, toolY = 220, btnS = 45;
    for (int i = 0; i < 9; i++) {
        SDL_Rect r = { toolX + (i % 2) * 55, toolY + (i / 2) * 55, btnS, btnS };
        if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
            if (i == 8) {
                currentTab = CODE;
            }
            else if (i == 7) {
                if (selectedBackdropIndex >= 0) {
                    isSaveModalOpen = true;
                    saveInputText = projectBackdrops[selectedBackdropIndex].name;
                    SDL_StartTextInput();
                }
            }
            else {
                if (isTyping && i != 4) ApplyTextToLayer();
                if (i == 0) ClearCurrentDrawingLayer(renderer);
                else if (i == 1) globalEditor.activeTool = TOOL_PEN;
                else if (i == 2) globalEditor.activeTool = TOOL_LINE;
                else if (i == 3) globalEditor.activeTool = TOOL_CIRCLE;
                else if (i == 4) globalEditor.activeTool = TOOL_TEXT;
                else if (i == 5) globalEditor.activeTool = TOOL_FILL;
                else if (i == 6) globalEditor.activeTool = TOOL_ERASER;
            }
            break;
        }
    }
}
void HandleCanvasMouseDown(int mx, int my) {
    if (currentTab != BACKDROPS || selectedBackdropIndex < 0) return;
    if (mx < 330 || mx > 930 || my < 280 || my > 660) return;

    auto tool = globalEditor.activeTool;
    if (tool == TOOL_TEXT) {
        if (isTyping) ApplyTextToLayer();
        isTyping = true;
        textX = mx; textY = my;
        textInput = "";
        SDL_StartTextInput();
    } else if (tool == TOOL_FILL) {
        ApplyFill(projectBackdrops[selectedBackdropIndex].drawingLayer, renderer);
    } else if (tool == TOOL_CIRCLE) {
        circleStartX = mx; circleStartY = my;
        isDrawingCircle = true;
    } else if (tool == TOOL_LINE) {
        lineStartX = mx; lineStartY = my;
        isDrawingLine = true;
    }
}

void HandleColorSelection(int mx, int my) {
    int colorX = 110 + 300;
    int colorY = 95 + 32;
    SDL_Color colors[] = {{0, 0, 0, 255}, {255, 0, 0, 255}, {0, 0, 255, 255}};

    for (int i = 0; i < 3; i++) {
        int targetX = colorX + i * 45;
        int targetY = colorY;
        if (pow(mx - targetX, 2) + pow(my - targetY, 2) <= pow(15, 2)) {
            globalEditor.currentColor = colors[i];
            break;
        }
    }
}
void MapMouseToCanvas(int mx, int my, int* outX, int* outY, SDL_Texture* target) {
    int tw, th;
    SDL_QueryTexture(target, NULL, NULL, &tw, &th);
    *outX = (mx - 330) * tw / 600;
    *outY = (my - 280) * th / 380;
}
void HandleCanvasMouseUp(int mx, int my) {
    if (selectedBackdropIndex < 0) return;
    SDL_Texture* target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    if (!target) return;

    int x1, y1, x2, y2;
    if (isDrawingCircle && globalEditor.activeTool == TOOL_CIRCLE) {
        MapMouseToCanvas(circleStartX, circleStartY, &x1, &y1, target);
        MapMouseToCanvas(mx, my, &x2, &y2, target);
        int radius = (int)sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        DrawCircleOnTexture(target, x1, y1, radius, renderer, true);
    } else if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) {
        MapMouseToCanvas(lineStartX, lineStartY, &x1, &y1, target);
        MapMouseToCanvas(mx, my, &x2, &y2, target);
        DrawLineOnTexture(target, x1, y1, x2, y2, renderer, false);
    }
    isDrawingCircle = isDrawingLine = false;
    lastMouseX = lastMouseY = -1;
}

void HandleContinuousDrawing(int mx, int my) {
    if (mx < 330 || mx > 930 || my < 280 || my > 660) {
        lastMouseX = -1;
        lastMouseY = -1;
        return;
    }

    SDL_Texture* target = nullptr;
    if (currentTab == COSTUMES && now_sprite && !now_sprite->costumes.empty()) {
        target = now_sprite->costumes[now_sprite->currentCostumeIndex]->drawingLayer;
    } else if (currentTab == BACKDROPS && !projectBackdrops.empty()) {
        target = projectBackdrops[selectedBackdropIndex].drawingLayer;
    }

    if (!target) return;

    int relX = mx - 330;
    int relY = my - 280;

    if (globalEditor.activeTool == TOOL_PEN || globalEditor.activeTool == TOOL_ERASER) {
        SDL_SetRenderTarget(renderer, target);

        if (globalEditor.activeTool == TOOL_ERASER) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        } else {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
        }

        if (lastMouseX != -1 && lastMouseY != -1) {
            int prevRelX = lastMouseX - 330;
            int prevRelY = lastMouseY - 280;

            for (int i = -globalEditor.brushSize; i <= globalEditor.brushSize; i++) {
                SDL_RenderDrawLine(renderer, prevRelX + i, prevRelY, relX + i, relY);
                SDL_RenderDrawLine(renderer, prevRelX, prevRelY + i, relX, relY + i);
            }
        } else {
            SDL_Rect r = { relX - globalEditor.brushSize, relY - globalEditor.brushSize, globalEditor.brushSize * 2, globalEditor.brushSize * 2 };
            SDL_RenderFillRect(renderer, &r);
        }

        SDL_SetRenderTarget(renderer, NULL);
        lastMouseX = mx;
        lastMouseY = my;
    }
}

void HandleBrushSizeSelection(int mx, int my) {
    int sizeX = 110 + 450;
    int sizeY = 95 + 32;
    int sizes[] = {2, 5, 10};

    for (int i = 0; i < 3; i++) {
        int targetX = sizeX + i * 40;
        int targetY = sizeY;
        if ((mx - targetX) * (mx - targetX) + (my - targetY) * (my - targetY) <= 400) {
            globalEditor.brushSize = sizes[i];
            break;
        }
    }
}

void SaveToLibrary(string name, SDL_Renderer* renderer) {
    if (selectedBackdropIndex < 0 || selectedBackdropIndex >= (int)projectBackdrops.size()) return;

    Backdrop& bd = projectBackdrops[selectedBackdropIndex];
    int w, h;
    SDL_QueryTexture(bd.texture, NULL, NULL, &w, &h);

    SDL_Texture* combinedTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

    SDL_SetRenderTarget(renderer, combinedTex);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, bd.texture, NULL, NULL);
    if (bd.drawingLayer) {
        SDL_RenderCopy(renderer, bd.drawingLayer, NULL, NULL);
    }

    SDL_SetRenderTarget(renderer, oldTarget);

    BackdropItem newItem;
    newItem.texture = combinedTex;
    newItem.name = name;
    libraryItems.push_back(newItem);

    isSaveModalOpen = false;
    saveInputText = "";
    SDL_StopTextInput();
}

void FlipCostumeHorizontal(Costume* costume) {
    if (!costume || !costume->texture) return;

    int w, h;
    SDL_QueryTexture(costume->texture, NULL, NULL, &w, &h);

    SDL_Texture* tempTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(tempTex, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, tempTex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_RenderCopyEx(renderer, costume->texture, NULL, NULL, 0, NULL, SDL_FLIP_HORIZONTAL);

    SDL_SetRenderTarget(renderer, NULL);
    SDL_DestroyTexture(costume->texture);
    costume->texture = tempTex;
}

void FlipCostumeVertical(Costume* costume) {
    if (!costume || !costume->texture) return;

    int w, h;
    SDL_QueryTexture(costume->texture, NULL, NULL, &w, &h);

    SDL_Texture* tempTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(tempTex, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, tempTex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_RenderCopyEx(renderer, costume->texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);

    SDL_SetRenderTarget(renderer, NULL);
    SDL_DestroyTexture(costume->texture);
    costume->texture = tempTex;
}

void FlipHorizontal(Costume* costume) {
    if (!costume || !costume->texture) return;

    int w, h;
    SDL_QueryTexture(costume->texture, NULL, NULL, &w, &h);

    SDL_Texture* target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!target) return;

    SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, target);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_RenderCopyEx(renderer, costume->texture, NULL, NULL, 0, NULL, SDL_FLIP_HORIZONTAL);

    SDL_SetRenderTarget(renderer, NULL);

    SDL_DestroyTexture(costume->texture);
    costume->texture = target;
}

void FlipVertical(Costume* costume) {
    if (!costume || !costume->texture) return;

    int w, h;
    SDL_QueryTexture(costume->texture, NULL, NULL, &w, &h);

    SDL_Texture* target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!target) return;

    SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, target);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_RenderCopyEx(renderer, costume->texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);

    SDL_SetRenderTarget(renderer, NULL);

    SDL_DestroyTexture(costume->texture);
    costume->texture = target;
}

void AddNewCharacterFromFile(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) return;
    Character newChar;
    newChar.name = "Sprite " + std::to_string(allCharacters.size() + 1);
    Costume* newCostume = new Costume();
    newCostume->name = "costume1";
    newCostume->texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w, h;
    SDL_QueryTexture(newCostume->texture, NULL, NULL, &w, &h);
    newChar.width = w;
    newChar.height = h;
    newCostume->drawingLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 600, 380);
    SDL_SetTextureBlendMode(newCostume->drawingLayer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, newCostume->drawingLayer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    newChar.costumes.push_back(newCostume);
    allCharacters.push_back(newChar);
    now_sprite = &allCharacters.back();
    SDL_FreeSurface(surface);
}


void ImportCharacterImage(SDL_Renderer* renderer) {
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // یا GetActiveWindow()
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Images\0*.png;*.jpg;*.jpeg;*.bmp\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        SDL_Surface* surface = IMG_Load(ofn.lpstrFile);
        if (surface) {
            Character newChar;
            newChar.name = "Sprite " + to_string(allCharacters.size() + 1);
            newChar.x = 640;
            newChar.y = 360;
            newChar.size = 0.5;
            newChar.isvisible = true;

            Costume* newCostume = new Costume();
            newCostume->name = "costume1";
            newCostume->texture = SDL_CreateTextureFromSurface(renderer, surface);

            int w, h;
            SDL_QueryTexture(newCostume->texture, NULL, NULL, &w, &h);
            newChar.width = w;
            newChar.height = h;

            newCostume->drawingLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 600, 380);
            SDL_SetTextureBlendMode(newCostume->drawingLayer, SDL_BLENDMODE_BLEND);

            SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
            SDL_SetRenderTarget(renderer, newCostume->drawingLayer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, oldTarget);

            newChar.costumes.push_back(newCostume);
            newChar.currentCostumeIndex = 0;

            allCharacters.push_back(newChar);
            now_sprite = &allCharacters.back();
            SDL_FreeSurface(surface);
        }
    }
}

void Get_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) stop = true;
        Handle_event_for_code_button(e);
        Handle_event_for_motion_sprite(e, now_sprite);
        Handle_event_for_flag_button(e, flag_button);
        Handle_event_for_stop_button(e, stop_button);
        Handle_event_for_show_button(e, show_button, now_sprite);
        Handle_event_for_hide_button(e, hide_button, now_sprite);
        Handle_event_for_Back_button(e, &Back_button);
        Handle_event_for_Backdrop_button(e, &Backdrop_button);
        Handle_event_for_sound_button(e, &Sounds_button);
        Handle_event_for_Character_button(e, &Character_button);
        Handle_event_for_Code_button(e, &code_button);
        Handle_event_for_choose_now_sprite(e, &cat_buttonUnderstage);
        Handle_event_for_choose_now_sprite(e, &dog_buttonUnderstage);
        Handle_event_for_choose_now_sprite(e, &fish_buttonUnderstage);
        Handle_event_for_choose_now_sprite(e, &balloon_buttonUnderstage);
        Handle_event_for_choose_now_sprite(e, &bear_buttonUnderstage);
        Handle_event_for_choose_now_sprite(e, &apple_buttonUnderstage);
        if (currentTab == CHARACTER) {
            Handle_event_for_choose_sprite_in_Character_panel(e, &cat_button, &cat_buttonUnderstage);
            Handle_event_for_choose_sprite_in_Character_panel(e, &dog_button, &dog_buttonUnderstage);
            Handle_event_for_choose_sprite_in_Character_panel(e, &fish_button, &fish_buttonUnderstage);
            Handle_event_for_choose_sprite_in_Character_panel(e, &balloon_button, &balloon_buttonUnderstage);
            Handle_event_for_choose_sprite_in_Character_panel(e, &bear_button, &bear_buttonUnderstage);
            Handle_event_for_choose_sprite_in_Character_panel(e, &apple_button, &apple_buttonUnderstage);
            Handle_event_for_choose_sprite_in_Character_panel(e, &emoji_button, &emoji_buttonUnderstage);
        }
        if (currentTab == SOUNDS) {
            Handle_event_for_run_button(e, &run_sound_button);
            Handle_event_for_volumeUp_button(e, &volumeUp_button);
            Handle_event_for_volumeDown_button(e, &volumeDown_button);
            Handle_event_for_increaseFrequency_button(e, &increase_frequency_button);
            Handle_event_for_decreaseFrequency_button(e, &decrease_frequency_button);
        }
        if (currentTab == CODE) {
            Handle_event_for_timer_button(e, &Timer_button);
            Handle_event_for_next_costume_button(e, renderer, &next_costume_button, now_sprite);
            Handle_event_for_costume_number_button(e, &costume_number_button);
            Handle_event_for_size_button(e, &size_button);
            Handle_event_for_drag_button(e, renderer, &drag_button, now_sprite);
            Handle_event_for_go_to_front_layer_button(e, renderer, &go_to_front_layer_button, now_sprite);
        }
        int mx, my;
        Uint32 mouseState = SDL_GetMouseState(&mx, &my);
        bool isLeftPressed = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

        if (isSaveModalOpen) {
            HandleKeyboardInput(e);
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mW = 400, mH = 200;
                int mX = (Get_width() - mW) / 2;
                int mY = (Get_height() - mH) / 2;

                if (mx >= mX + 280 && mx <= mX + 380 && my >= mY + 140 && my <= mY + 180) {
                    SaveToLibrary(saveInputText, renderer);
                    isSaveModalOpen = false;
                    saveInputText = "";
                    SDL_StopTextInput();
                } else if (mx >= mX + 170 && mx <= mX + 270 && my >= mY + 140 && my <= mY + 180) {
                    isSaveModalOpen = false;
                    saveInputText = "";
                    SDL_StopTextInput();
                }
            }
            continue;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (isLibraryOpen) {
                if (mx >= 20 && mx <= 120 && my >= 10 && my <= 50) isLibraryOpen = false;
                else HandleLibraryClick(mx, my);
            } else {
                double distUpload = sqrt(pow(mx - (Get_width() - 130), 2) + pow(my - (Get_height() - 100 - 42), 2));
                if (distUpload <= 18) {
                    ImportCharacterImage(renderer);
                }

                Handle_Tab_Switch(mx, my);
                Handle_Backdrop_Selection(mx, my);
                Handle_Backdrop_Menu_Clicks(mx, my);

                double distPaint = sqrt(pow(mx - (Get_width() - 130), 2) + pow(my - (Get_height() - 100), 2));
                if (distPaint <= 18) currentTab = COSTUMES;

                if (currentTab == COSTUMES || currentTab == BACKDROPS) {
                    if (my >= 95 && my <= 205) {
                        HandleColorSelection(mx, my);
                        HandleBrushSizeSelection(mx, my);

                        if (currentTab == COSTUMES && now_sprite != nullptr && !now_sprite->costumes.empty()) {
                            Costume *curr = now_sprite->costumes[now_sprite->currentCostumeIndex];
                            if (my >= 160 && my <= 190) {
                                if (mx >= 350 && mx <= 450) FlipHorizontal(curr);
                                else if (mx >= 460 && mx <= 560) FlipVertical(curr);
                                else if (mx >= 570 && mx <= 620) {
                                    now_sprite->size += 0.05;
                                    if (now_sprite->size > 3.0) now_sprite->size = 3.0;
                                } else if (mx >= 630 && mx <= 680) {
                                    now_sprite->size -= 0.05;
                                    if (now_sprite->size < 0.05) now_sprite->size = 0.05;
                                }
                            }
                        }
                    }
                    else if (mx >= 115 && mx <= 225 && my >= 220) {
                        HandleToolSelection(mx, my);
                    }
                    else if (mx >= 330 && mx <= 930 && my >= 280 && my <= 660) {
                        HandleCanvasMouseDown(mx, my);
                    }
                }
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            if (currentTab == COSTUMES || currentTab == BACKDROPS) {
                HandleCanvasMouseUp(mx, my);
            }
        }

        if (isLeftPressed && !isLibraryOpen && !isSaveModalOpen) {
        if (currentTab == COSTUMES || currentTab == BACKDROPS) {
        if (mx >= 330 && mx <= 930 && my >= 280 && my <= 660) {
        HandleContinuousDrawing(mx, my);
        } else {
        lastMouseX = -1;
        lastMouseY = -1;
        }
        }
        }

        if (currentTab == CODE && !isLibraryOpen && !isSaveModalOpen) {
        HandleBlockEvent(e);
        }

        HandleKeyboardInput(e);
        }
        }

void Draw_Stage_Content(SDL_Renderer* renderer) {
    SDL_Rect stageArea = stage;

    if (!projectBackdrops.empty() && selectedBackdropIndex >= 0 && selectedBackdropIndex < (int)projectBackdrops.size()) {
        SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].texture, NULL, &stageArea);
        SDL_RenderCopy(renderer, projectBackdrops[selectedBackdropIndex].drawingLayer, NULL, &stageArea);
    }

    int centerX = stageArea.x + (stageArea.w / 2);
    int centerY = stageArea.y + (stageArea.h / 2);

    for (auto& ch : allCharacters) {
        if (ch.isvisible && !ch.costumes.empty()) {

            int rSize = (int)(ch.size * 500);
            if (rSize <= 0) rSize = 1;

            ch.width = (double)rSize;
            ch.height = (double)rSize;

            SDL_Rect charPos = {
                    centerX + (int)ch.x - (rSize / 2),
                    centerY + (int)ch.y - (rSize / 2),
                    rSize,
                    rSize
            };

            SDL_RenderCopyEx(renderer,
                             ch.costumes[ch.currentCostumeIndex]->texture,
                             NULL,
                             &charPos,
                             (double)ch.degree,
                             NULL,
                             SDL_FLIP_NONE);
        }
    }
}

void DrawSaveModal(SDL_Renderer* renderer, TTF_Font* font) {
    int mW = 400, mH = 200;
    int mX = (Get_width() - mW) / 2;
    int mY = (Get_height() - mH) / 2;

    SDL_Rect bg = { mX, mY, mW, mH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);

    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Texture* title = LoadText(renderer, font, "Save Backdrop As:", black);
    SDL_Rect titleRect = { mX + 20, mY + 20, 0, 0 };
    SDL_QueryTexture(title, NULL, NULL, &titleRect.w, &titleRect.h);
    SDL_RenderCopy(renderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

    SDL_Rect inputArea = { mX + 20, mY + 60, mW - 40, 40 };
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &inputArea);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &inputArea);

    if (!saveInputText.empty()) {
        SDL_Texture* inputTex = LoadText(renderer, font, saveInputText, black);
        SDL_Rect txtR = { mX + 30, mY + 70, 0, 0 };
        SDL_QueryTexture(inputTex, NULL, NULL, &txtR.w, &txtR.h);
        SDL_RenderCopy(renderer, inputTex, NULL, &txtR);
        SDL_DestroyTexture(inputTex);
    }

    SDL_Rect btnSave = { mX + 280, mY + 140, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
    SDL_RenderFillRect(renderer, &btnSave);
    SDL_Texture* sText = LoadText(renderer, font, "Save", { 255, 255, 255, 255 });
    SDL_Rect sTextR = { mX + 310, mY + 150, 40, 20 };
    SDL_RenderCopy(renderer, sText, NULL, &sTextR);
    SDL_DestroyTexture(sText);

    SDL_Rect btnCancel = { mX + 170, mY + 140, 100, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &btnCancel);
    SDL_Texture* cText = LoadText(renderer, font, "Cancel", black);
    SDL_Rect cTextR = { mX + 195, mY + 150, 50, 20 };
    SDL_RenderCopy(renderer, cText, NULL, &cTextR);
    SDL_DestroyTexture(cText);
}

void Update() {
    UpdateMenuState();
    UpdateExecution();
    UpdateOutputDisplay();
    Update_Character_Menu_State();

    SDL_SetRenderDrawColor(renderer, 229, 240, 255, 255);
    SDL_RenderClear(renderer);

    Draw_flag_and_stop_button(renderer, flag_button, stop_button, green_flag, stop_sign);

    if (isLibraryOpen) {
        DrawBackdropLibrary(renderer, main_font);
    } else {
        Draw_BlueBar_Top(renderer, Get_width(), Scratch_logo);
        Draw_Top_Button(renderer, Top_button, File_Text);
        if (currentTab == SOUNDS) {
            Draw_sound_panel(renderer);
            Draw_sounds_functions_button(renderer, run_sound_button, Run_text);
            Draw_sounds_functions_button(renderer, volumeUp_button, volumeUp_text);
            Draw_sounds_functions_button(renderer, volumeDown_button, volumeDown_text);
            Draw_sounds_functions_button(renderer, increase_frequency_button, increase_frequency_text);
            Draw_sounds_functions_button(renderer, decrease_frequency_button, decrease_frequency_text);
            Draw_report_button(renderer,&volume_button,volume_value);
            Draw_volume_text(renderer,volume_text);
            Draw_report_button(renderer,&frequency_button,frequency_value);
            Draw_frequency_text(renderer,frequency_text);
        }
        if (currentTab == CODE) {
            Draw_RunningBar(renderer);
            Draw_CodeBar(renderer);
            Draw_CodeBar_Item(renderer, categories);
            Draw_Menu_Blocks(renderer, code_bar_font);
            DrawALLBlocks(renderer, code_bar_font);
            Draw_report_button(renderer, &Timer_button, Timer_text);
            Draw_report_button(renderer, &size_button, size_button_text);
            Draw_report_button(renderer, &next_costume_button, next_costume_text);
            Draw_report_button(renderer, &costume_number_button, costume_number_text);
        } else if (currentTab == BACKDROPS || currentTab == COSTUMES) {
            Draw_Backdrop_List_Sidebar(renderer, main_font);

            SDL_Texture* baseTex = nullptr;
            string bName = "Backdrop";
            if (!projectBackdrops.empty() && selectedBackdropIndex >= 0) {
                baseTex = projectBackdrops[selectedBackdropIndex].texture;
                bName = projectBackdrops[selectedBackdropIndex].name;
            }

            Draw_Image_Editor(renderer, main_font, baseTex, bName);

            if (selectedBackdropIndex >= 0 && selectedBackdropIndex < (int) projectBackdrops.size()) {
                if (isDrawingCircle && globalEditor.activeTool == TOOL_CIRCLE) {
                    int curX, curY;
                    SDL_GetMouseState(&curX, &curY);
                    int radius = (int) sqrt(pow(curX - circleStartX, 2) + pow(curY - circleStartY, 2));
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g,
                                           globalEditor.currentColor.b, 255);
                    for (int a = 0; a < 360; a++) {
                        float r1 = a * M_PI / 180.0f;
                        float r2 = (a + 1) * M_PI / 180.0f;
                        SDL_RenderDrawLine(renderer, circleStartX + (int) (radius * cos(r1)),
                                           circleStartY + (int) (radius * sin(r1)),
                                           circleStartX + (int) (radius * cos(r2)),
                                           circleStartY + (int) (radius * sin(r2)));
                    }
                } else if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) {
                    int curX, curY;
                    SDL_GetMouseState(&curX, &curY);
                    SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g,
                                           globalEditor.currentColor.b, 255);
                    SDL_RenderDrawLine(renderer, lineStartX, lineStartY, curX, curY);
                }
            }
        }
        Draw_Information_of_Character(renderer);
        Draw_Code_button(renderer, code_button, code_text);
        Draw_Character_button(renderer,Character_button,Character_text);
        Draw_sound_button(renderer, Sounds_button, Sound_text);
        Draw_Backdrop_button(renderer, Backdrop_button, Backdrop_text);
        Draw_Back_button(renderer, Back_button, Back_text);
        Draw_Character_Show_Bar(renderer);
        Draw_Stage_Bar(renderer, main_font);
        Draw_Stage_Content(renderer);
        Draw_report_informationOfCharacter_box(renderer, name_of_sprite);
        Draw_report_informationOfCharacter_box(renderer, direction);
        Draw_report_informationOfCharacter_box(renderer, size);
        Draw_report_informationOfCharacter_box(renderer, positionX);
        Draw_report_informationOfCharacter_box(renderer, positionY);

        Draw_X_text(renderer, X_text);
        Draw_Y_text(renderer, Y_text);
        Draw_size_text(renderer, size_text);
        Draw_direction_text(renderer, direction_text);
        Draw_sprite_text(renderer, sprite_text);
        Draw_show_text(renderer, show_text);
        volume_value = LoadText(renderer,main_font,to_string(volume),{100,100,100});
        frequency_value = LoadText(renderer,main_font,to_string(frequency),{100,100,100});
        Draw_show_and_hide_button(renderer, show_button, hide_button, S_text, H_text, now_sprite);
        name_of_sprite_text = LoadText(renderer, report_font, now_sprite->name, {100, 100, 100});
        Draw_name_of_character(renderer, name_of_sprite, name_of_sprite_text);
        positionX_text = LoadText(renderer, report_font, to_string((int)now_sprite->x), {100, 100, 100});
        Draw_positionX(renderer, positionX, positionX_text);
        positionY_text = LoadText(renderer, report_font, to_string(-(int)now_sprite->y), {100, 100, 100});
        Draw_positionX(renderer, positionY, positionY_text);
        size_of_sprite_text = LoadText(renderer, report_font, to_string((int)(now_sprite->size *500)), {100, 100, 100});
        Draw_size(renderer, size, size_of_sprite_text);
        direction_of_sprite_text = LoadText(renderer, report_font, to_string((int)now_sprite->degree), {100, 100, 100});
        Draw_direction(renderer, direction, direction_of_sprite_text);
        Draw_sprite_button_under_stage(renderer,&cat_buttonUnderstage,cat_text);
        Draw_sprite_button_under_stage(renderer,&dog_buttonUnderstage,dog_text);
        Draw_sprite_button_under_stage(renderer,&balloon_buttonUnderstage,balloon_text);
        Draw_sprite_button_under_stage(renderer,&fish_buttonUnderstage,fish_text);
        Draw_sprite_button_under_stage(renderer,&apple_buttonUnderstage,apple_text);
        Draw_sprite_button_under_stage(renderer,&bear_buttonUnderstage,bear_text);
        Draw_sprite_button_under_stage(renderer,&emoji_buttonUnderstage,emoji_text);
        choose_now_sprite();
        Draw_Character(renderer);
        if (is_timer) timer(renderer, report_font);
        if (is_size_on) Draw_size_report(renderer, report_font, now_sprite);
        if (is_costume_number_on) Draw_costume_report(renderer, report_font, now_sprite);

        Draw_Code_button(renderer, code_button, code_text);
        Draw_sound_button(renderer, Sounds_button, Sound_text);
        Draw_Backdrop_button(renderer, Backdrop_button, Backdrop_text);
        Draw_Back_button(renderer, Back_button, Back_text);

        if (currentTab == SOUNDS) {
            Draw_sound_panel(renderer);
            Draw_sounds_functions_button(renderer, run_sound_button, Run_text);
            Draw_sounds_functions_button(renderer, volumeUp_button, volumeUp_text);
            Draw_sounds_functions_button(renderer, volumeDown_button, volumeDown_text);
            Draw_sounds_functions_button(renderer, increase_frequency_button, increase_frequency_text);
            Draw_sounds_functions_button(renderer, decrease_frequency_button, decrease_frequency_text);
            Draw_report_button(renderer, &volume_button, volume_value);
            Draw_volume_text(renderer, volume_text);
            Draw_report_button(renderer, &frequency_button, frequency_value);
            Draw_frequency_text(renderer, frequency_text);
        }
        else if (currentTab == CODE) {
            Draw_RunningBar(renderer);
            Draw_CodeBar(renderer);
            Draw_CodeBar_Item(renderer, categories);
            Draw_Menu_Blocks(renderer, code_bar_font);
            DrawALLBlocks(renderer, code_bar_font);
            Draw_report_button(renderer, &Timer_button, Timer_text);
            Draw_report_button(renderer, &size_button, size_button_text);
            Draw_report_button(renderer, &next_costume_button, next_costume_text);
            Draw_report_button(renderer, &costume_number_button, costume_number_text);
            Draw_report_button(renderer, &drag_button, drag_text);
            Draw_report_button(renderer, &go_to_front_layer_button, go_to_front_layer_text);
        }
        if(currentTab == CHARACTER){
            Draw_Character_panel(renderer);
            Draw_sprite_button(renderer,&cat_button,cat.costumes[0]->texture,cat_text);
            Draw_sprite_button(renderer,&dog_button,dog.costumes[0]->texture,dog_text);
            Draw_sprite_button(renderer,&bear_button,bear.costumes[0]->texture,bear_text);
            Draw_sprite_button(renderer,&fish_button,fish.costumes[0]->texture,fish_text);
            Draw_sprite_button(renderer,&balloon_button,balloon.costumes[0]->texture,balloon_text);
            Draw_sprite_button(renderer,&apple_button,red_apple.costumes[0]->texture,apple_text);
            Draw_sprite_button(renderer,&emoji_button,emoji.costumes[0]->texture,emoji_text);
        }
        else if (currentTab == BACKDROPS || currentTab == COSTUMES) {
            Draw_Backdrop_List_Sidebar(renderer, main_font);

            if (currentTab == COSTUMES && now_sprite != nullptr) {
                Draw_Image_Editor(renderer, main_font, nullptr, now_sprite->name);

                SDL_Rect flipH_rect = { 350, 160, 100, 30 };
                SDL_Rect flipV_rect = { 460, 160, 100, 30 };
                SDL_Rect sizeUp_rect = { 570, 160, 50, 30 };
                SDL_Rect sizeDown_rect = { 630, 160, 50, 30 };

                SDL_SetRenderDrawColor(renderer, 77, 151, 255, 255);
                SDL_RenderFillRect(renderer, &flipH_rect);
                SDL_RenderFillRect(renderer, &flipV_rect);
                SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                SDL_RenderFillRect(renderer, &sizeUp_rect);
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
                SDL_RenderFillRect(renderer, &sizeDown_rect);

                SDL_Texture* hTxt = LoadText(renderer, main_font, "Flip H", {255,255,255});
                SDL_Texture* vTxt = LoadText(renderer, main_font, "Flip V", {255,255,255});
                SDL_Texture* pTxt = LoadText(renderer, main_font, "+", {255,255,255});
                SDL_Texture* mTxt = LoadText(renderer, main_font, "-", {255,255,255});
                SDL_RenderCopy(renderer, hTxt, NULL, &flipH_rect);
                SDL_RenderCopy(renderer, vTxt, NULL, &flipV_rect);
                SDL_RenderCopy(renderer, pTxt, NULL, &sizeUp_rect);
                SDL_RenderCopy(renderer, mTxt, NULL, &sizeDown_rect);
                SDL_DestroyTexture(hTxt); SDL_DestroyTexture(vTxt);
                SDL_DestroyTexture(pTxt); SDL_DestroyTexture(mTxt);

                if (!now_sprite->costumes.empty()) {
                    Costume* currentCostume = now_sprite->costumes[now_sprite->currentCostumeIndex];
                    if (currentCostume && currentCostume->texture) {
                        int texW, texH;
                        SDL_QueryTexture(currentCostume->texture, NULL, NULL, &texW, &texH);
                        float editorScale = 0.5f;
                        int finalW = (int)(texW * editorScale);
                        int finalH = (int)(texH * editorScale);
                        SDL_Rect destRect = { 330 + (600 / 2) - (finalW / 2), 420 - (finalH / 2), finalW, finalH };
                        SDL_RenderCopy(renderer, currentCostume->texture, NULL, &destRect);

                        SDL_SetRenderDrawColor(renderer, globalEditor.currentColor.r, globalEditor.currentColor.g, globalEditor.currentColor.b, 255);
                        int curX, curY;
                        SDL_GetMouseState(&curX, &curY);
                        if (isDrawingCircle && globalEditor.activeTool == TOOL_CIRCLE) {
                            int rVal = (int)sqrt(pow(curX - circleStartX, 2) + pow(curY - circleStartY, 2));
                            for (int a = 0; a < 360; a++) {
                                float r1 = a * M_PI / 180.0f; float r2 = (a + 1) * M_PI / 180.0f;
                                SDL_RenderDrawLine(renderer, circleStartX + (int)(rVal * cos(r1)), circleStartY + (int)(rVal * sin(r1)),
                                                   circleStartX + (int)(rVal * cos(r2)), circleStartY + (int)(rVal * sin(r2)));
                            }
                        }
                        else if (isDrawingLine && globalEditor.activeTool == TOOL_LINE) SDL_RenderDrawLine(renderer, lineStartX, lineStartY, curX, curY);
                        else if (isDrawingRect && globalEditor.activeTool == TOOL_RECT) {
                            SDL_Rect r = { min(lineStartX, curX), min(lineStartY, curY), abs(curX - lineStartX), abs(curY - lineStartY) };
                            SDL_RenderDrawRect(renderer, &r);
                        }
                    }
                }
            }
            else if (currentTab == BACKDROPS && !projectBackdrops.empty() && selectedBackdropIndex >= 0) {
                Draw_Image_Editor(renderer, main_font, projectBackdrops[selectedBackdropIndex].texture, projectBackdrops[selectedBackdropIndex].name);
            }
        }

        if (isBackdropMenuOpen) DrawBackdropSubMenu(renderer);
        Draw_Character_Floating_Buttons(renderer);
    }
    if (isSaveModalOpen) DrawSaveModal(renderer, main_font);
    SDL_RenderPresent(renderer);
}
void Render(){
        SDL_RenderPresent(renderer);
}

void Clean(){
        TTF_CloseFont(main_font);
        TTF_Quit();
        Mix_CloseAudio();
        SDL_DestroyTexture(volume_value);
        SDL_DestroyTexture(frequency_value);
        SDL_DestroyTexture(name_of_sprite_text);
        SDL_DestroyTexture(positionY_text);
        SDL_DestroyTexture(positionX_text);
        SDL_DestroyTexture(size_of_sprite_text);
        SDL_DestroyTexture(direction_of_sprite_text);
        SDL_DestroyWindow(main_window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
}

void SDL_wait_key(){
        auto *e = new SDL_Event();
        while(e->type != SDL_KEYDOWN){
                SDL_PollEvent(e);
        }
}





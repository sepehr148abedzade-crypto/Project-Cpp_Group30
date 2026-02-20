#ifndef SCRIPT_H
#define SCRIPT_H
#include "vector"
#include "event_system.h"
#include "SDL2/SDL.h"

enum BlockFunctionsType {MOVE,TURN_CLOCKWISE,TURN_COUNTERCLOCKWISE,GO_TO_RANDOM_POSITION,GO_TO_X_Y,POINT_IN_DIRECTION,
        CHANGE_X_BY,SET_X_TO,CHANGE_Y_BY,SET_Y_TO,CHANGE_SIZE_BY,SET_SIZE_TO,SHOW_CHARACTER,
        HIDE_CHARACTER,SAY_A_FOR_T_SECONDS,SAY_A,THINK_A_FOR_T_SECOND,THINK_A,SWITCH_COSTUME_TO,
        PLAY_SOUND,CHANGE_VOLUME_BY,SET_VOLUME_TO,CHANGE_FREQUENCY,
        TIMER,DISTANCE_TO_MOUSE_POINTER
        };


struct Function_Block{
    BlockFunctionsType Type;

    double value1;
    double value2;

    std::string message1;
    std::string message2;

    bool stated = false;
    bool finished = false;

    Uint32 startTme = 0;
};

struct Script{
    std::vector<Function_Block> blocks;
    int index =0;
    bool running = false;
    EventStartType trigger;
};
#endif //SCRIPT_H

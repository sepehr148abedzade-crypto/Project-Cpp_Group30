#include "script_engine.h"
#include "motion_functions.h"
#include "looks_functions.h"
#include "sound_functions.h"
#include "sensing_functions.h"
#include "operators_functions.h"
#include "Entity.h"
#include "script.h"

void executeBlock(Function_Block &block){
    switch (block.Type) {
        case MOVE :
            move_steps_character(block.value1,now_sprite);
            block.finished = true;
            break;
        case TURN_CLOCKWISE :
            turn_clockwise_character(block.value1,now_sprite);
            block.finished = true;
            break;
        case TURN_COUNTERCLOCKWISE :
            turn_counterclockwise_character(block.value1,now_sprite);
            block.finished = true;
            break;
        case GO_TO_RANDOM_POSITION :
            go_to_random_position(now_sprite);
            block.finished = true;
            break;
        case GO_TO_X_Y :
            go_to_x_y(block.value1,block.value2,now_sprite);
            block.finished = true;
            break;
        case POINT_IN_DIRECTION :
            point_in_direction(block.value1,now_sprite);
            block.finished = true;
            break;
        case CHANGE_X_BY :
            change_x_by(block.value1,now_sprite);
            block.finished = true;
            break;
        case SET_X_TO :
            set_x_to(block.value1,now_sprite);
            block.finished = true;
            break;
        case CHANGE_Y_BY :
            change_y_by(block.value1,now_sprite);
            block.finished = true;
            break;
        case SET_Y_TO :
            set_y_to(block.value1,now_sprite);
            block.finished = true;
            break;
//        case CHANGE_SIZE_BY :
//            change_size_by(block.value1,now_sprite);
//            block.finished = true;
//            break;
//        case SET_SIZE_TO :
//            move_steps_character(block.value1,now_sprite);
//            block.finished = true;
//            break;
        case SHOW_CHARACTER :
            show_character(now_sprite);
            block.finished = true;
            break;
        case HIDE_CHARACTER :
            hide_character(now_sprite);
            block.finished = true;
            break;
        case SAY_A_FOR_T_SECONDS :
            say_a_for_t_seconds(block.message1,now_sprite);
            block.finished = true;
            break;
        case SAY_A :
            say_a(block.message1,now_sprite);
            block.finished = true;
            break;
        case THINK_A_FOR_T_SECOND :
            think_a_for_t_second(block.message1,now_sprite);
            block.finished = true;
            break;
//        case THINK_A :
//            think_a(block.message1,now_sprite);
//            block.finished = true;
//            break;
//        case SWITCH_COSTUME_TO :
//            move_steps_character(block.value1,now_sprite);
//            block.finished = true;
//            break;
        case PLAY_SOUND :
            play_sound();
            block.finished = true;
            break;
        case CHANGE_VOLUME_BY :
            change_volume_by((int)block.value1);
            block.finished = true;
            break;
        case SET_VOLUME_TO :
            set_volume_to((int)block.value1);
            block.finished = true;
            break;
        case CHANGE_FREQUENCY :
            change_frequency((int)block.value1);
            block.finished = true;
            break;
        case TIMER :
            is_timer= true;
            block.finished = true;
            break;
        case DISTANCE_TO_MOUSE_POINTER :
            move_steps_character(block.value1,now_sprite);
            block.finished = true;
            break;

    }
}
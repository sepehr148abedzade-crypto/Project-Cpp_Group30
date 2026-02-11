#include "Entity.h"


Button file_button{};
void Init_Button() {
    file_button.rect = {140, 0, 40, 48};
    file_button.first_color = {77, 151, 255, 150};
    file_button.second_color = {66, 128, 217};
    file_button.is_mouse_on = false;
}
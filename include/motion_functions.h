#ifndef MOTION_FUNCTIONS_H
#define MOTION_FUNCTIONS_H

struct Character;

void move_steps(double step,Character &sprite);
void turn_clockwise_character(double angle,Character &sprite);
void turn_counterclockwise_character(double angle, Character &sprite);
void go_to_random_position(Character &sprite);
#endif //MOTION_FUNCTIONS_H

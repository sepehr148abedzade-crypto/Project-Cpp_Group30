#ifndef MOTION_FUNCTIONS_H
#define MOTION_FUNCTIONS_H

struct Character;

void move_steps_character(double step,Character* sprite);
void turn_clockwise_character(double angle,Character* sprite);
void turn_counterclockwise_character(double angle, Character* sprite);
void go_to_random_position(Character* sprite);
void go_to_x_y(double x,double y,Character* sprite);
void point_in_direction(double angle,Character* sprite);
void change_x_by(double x,Character* sprite);
void set_x_to(double x,Character* sprite);
void change_y_by(double y,Character* sprite);
void set_y_to(double y,Character* sprite);
#endif //MOTION_FUNCTIONS_H

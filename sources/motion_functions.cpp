#include "motion_functions.h"
#include "Entity.h"
#include "cmath"
#include "constants.h"
#include "cstdlib"

void move_steps_character(double step,Character &sprite) {
    bool stop = false;
    double nextX = sprite.x + cos((sprite.degree * PI) / 180) * step;
    double nextY = sprite.y - sin((sprite.degree * PI) / 180) * step;
    double targetX = nextX;
    double targetY = nextY;
    double limitX = (double) stage.w / 2 - sprite.width / 2;
    double limitY = (double) stage.h / 2 - sprite.height / 2;

    if (targetX > limitX) sprite.x = limitX;
    else if (targetX < -limitX) sprite.x = -limitX;
    else sprite.x = targetX;

    if (targetY > limitY) sprite.y = limitY;
    else if (targetY < -limitY) sprite.y = -limitY;
    else sprite.y = targetY;
}
void turn_clockwise_character(double angle,Character &sprite){
    sprite.degree += -angle;
}

void turn_counterclockwise_character(double angle, Character &sprite){
    sprite.degree += angle;
}

void go_to_random_position(Character &sprite){
    sprite.x = rand()%(stage.w)-(double)stage.w/2;
    sprite.y = rand()%(stage.h)-(double)stage.h/2 ;
}

void go_to_x_y(double x,double y,Character &sprite){
    double targetX = x;
    double targetY =-y;

    double limitX = (double)stage.w / 2 - sprite.width / 2;
    double limitY = (double)stage.h / 2 - sprite.height / 2;

    if(targetX > limitX) sprite.x=limitX;
    else if(targetX < -limitX) sprite.x = -limitX;
    else sprite.x = targetX;

    if(targetY > limitY) sprite.y = limitY;
    else if(targetY < -limitY) sprite.y = -limitY;
    else sprite.y =targetY;
}

void point_in_direction(double angle,Character &sprite){
    sprite.degree = angle;
}

void change_x_by(double x,Character &sprite){
    double limitX = (double)stage.w / 2 - sprite.width / 2;
    sprite.x += x;
    if(sprite.x<-limitX) sprite.x = -limitX;
    if(sprite.x>limitX) sprite.x = limitX;

}

void set_x_to(double x,Character &sprite){
    double limitX = (double)stage.w / 2 - sprite.width / 2;
    if(sprite.x>limitX) sprite.x = limitX;
    else if(sprite.x<-limitX) sprite.x = - limitX;
    else sprite.x = x;
}

void change_y_by(double y,Character &sprite){
    double limitY = (double)stage.h / 2 - sprite.height / 2;
    sprite.y += -y;
    if(sprite.y<-limitY) sprite.y = limitY;
    if(sprite.y>limitY) sprite.y = -limitY;

}

void set_y_to(double y,Character &sprite){
    double limitY = (double)stage.h / 2 - sprite.height / 2;
    if(sprite.y>limitY) sprite.y = -limitY;
    else if(sprite.y<-limitY) sprite.y = limitY;
    else sprite.y = -y;
}
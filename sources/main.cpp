#include "Game.h"

int main(int argc,char* argv[]) {
    if(Loading()) return -1;
    if (!Init_Game()) return -1;
    while (!stop) {
        Get_event();
        Update();
        Render();
    }
    Clean();
}
#include "Game.h"
#include "ctime"

int main(int argc,char* argv[]) {
    srand(time(nullptr));
    if(Loading()) return -1;
    if (!Init_Game()) return -1;
    while (!stop) {
        Get_event();
        Update();
        Render();
    }
    Clean();
}
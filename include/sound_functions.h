#ifndef SOUND_FUNCTIONS_H
#define SOUND_FUNCTIONS_H
#include "SDL2/SDL.h"
extern int volume;
extern int frequency;
struct Button;

void play_sound();
void change_volume_by(int n);
void set_volume_to(int n);
void change_frequency(int n);
void Handle_event_for_run_button(SDL_Event &e,Button* button);
void Handle_event_for_volumeUp_button(SDL_Event &e,Button* button);
void Handle_event_for_volumeDown_button(SDL_Event &e,Button* button);
void Handle_event_for_increaseFrequency_button(SDL_Event &e,Button* button);
void Handle_event_for_decreaseFrequency_button(SDL_Event &e,Button* button);
#endif //SOUND_FUNCTIONS_H

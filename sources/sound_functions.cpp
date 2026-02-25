#include "sound_functions.h"
#include "Entity.h"
#include "Graphic_Element.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

int volume = 128;
int frequency = 44200;

void play_sound(){
    Mix_OpenAudio(frequency,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Chunk* Meow = Mix_LoadWAV("asset/sound/Meow.wav");
    Mix_VolumeChunk(Meow,volume);
    Mix_PlayChannel(-1,Meow,0);
}

void change_volume_by(int n){
    if(volume + n < 0) {
        volume = 0;
        return;
    }
    if(volume + n > 128 ){
        volume = 128;
        return;
    }
    volume += n;
}

void set_volume_to(int n){
    if(n<=0){
        volume=0;
        return;
    }
    if(n>=100){
        volume = 128;
    }
    volume = volume * (n/100);
}

void change_frequency(int n){
    frequency += (n * 10000);
    if(frequency<=5000){
        frequency = 5000;
        return;
    }
    if(frequency>=95000){
        frequency = 95000;
        return;
    }
}

void Handle_event_for_run_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            play_sound();
        }
    }
}
void Handle_event_for_volumeUp_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            change_volume_by(10);
        }
    }
}
void Handle_event_for_volumeDown_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            change_volume_by(-10);
        }
    }
}
void Handle_event_for_increaseFrequency_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            change_frequency(1);
        }
    }
}
void Handle_event_for_decreaseFrequency_button(SDL_Event &e,Button* button){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(e.button.button == SDL_BUTTON_LEFT){
            if(Is_mouse_on(button->rect.x,button->rect.y,button->rect.w,button->rect.h))
            change_frequency(-1);
        }
    }
}

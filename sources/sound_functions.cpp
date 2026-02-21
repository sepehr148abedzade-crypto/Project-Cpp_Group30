#include "sound_functions.h"
#include "SDL2/SDL_mixer.h"

int volume = 128;
int frequency = 44200;

void play_sound(){
    Mix_OpenAudio(100000,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Chunk* Meow = Mix_LoadWAV("asset/sound/Meow.wav");
    Mix_VolumeChunk(Meow,volume);
    Mix_PlayChannel(-1,Meow,0);
}

void change_volume_by(int n){
    if(volume < 0) {
        volume = 0;
        return;
    }
    if(volume > 128 ){
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
    if(n<=0){
        frequency = 5000;
        return;
    }
    if(n>=10){
        frequency = 95000;
        return;
    }
    frequency = (n * 10000) + 5000;
}
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <sys/types.h>
#include <wchar.h>
#include "../../src/headers/chip8.h"
#include "../../src/headers/handlers.h"
#include "../../src/headers/timers.h"


SDL_AudioSpec audio_spec = {
    44100,
    AUDIO_S16SYS,
    1, 0,
    4096
};

uint8_t prev_st = 0;

uint8_t key_map(uint8_t scancode){
    switch (scancode) {
        case 30: return 0x1;
        case 31: return 0x2;
        case 32: return 0x3;
        case 33: return 0xC;
        case 20: return 0x4;
        case 26: return 0x5;
        case 8: return 0x6;
        case 21: return 0xD;
        case 4: return 0x7;
        case 22: return 0x8;
        case 7: return 0x9;
        case 9: return 0xE;
        case 29: return 0xA;
        case 27: return 0x0;
        case 6: return 0xB;
        case 25: return 0xF;
        default: return 0xFF;
    }
}
void play_beep(SDL_AudioDeviceID dev, uint32_t frequency, uint32_t duration_ms){
    uint32_t sample_count = audio_spec.freq * duration_ms / 1000;
    int16_t samples[sample_count];

    for(int i=0; i<sample_count; i++){
        double_t t = (double)i/audio_spec.freq;
        samples[i] = (sin(2*M_PI*frequency*t) > 0) ? 8000: -8000;
    }

    SDL_QueueAudio(dev, samples, sizeof(int16_t) * sample_count);
    SDL_PauseAudioDevice(dev, false);
};

int main(int argc, char *argv[]){
    printf("pranshoe's CHIP 8 Emulator\n\n");
    if (argc < 2){
        printf("Usage: chip8_emul <rom>");
        return 0;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

    SDL_Window* window = SDL_CreateWindow(
        "pranshoe's CHIP 8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 320,
        0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    chip8_t chip8;
    chip8_reset(&chip8);
    if(chip8_load_rom(&chip8, argv[1])) return 0;

    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                running=false;
            else if(event.type == SDL_KEYDOWN){
                if(event.key.repeat == 0){
                    uint8_t key = key_map(event.key.keysym.scancode);
                    if(key!=0xFF)
                        handle_keydown(&chip8, key);
                }
                // printf("Keydown: %d\n", event.key.keysym.scancode);
            }
            else if(event.type == SDL_KEYUP){
                uint8_t key = key_map(event.key.keysym.scancode);
                if(key!=0xFF)
                    handle_keyup(&chip8, key);
            }
        }

        for(int i=0; i<10; i++)
            if(chip8_tick(&chip8))
                running = false;
        timer_tick(&chip8);

        if(chip8.sound_timer > 0){
            if(!prev_st)
                play_beep(audio_dev, 440, 100);
            prev_st = chip8.sound_timer;
        } else{
            SDL_PauseAudioDevice(audio_dev, true);
            prev_st = 0;
        }

        for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++)
            pixels[i] = chip8.display[i] ? ~0 : 0;

        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * 4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

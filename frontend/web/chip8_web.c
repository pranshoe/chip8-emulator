#include <emscripten/emscripten.h>
#include "../../src/headers/config.h"
#include "../../src/headers/chip8.h"
#include "../../src/headers/handlers.h"
#include "../../src/headers/logs.h"
#include "emscripten/em_macros.h"
#include <stdio.h>
#include <string.h>

static chip8_t chip8;

EMSCRIPTEN_KEEPALIVE
void web_chip8_init(void){
    chip8_reset(&chip8);
}

EMSCRIPTEN_KEEPALIVE
void web_chip8_tick(void){
    chip8_tick(&chip8);
}

EMSCRIPTEN_KEEPALIVE
bool web_chip8_load_rom(uint8_t* buf, int size) {
    if (size > MEMORY_SIZE - 0x200) {
        printf("ROM is too large.");
        return false;
    }
    memcpy(chip8.memory + 0x200, buf, size);
    debug_log("INFO: Loaded ROM from buffer, %x bytes\n", size);
    chip8.PC = 0x200;
    return true;
}

EMSCRIPTEN_KEEPALIVE
bool* web_chip8_display(void){
    return chip8.display;
}

EMSCRIPTEN_KEEPALIVE
void web_chip8_keyup(uint8_t key){
    handle_keyup(&chip8, key);
}

EMSCRIPTEN_KEEPALIVE
void web_chip8_keydown(uint8_t key){
    handle_keydown(&chip8, key);
}

EMSCRIPTEN_KEEPALIVE
uint8_t web_chip8_get_sound_timer(){
    return chip8.sound_timer;
}

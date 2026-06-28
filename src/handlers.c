#include "headers/handlers.h"
#include "headers/config.h"

void handle_keydown(chip8_t* chip8, uint8_t key){
    chip8->keypad[key] = 1;
}

void handle_keyup(chip8_t* chip8, uint8_t key){
    chip8->keypad[key] = 0;
}

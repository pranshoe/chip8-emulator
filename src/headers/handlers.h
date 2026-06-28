#ifndef HANDLERS_H
#define HANDLERS_H

#include "config.h"

void handle_keydown(chip8_t* chip8, uint8_t key);
void handle_keyup(chip8_t* chip8, uint8_t key);

#endif

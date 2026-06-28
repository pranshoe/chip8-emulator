#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

void chip8_reset(chip8_t* chip8);
bool chip8_tick(chip8_t* chip8);
bool chip8_load_rom(chip8_t* chip8, const char* path);

#endif

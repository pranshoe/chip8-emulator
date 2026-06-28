#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define STACK_SIZE 16
#define NUM_REGS 16
#define MEMORY_SIZE 4096
#define NUM_KEYS 16
#define BYTES_PER_FONT 5
#define FONTSET_SIZE NUM_KEYS * BYTES_PER_FONT

typedef struct chip8_t{
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[NUM_REGS];
    uint16_t I;
    uint16_t PC;
    uint16_t stack[STACK_SIZE];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool display[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool keypad[NUM_KEYS];
} chip8_t;

extern uint8_t FONTSET[FONTSET_SIZE];

#endif

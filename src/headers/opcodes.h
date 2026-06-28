#ifndef OPCODES_H
#define OPCODES_H
#include "config.h"
#include <stdint.h>

typedef struct opcode_t{
    uint8_t nibble1;
    uint16_t NNN;
    uint8_t NN;
    uint8_t N;
    uint8_t X;
    uint8_t Y;
} opcode_t;

bool cls_ret(chip8_t* chip8, opcode_t* opcode);
void jmp(chip8_t* chip8, opcode_t* opcode);
void call(chip8_t* chip8, opcode_t* opcode);
void skip(chip8_t* chip8, opcode_t* opcode);
void load(chip8_t* chip8, opcode_t* opcode);
void add(chip8_t* chip8, opcode_t* opcode);
void or(chip8_t* chip8, opcode_t* opcode);
void and(chip8_t* chip8, opcode_t* opcode);
void xor(chip8_t* chip8, opcode_t* opcode);
void sub(chip8_t* chip8, opcode_t* opcode);
void shift(chip8_t* chip8, opcode_t* opcode);
void rnd(chip8_t* chip8, opcode_t* opcode);
void draw(chip8_t* chip8, opcode_t* opcode);
void keyOP(chip8_t* chip8, opcode_t* opcode);
void bcd(chip8_t* chip8, opcode_t* opcode);
void str(chip8_t* chip8, opcode_t* opcode);

#endif

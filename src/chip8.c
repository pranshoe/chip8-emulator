#include "headers/chip8.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/logs.h"
#include "headers/opcodes.h"
#include "headers/timers.h"

uint16_t fetch(chip8_t* chip8){
    uint8_t higher_opcode = chip8->memory[chip8->PC++];
    uint8_t lower_opcode = chip8->memory[chip8->PC++];
    uint16_t opcode = (higher_opcode << 8) | lower_opcode;
    return opcode;
}

bool execute(chip8_t* chip8, uint16_t raw_opcode){
    // debug_log("PC: 0x%x\tOP: 0x%x\t", chip8->PC-2, raw_opcode);
    opcode_t opcode;
    opcode.nibble1 = (raw_opcode >> 12) & 0xF;
    opcode.X = (raw_opcode >> 8) & 0xF;
    opcode.Y = (raw_opcode >> 4) & 0xF;
    opcode.NNN = raw_opcode & 0x0FFF;
    opcode.NN = raw_opcode & 0x00FF;
    opcode.N = raw_opcode & 0xF;

    switch (opcode.nibble1){
        case 0x0: {
            if(cls_ret(chip8, &opcode))
                goto unrec_op;
            break;
        }
        case 0x1: {
            jmp(chip8, &opcode);
            break;
        }
        case 0x2: {
            call(chip8, &opcode);
            break;
        }
        case 0x3:
        case 0x4:
        case 0x5: {
            skip(chip8, &opcode);
            break;
        }
        case 0x6: {
            load(chip8, &opcode);
            break;
        }
        case 0x7:{
            add(chip8, &opcode);
            break;
        }
        case 0x8: {
            switch(opcode.N){
                case 0x0: {
                    load(chip8, &opcode);
                    break;
                }
                case 0x1: {
                    or(chip8, &opcode);
                    break;
                }
                case 0x2:{
                    and(chip8, &opcode);
                    break;
                }
                case 0x3:{
                    xor(chip8, &opcode);
                    break;
                }
                case 0x4:{
                    add(chip8, &opcode);
                    break;
                }
                case 0x5:
                case 0x7:{
                    sub(chip8, &opcode);
                    break;
                }
                case 0x6:
                case 0xE:{
                    shift(chip8, &opcode);
                    break;
                }
            }
            break;
        }
        case 0x9:{
            skip(chip8, &opcode);
            break;
        }
        case 0xA:{
            load(chip8, &opcode);
            break;
        }
        case 0xB:{
            jmp(chip8, &opcode);
            break;
        }
        case 0xC:{
            rnd(chip8, &opcode);
            break;
        }
        case 0xD: {
            draw(chip8, &opcode);
            break;
        }
        case 0xE:{
            keyOP(chip8, &opcode);
            break;
        }
        case 0xF:{
            switch (opcode.NN) {
                case 0x07:
                case 0x15:
                case 0x18:
                case 0x29:{
                    load(chip8, &opcode);
                    break;
                }
                case 0x0A:{
                    keyOP(chip8, &opcode);
                    break;
                }
                case 0x33:{
                    bcd(chip8, &opcode);
                    break;
                }
                case 0x1E:{
                    add(chip8, &opcode);
                    break;
                }
                case 0x55:{
                    str(chip8, &opcode);
                    break;
                }
                case 0x65:{
                    load(chip8, &opcode);
                    break;
                }
            }
            break;
        }
        default:{
            goto unrec_op;
        }
    }

    return 0;
    unrec_op:
    // debug_log("ERROR: Unrecognized OPCODE %x\n", raw_opcode);
    return 1;
}

void chip8_reset(chip8_t* chip8){
    memset(chip8, 0, sizeof(chip8_t));
    chip8->PC = 0x200;
    memcpy(chip8->memory, FONTSET, FONTSET_SIZE);
    debug_log("INFO: CHIP8 Intialized.\n");
}

bool chip8_tick(chip8_t* chip8){
    uint16_t opcode = fetch(chip8);
    if(execute(chip8, opcode)){
        printf("ERROR: Failed to execute opcode\n");
        return 1;
    }
    timer_tick(chip8);
    return 0;
}

bool chip8_load_rom(chip8_t* chip8, const char* path){
    FILE *rom = fopen(path, "rb");
    if(rom == NULL){
        printf("Cannot open %s.", path);
        return 1;
    }

    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    if (rom_size > MEMORY_SIZE - 0x200){
        printf("ROM is too large.");
        return 1;
    }

    rewind(rom);
    size_t bytes_read = fread(chip8->memory + 0x200, 1, rom_size, rom);
    if (bytes_read != rom_size){
        printf("Cannot read ROM.");
        fclose(rom);
        return 1;
    }

    fclose(rom);
    debug_log("INFO: Loaded file %s, %x bytes\n", path, rom_size);
    return 0;
}

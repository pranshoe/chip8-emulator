#include "headers/opcodes.h"
#include "headers/config.h"
#include "headers/logs.h"
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

bool cls_ret(chip8_t* chip8, opcode_t* opcode){
    switch(opcode->NN){
        case 0xE0: {
            memset(chip8->display, 0, SCREEN_HEIGHT * SCREEN_WIDTH);
            debug_log("CLS\n");
            return 0;
        }
        case 0xEE: {
            chip8->sp--;
            chip8->PC = chip8->stack[chip8->sp];
            debug_log("RET\n");
            return 0;
        }
    }

    return 1;
}

void jmp(chip8_t* chip8, opcode_t* opcode){
    if (opcode->nibble1 == 0x1) {
        chip8->PC = opcode->NNN;
        debug_log("JMP\n");
    } else if(opcode->nibble1 == 0xB){
        chip8->PC = chip8->V[0x0] + opcode->NNN;
        debug_log("JMP V0, 0x%x\t\t\tV0: 0x%x", opcode->NNN, chip8->V[0x0]);
    }
    return;
}

void call(chip8_t* chip8, opcode_t* opcode){
    chip8->stack[chip8->sp++] = chip8->PC;
    chip8->PC = opcode->NNN;
    debug_log("CALL 0x%x\n", chip8->PC);
    return;
}

void skip(chip8_t* chip8, opcode_t* opcode){
    switch(opcode->nibble1){
        case 0x3:{
            if(chip8->V[opcode->X] == opcode->NN)
                chip8->PC +=2;
            debug_log("SE V%x, 0x%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->NN, opcode->X, chip8->V[opcode->X]);
            break;
        }
        case 0x4:{
            if(chip8->V[opcode->X] != opcode->NN)
                chip8->PC +=2;
            debug_log("SNE V%x, 0x%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->NN, opcode->X, chip8->V[opcode->X]);
            break;
        }
        case 0x5:{
            if(chip8->V[opcode->X] == chip8->V[opcode->Y])
                chip8->PC +=2;
            debug_log("SE V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
            break;
        }
        case 0x9:{
            if(chip8->V[opcode->X] != chip8->V[opcode->Y])
                chip8->PC += 2;
            debug_log("SNE V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
            break;
        }
    }
    return;
}

void load(chip8_t* chip8, opcode_t* opcode){
    if(opcode->nibble1 == 0x6){
        chip8->V[opcode->X] = opcode->NN;
        debug_log("LD V%x, 0x%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->NN, opcode->X, chip8->V[opcode->X]);

    } else if(opcode->nibble1 == 0x8){
        chip8->V[opcode->X] = chip8->V[opcode->Y];
        debug_log("LD V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
    } else if(opcode->nibble1 == 0xA){
        chip8->I = opcode->NNN;
        debug_log("LD I, 0x%x\t\t\tI: 0x%x\n", opcode->NNN, opcode->NNN);
    } else if(opcode->nibble1 == 0xF){
        if(opcode->N == 0x7){
            chip8->V[opcode->X] = chip8->delay_timer;
            debug_log("LD V%x, delay_timer\t\t\tV%x: 0x%x\n", opcode->X, opcode->X, chip8->V[opcode->X]);
        } else if(opcode->NN == 0x15){
            chip8->delay_timer = chip8->V[opcode->X];
            debug_log("LD delay_timer, V%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->X, chip8->V[opcode->X]);
        } else if(opcode->NN == 0x18){
            chip8->sound_timer = chip8->V[opcode->X];
            debug_log("LD sound_timer, V%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->X, chip8->V[opcode->X]);
        } else if(opcode->NN == 0x29){
            chip8->I = 0x050 + (chip8->V[opcode->X] & 0x0F) * BYTES_PER_FONT;
            debug_log("LD I, sprite %x\t\t\tI: 0x%x\n", chip8->V[opcode->X], chip8->I);
        } else if(opcode->NN == 0x65){
            for(int i=0; i<opcode->X + 1; i++){
                chip8->V[i] = chip8->memory[chip8->I + i];
            }
            debug_log("LD V%x\t\t\tI: 0x%x", opcode->X, chip8->I);
        }
    }

    return;
}

void add(chip8_t* chip8, opcode_t* opcode){
    if(opcode->nibble1 == 0x7){
        chip8->V[opcode->X] += opcode->NN;
        debug_log("ADD V%x, 0x%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->NN, opcode->X, chip8->V[opcode->X]);
    } else if(opcode->nibble1 == 0x8){
        uint32_t sum = chip8->V[opcode->X] + chip8->V[opcode->Y];
        chip8->V[0xF] = (sum > 255);
        chip8->V[opcode->X] = sum & 0xFF;
        debug_log("ADD V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\tVF: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y], chip8->V[0xF]);
    } else if(opcode->nibble1 == 0xF){
        chip8->I += chip8->V[opcode->X];
        debug_log("ADD I, V%x\t\t\tI: 0x%x\tV%x: 0x%x\t\n", opcode->X, chip8->I, opcode->X, chip8->V[opcode->X]);
    }
    return;
}

void or(chip8_t* chip8, opcode_t* opcode){
    chip8->V[opcode->X] |= chip8->V[opcode->Y];
    debug_log("OR V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
    return;
}

void and(chip8_t* chip8, opcode_t* opcode){
    chip8->V[opcode->X] &= chip8->V[opcode->Y];
    debug_log("AND V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
    return;
}

void xor(chip8_t* chip8, opcode_t* opcode){
    chip8->V[opcode->X] ^= chip8->V[opcode->Y];
    debug_log("XOR V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
    return;
}

void sub(chip8_t* chip8, opcode_t* opcode){
    if(opcode->N == 0x5){
        chip8->V[0xF] = (chip8->V[opcode->X] >= chip8->V[opcode->Y]);
        chip8->V[opcode->X] -= chip8->V[opcode->Y];
        debug_log("SUB V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\tVF: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y], chip8->V[0xF]);
    } else if(opcode->N == 0x7){
        chip8->V[0xF] = (chip8->V[opcode->Y] >= chip8->V[opcode->X]);
        chip8->V[opcode->X] = chip8->V[opcode->Y] - chip8->V[opcode->X];
        debug_log("SUBN V%x, V%x\t\t\tV%x: 0x%x\tV%x: 0x%x\tVF: 0x%x\n", opcode->X, opcode->Y, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y], chip8->V[0xF]);
    }
    return;
}

void shift(chip8_t* chip8, opcode_t* opcode){
    if(opcode->N == 0x6){
        chip8->V[0xF] = chip8->V[opcode->Y] & 1;
        chip8->V[opcode->X] = chip8->V[opcode->Y] >> 1;
        debug_log("SHR V%x\t\t\tV%x: 0x%x\tVF: 0x%x\n", opcode->X, opcode->X, chip8->V[opcode->X], chip8->V[0xF]);
    } else if(opcode->N == 0xE){
        chip8->V[0xF] = (chip8->V[opcode->Y] >> 7) & 1;
        chip8->V[opcode->X] = chip8->V[opcode->Y] << 1;
        debug_log("SHL V%x\t\t\tV%x: 0x%x\tVF: 0x%x\n", opcode->X, opcode->X, chip8->V[opcode->X], chip8->V[0xF]);
    }
}

void rnd(chip8_t* chip8, opcode_t* opcode){
    chip8->V[opcode->X] = (rand() % 256) & opcode->NN;
    debug_log("RND V%x, 0x%x\t\t\tV%x: 0x%x\n", opcode->X, opcode->NN, opcode->X, chip8->V[opcode->X]);
    return;
}

void draw(chip8_t* chip8, opcode_t* opcode){
    uint8_t x = chip8->V[opcode->X] % SCREEN_WIDTH;
    uint8_t y = chip8->V[opcode->Y] % SCREEN_HEIGHT;

    chip8->V[0xF] = 0;

    for (int row=0; row<opcode->N; row++){
        uint8_t sprite_byte = chip8->memory[chip8->I + row];

        for(int col=0; col<8; col++){
            if(sprite_byte & 1<<(7-col)){
                uint8_t px = (x + col) % SCREEN_WIDTH;
                uint8_t py = (y + row) % SCREEN_HEIGHT;
                uint16_t index = py * SCREEN_WIDTH + px;

                chip8->V[0xF] = chip8->display[index] ? 1 : chip8->V[0xF];
                chip8->display[index] ^= 1;
            }
        }
    }

    debug_log("DRAW V%x, V%x, 0x%x\t\tV%x: 0x%x\tV%x: 0x%x\n", opcode->X, opcode->Y, opcode->N, opcode->X, chip8->V[opcode->X], opcode->Y, chip8->V[opcode->Y]);
}

void keyOP(chip8_t* chip8, opcode_t* opcode){
    if(opcode->nibble1 == 0xE){
        if(opcode->NN == 0x9E && chip8->keypad[chip8->V[opcode->X]] == 1){
            chip8->PC += 2;
            //COMMENTS
        }
        else if (opcode->NN == 0xA1 && chip8->keypad[chip8->V[opcode->X]] == 0)
            chip8->PC += 2;
    }
    else if(opcode->nibble1 == 0xF){
        bool key_found = false;

        for (int i=0; i<NUM_KEYS; i++){
            if(chip8->keypad[i]){
                key_found=true;
                chip8->V[opcode->X] = i;
                break;
            }
        }

        if(!key_found)
            chip8->PC -= 2;
    }
}

void bcd(chip8_t* chip8, opcode_t* opcode){
    chip8->memory[chip8->I] = chip8->V[opcode->X] / 100;
    chip8->memory[chip8->I + 1] = (chip8->V[opcode->X] / 10) % 10;
    chip8->memory[chip8->I + 2] = chip8->V[opcode->X] % 10;
}

void str(chip8_t* chip8, opcode_t* opcode){
    for(int i=0; i<opcode->X + 1; i++){
        chip8->memory[chip8->I + i] = chip8->V[i];
    }
    debug_log("STR V%x\t\t\tI: 0x%x", opcode->X, chip8->I);
}

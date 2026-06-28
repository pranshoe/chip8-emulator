#include "headers/timers.h"
#include "stdint.h"

static uint8_t cycle_counter =0;

void timer_tick(chip8_t* chip8){
    cycle_counter += 1;

    if (cycle_counter >= CYCLES_PER_TICK){
        cycle_counter = 0;

        if (chip8->delay_timer > 0)
            chip8->delay_timer -= 1;

        if (chip8->sound_timer > 0)
            chip8->sound_timer -= 1;
    }
}

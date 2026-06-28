#ifndef TIMERS_H
#define TIMERS_H

#include "config.h"
#define CPU_FREQ 700
#define TIMER_FREQ 60
#define CYCLES_PER_TICK (CPU_FREQ/TIMER_FREQ)

void timer_tick(chip8_t* chip8);

#endif

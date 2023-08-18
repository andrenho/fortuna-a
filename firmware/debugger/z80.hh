#ifndef Z80_HH_
#define Z80_HH_

#include <stdint.h>

#include "bus.hh"

namespace z80 {

struct StepCycleStatus {
    uint8_t  data;
    uint16_t addr;
    bool     m1    : 1;
    bool     iorq  : 1;
    bool     busak : 1;
    bool     wait  : 1;
    bool     int_  : 1;
    MemPins  mem_pins;
};

struct StepStatus {
    uint16_t af, bc, de, hl, afx, bcx, dex, hlx, ix, iy, sp, pc;
    uint16_t stack[8];
};

bool is_present();
void release_bus();

void            reset();
void            clk();
StepCycleStatus step_cycle();
uint16_t        step();
StepStatus      step_nmi();
uint16_t        debug_run();

}

#endif

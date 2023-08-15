#ifndef Z80_HH_
#define Z80_HH_

#include <stdint.h>

#include "bus.hh"

namespace z80 {

struct StepStatus {
    uint8_t  data;
    uint16_t addr;
    bool     m1    : 1;
    bool     iorq  : 1;
    bool     busak : 1;
    bool     wait  : 1;
    bool     int_  : 1;
    MemPins  mem_pins;
};

bool is_present();
void release_bus();

void       reset();
void       clk();
StepStatus step();

}

#endif

#include "memory.hh"

#include "bus.hh"

namespace memory {

void set(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000) {   // ROM
        bus::set_rom_we(1);
        bus::set_mem({ 1, 1, 0 });
    } else {
        bus::set_mem({ 0, 1, 0 });
    }
}

uint8_t get(uint16_t addr)
{
}

}

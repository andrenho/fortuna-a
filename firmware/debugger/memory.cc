#include "memory.hh"

#include "bus.hh"

namespace memory {

void set(uint16_t addr, uint8_t data)
{
    bus::set_addr(addr);
    bus::set_data(data);

    if (addr < 0x2000)
        bus::set_rom_we(0);
    bus::set_mem({ 0, 1, 0 });

    // TODO - rest of the process
}

uint8_t get(uint16_t addr)
{
    bus::set_addr(addr);
    bus::set_mem({ 1, 0, 0 });

    return 0;  // TODO - rest of the process
}

void set_memory_state(uint8_t high_addr, bool ramonly)
{
    uint8_t data = high_addr | (ramonly ? 0b1000 : 0);
    bus::set_data(data);
    bus::pulse_y0w();
    bus::release_data();
}

}

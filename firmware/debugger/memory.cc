#include "memory.hh"

#include <avr/cpufunc.h>
#include <util/delay.h>

#include "bus.hh"
#include "z80.hh"

namespace memory {

bool set(uint16_t addr, uint8_t data)
{
    // don't set if data is already there
    uint8_t current_data = get(addr);
    if (data == current_data)
        return true;

    bool write_to_rom = addr < 0x2000 && !bus::get_ramonly();

    // set pinout
    bus::set_addr(addr);
    bus::set_data(data);
    bus::set_mem({ 0, 1, 0 });
    _NOP();

    // if writing to ROM, set additional pinout and wait
    if (write_to_rom) {
        bus::set_rom_we(0);
        _delay_us(10000);
    }

    // release pins
    bus::release_addr();
    bus::release_mem();
    bus::set_data(~data);  // mess up DATA
    bus::release_data();

    // if writing to ROM, wait until data has been written
    if (write_to_rom) {
        for (int i = 0; i < 200; ++i) {
            uint8_t read_data = get(addr);
            if (read_data == data)
                goto ok;
            _delay_us(10000);
        }
        return false;
    }

ok:
    return true;
}

uint8_t get(uint16_t addr)
{
    z80::release_bus();

    bus::set_addr(addr);
    bus::set_mem({ 1, 0, 0 });
    _NOP();

    uint8_t data = bus::get_data();

    bus::release_addr();
    bus::release_mem();

    return data;
}

void set_memory_state(uint8_t high_addr, bool ramonly)
{
    uint8_t data = high_addr | (ramonly ? 0b1000 : 0);
    bus::set_data(data);
    bus::pulse_y0w();
    bus::release_data();
}

}

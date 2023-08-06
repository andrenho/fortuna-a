#ifndef BUS_HH_
#define BUS_HH_

#include <cstdint>

struct MemPins {
    bool wr : 1;
    bool rd : 1;
    bool mreq : 1;
};

namespace bus {

void     init();

void     set_rom_we(bool v);
void     set_nmi(bool v);
void     set_clk_ena(bool v);
void     set_busrq(bool v);
void     set_rst(bool v);

void     clk_pulse();

MemPins  get_wr();
void     set_wr(MemPins mem);
void     release_mempins();

void     set_y0w(bool v);
void     release_y0w();

uint8_t  get_data();
void     set_data(uint8_t data);
void     release_data();

uint16_t get_addr();
void     set_addr(uint16_t addr);
void     release_addr();

uint8_t  get_addr_high();
bool     get_ramonly();

}

#endif

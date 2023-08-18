#include "z80.hh"

#include <stddef.h>

#include "memory.hh"

namespace z80 {

bool is_present()
{
    return true;  // TODO
}

void release_bus()
{
    if (bus::get_busak() == 0)
        return;

    bus::set_busrq(0);
    bus::pulse_clk();
    while (bus::get_busak() != 0)
        bus::pulse_clk();
    bus::set_busrq(1);
}

void reset()
{
    bus::set_rst(0);
    for (size_t i = 0; i < 50; ++i)
        bus::pulse_clk();
    bus::set_rst(1);
}

void clk()
{
    bus::pulse_clk();
}

StepCycleStatus step_cycle()
{
    bus::pulse_clk();
    return {
        bus::get_data(),
        bus::get_addr(),
        bus::get_m1(),
        bus::get_iorq(),
        bus::get_busak(),
        bus::get_wait(),
        bus::get_int(),
        bus::get_mem()
    };
}

uint16_t step()
{
    bus::set_busrq(1);  // make sure we're not requesting the bus
    bus::pulse_clk();

    while (bus::get_m1() != 0)
        bus::pulse_clk();

    bus::pulse_clk();

    static uint8_t previous_instruction = 0x00;
    bool combined_instruction = (previous_instruction == 0xcb || previous_instruction == 0xdd || previous_instruction == 0xed || previous_instruction == 0xfd);
    previous_instruction = bus::get_data();
    if (combined_instruction)
        step();

    return bus::get_addr();
}

StepStatus step_nmi()
{
    // activate NMI
    bus::set_nmi(0);
    step();
    step();
    bus::set_nmi(1);

    // execute NMI subroutine
    while (bus::get_data() != 0xc9)  // run until 'ret'
        step();

    // get registers
    auto ram = [](uint16_t pos) {
        return ((uint16_t) memory::get(pos + 1) << 8) | memory::get(pos);
    };

    uint16_t sp = ram(0x2014);
    StepStatus ss = {
            ram(0x2000), ram(0x2002), ram(0x2004), ram(0x2006), ram(0x2008), ram(0x200a), ram(0x200c), ram(0x200e),
            ram(0x2010), ram(0x2012), sp, 0, {0}
    };

    // get stack
    for (size_t i = 0; i < 8; ++i)
        ss.stack[i] = ram(sp + ((i + 1) * 2));

    bus::set_busrq(1);   // regain control of the bus

    // return from NMI
    step();
    ss.pc = bus::get_addr();
    return ss;
}

static bool is_breakpoint(uint16_t addr)
{
    return false;
}

uint16_t debug_run() {
    uint16_t pc;

    do {
        pc = step();
    } while (!is_breakpoint(pc));

    return pc;
}

}

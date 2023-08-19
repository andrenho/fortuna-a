#include "z80.hh"

#include <stddef.h>

#include "memory.hh"

namespace z80 {

static uint16_t bkps[MAX_BKP];
static uint16_t current_pc = 0;

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
    for (size_t i = 0; i < MAX_BKP; ++i)
        bkps[i] = NO_BKP;
    bus::set_rst(1);
    current_pc = 0;
}

void clk()
{
    bus::pulse_clk();
}

StepCycleStatus step_cycle()
{
    bus::pulse_clk();
    if (bus::get_m1() == 0)
        current_pc = bus::get_addr();
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

    current_pc = bus::get_addr();
    return current_pc;
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
            ram(0x2010), ram(0x2012), sp, 0, {0}, true
    };

    // get stack
    for (size_t i = 0; i < 8; ++i)
        ss.stack[i] = ram(sp + ((i + 1) * 2));

    bus::set_busrq(1);   // regain control of the bus

    // return from NMI
    step();
    ss.pc = bus::get_addr();
    current_pc = ss.pc;
    return ss;
}

static int8_t next_instruction_subroutine_size()  // return call/rst instruction size for next instruction, or -1 if not call/rst
{
    static const uint8_t CALL_OPS[] = { 0xC4, 0xCC, 0xCD, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC };  // TODO - move this to PROGMEM
    static const uint8_t RST_OPS[] = { 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF };

    uint8_t op = memory::get(current_pc);
    for (size_t i = 0; i < sizeof(CALL_OPS); ++i)
        if (op == CALL_OPS[i])
            return 3;
    for (size_t i = 0; i < sizeof(RST_OPS); ++i)
        if (op == RST_OPS[i])
            return 1;

    return -1;
}

static bool is_breakpoint(uint16_t addr)
{
    for (size_t i = 0; i < MAX_BKP; ++i)
        if (bkps[i] != NO_BKP && addr == bkps[i])
            return true;
    return false;
}

StepStatus next()
{
    int8_t sz = next_instruction_subroutine_size();
    if (sz != -1) {
        uint16_t bkp = current_pc + sz;
        if (!is_breakpoint(bkp))
            bkp_swap(bkp);
        uint16_t pc = debug_run();
        current_pc = pc;
        bkp_swap(bkp);

        StepStatus ss;
        ss.pc = pc;
        ss.has_info = false;
        return ss;
    } else {
        return step_nmi();
    }
}

uint16_t debug_run() {
    uint16_t pc;

    do {
        pc = step();
    } while (!is_breakpoint(pc));

    current_pc = pc;
    return pc;
}

void bkp_swap(uint16_t bkp)
{
    for (size_t i = 0; i < MAX_BKP; ++i) {
        if (bkps[i] == bkp) {
            bkps[i] = NO_BKP;
            return;
        }
    }

    for (size_t i = 0; i < MAX_BKP; ++i) {
        if (bkps[i] == NO_BKP) {
            bkps[i] = bkp;
            return;
        }
    }
}

uint16_t const* bkp_list()
{
    return bkps;
}

}

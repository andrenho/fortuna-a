#include "post.hh"

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <avr/pgmspace.h>

#include "memory.hh"
#include "random.hh"

namespace post {

char error[256] = {0};

static bool test_memory(uint16_t min_memory, uint16_t max_memory, bool write)
{
    const size_t N_ADDR = 32;
    const size_t N_TESTS = 16;

    uint16_t addr[N_ADDR];
    uint8_t data[N_ADDR];

    for (size_t i = 0; i < N_ADDR; ++i) {
        do {
            addr[i] = random::nextw();
        } while (addr[i] < min_memory || addr[i] > max_memory);
    }

    for (size_t i = 0; i < N_ADDR; ++i) {
        if (write) {
            data[i] = random::nextb();
            if (!memory::set(addr[i], data[i])) {
                snprintf_P(error, sizeof error, PSTR("write [%04X] = %02X item %d"), addr[i], data[i], i);
                return false;
            }
        } else {
            data[i] = memory::get(addr[i]);
        }
    }

    for (size_t i = 0; i < N_TESTS; ++i) {
        for (size_t j = 0; j < N_ADDR; ++j) {
            uint8_t value = memory::get(addr[j]);
            if (value != data[j]) {
                snprintf_P(error, sizeof error, PSTR("verify [%04X] = %02X attempt %d item %d"), addr[j], data[j], i, j);
                return false;
            }
        }
    }

    return true;
}

bool read_rom_memory()
{
    return test_memory(0x0, 0x1fff, false);
}

bool read_shared_memory()
{
    return test_memory(0x2000, 0x3fff, false);
}

bool read_high_memory()
{
    return test_memory(0x4000, 0xffff, false);
}

bool write_rom_memory()
{
    return test_memory(0x0, 0x1fff, true);
}

bool write_shared_memory()
{
    return test_memory(0x2000, 0x3fff, true);
}

bool write_high_memory()
{
    return test_memory(0x4000, 0xffff, true);
}

}

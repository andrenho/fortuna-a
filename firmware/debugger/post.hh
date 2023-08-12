#ifndef POST_HH_
#define POST_HH_

#include <stdint.h>
#include <stddef.h>

namespace post {

bool read_rom_memory();
bool read_shared_memory();
bool read_high_memory();

bool write_rom_memory();
bool write_shared_memory();
bool write_high_memory();

bool write_memory_banks();
bool write_ramonly();

size_t run_tests(uint32_t* results);

}

#endif

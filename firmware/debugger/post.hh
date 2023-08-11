#ifndef POST_HH_
#define POST_HH_

namespace post {

extern char error[];

bool read_rom_memory();
bool read_shared_memory();
bool read_high_memory();

bool write_rom_memory();
bool write_shared_memory();
bool write_high_memory();

bool write_memory_banks();
bool write_ramonly();

}

#endif

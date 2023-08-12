#include "comm.hh"

#include <stdio.h>

#include <avr/pgmspace.h>

#include "buffer.hh"
#include "memory.hh"
#include "post.hh"

#define NO_VALUE 0x80000000

namespace comm {

static bool input_buffer()
{
    int i = 0;

    for (;;) {
        char c = getchar();
        if (c == 10 || c == 13) {
            buffer::input[i++] = 0;
            putchar('\n');
            return true;
        } else if ((c == 8 || c == 127)) {
            if (i > 0) {
                --i;
                printf_P(PSTR("\b \b"));
            }
            continue;
        }
        buffer::input[i++] = c;
        putchar(c);
        if (i > INPUT_SZ)
            return false;
    }
}

static uint32_t next_value(size_t* i)
{
    if (buffer::input[*i] == 0)
        return NO_VALUE;

    int value = 0;

    for (;;) {
        char c = buffer::input[(*i)++];
        if (c == 0 || c == 32) {
            if (c == 0)
                --(*i); // keep index on top of EOS
            return value;
        }
        value <<= 4;
        if (c >= '0' && c <= '9')
            value |= (c - '0');
        else if (c >= 'A' && c <= 'F')
            value |= (c - 'A' + 0xa);
        else if (c >= 'a' && c <= 'f')
            value |= (c - 'a' + 0xa);
        else
            return NO_VALUE;
    }
}

static bool post_tests()
{
    uint32_t results;
    size_t n_tests = post::run_tests(&results);
    putchar(results == 0 ? '+' : '-');
    putchar(' ');

    for (size_t i = 0; i < n_tests; ++i) {
        putchar(((results >> (n_tests - i - 1)) & 1) ? '-' : '+');
        putchar(i + 'a');
        putchar(' ');
    }

    putchar('\n');

    return true;
}

static bool read_memory(size_t i)
{
    uint32_t addr = next_value(&i);
    if (addr == NO_VALUE)
       return false;

    uint32_t count = next_value(&i);
    if (count == NO_VALUE)
        return false;

    printf_P(PSTR("+ %X"), count);

    for (size_t j = 0; j < count; ++j)
        printf_P(PSTR(" %02X"), memory::get(addr + j));
    putchar('\n');

    return true;
}

static bool parse_input()
{
    size_t i = 0;
    while (buffer::input[i] == ' ') ++i;  // trim left

    if (buffer::input[i+1] != ' ' && buffer::input[i+1] != 0)
        return false;

    switch (buffer::input[i]) {
        case 'A':
            puts_P(PSTR("+"));
            return true;
        case 'P':
            return post_tests();
        case 'R':
            return read_memory(i + 2);
    }

    return false;
}

void listen()
{
    bool ok = input_buffer();
    if (!ok) {
        puts_P(PSTR("x V"));
    } else {
        if (!parse_input())
            puts_P(PSTR("x ?"));
    }
}

}

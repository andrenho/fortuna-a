#include "comm.hh"

#include <stdio.h>

#include <avr/pgmspace.h>

#include "buffer.hh"
#include "post.hh"

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

static void post_tests()
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
            post_tests();
            return true;
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

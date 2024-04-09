#include <stdio.h>

#include <pico/stdlib.h>
#include <hardware/pio.h>

#include "io.pio.h"

#define BUTTON_PIN  0
#define LED_PIN     1
#define DATA0_PIN  15

int main()
{
    PIO pio = pio0;

    uint pio_memory_offset = pio_add_program(pio, &io_program);
    uint sm = pio_claim_unused_sm(pio, true);

    pio_sm_config c = io_program_get_default_config(pio_memory_offset);

    // LED (WAIT)
    pio_gpio_init(pio, LED_PIN);
    sm_config_set_set_pins(&c, LED_PIN, 1);
    pio_sm_set_consecutive_pindirs(pio, sm, LED_PIN, 1, true);

    // DATA
    for (int i = 0; i < 8; ++i)
        pio_gpio_init(pio, DATA0_PIN + i);
    sm_config_set_set_pins(&c, DATA0_PIN, 4);
    sm_config_set_set_pins(&c, DATA0_PIN + 4, 4);
    pio_sm_set_consecutive_pindirs(pio, sm, DATA0_PIN, 8, true);

    // BUTTON (I/O)
    pio_gpio_init(pio, BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm, BUTTON_PIN, 1, false);

    pio_sm_init(pio, sm, pio_memory_offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    while (true) {
        sleep_ms(1000);
    }
}

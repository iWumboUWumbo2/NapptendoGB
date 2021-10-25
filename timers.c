#include "timers.h"

const uint16_t CLOCK_FREQS[] = {1024, 16, 64, 256};

void timer_reset(timergb_t* timer) {
    timer->timer_counter = CLOCKSPEED/CLOCK_FREQS[0];
    timer->divider_counter = 0;
}

uint8_t timer_is_enabled(timergb_t* timer) {
    return bus_read(timer->bus, TAC) & 0x4 ? 1 : 0;
}

uint8_t timer_get_clock_freq(timergb_t* timer) {
    return bus_read(timer->bus, TAC) & 0x3;
}

void timer_set_clock_freq(timergb_t* timer) {
    uint8_t clockfreq = timer_get_clock_freq(timer);
    timer->timer_counter = CLOCK_FREQS[clockfreq];
}

void timer_divider_reg(timergb_t* timer, uint8_t cycles) {
    if (timer->bus->cpu->stopped) {
        timer->bus->rom[DIV] = 0;
        return;
    }

    timer->divider_counter += cycles;

    if (timer->divider_counter >= 255) {
        timer->divider_counter = 0;
        timer->bus->rom[DIV]++;
    }
}

void timer_counter_reg(timergb_t* timer, uint8_t cycles) {
    timer->timer_counter -= cycles;

    if (timer->timer_counter <= 0) {
        timer_set_clock_freq(timer);

        uint8_t tima = bus_read(timer->bus, TIMA);
        if (tima == 0xFF) {
            bus_write(timer->bus, TIMA, bus_read(timer->bus, TMA));
            interrupt_req(timer->bus->interrupt, 2);
        } else {
            bus_write(timer->bus, TIMA, tima + 1);
        }
    }
}

void timer_update(timergb_t* timer) {
    timer_divider_reg(timer, timer->bus->cpu->cycles);
    if (timer_is_enabled(timer)) {
        timer_counter_reg(timer, timer->bus->cpu->cycles);
    }
}

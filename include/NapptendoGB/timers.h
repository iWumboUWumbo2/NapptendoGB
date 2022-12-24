#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include "bus.h"

#define TIMA    0xFF05
#define TMA     0xFF06
#define TAC     0xFF07
#define DIV     0xFF04

#define CLOCKSPEED  4194304

typedef struct timer timergb_t;
typedef struct bus bus_t;

struct timer {
    int32_t timer_counter;
    bus_t* bus;
    uint16_t divider_counter;
};

void timer_reset(timergb_t* timer);
void timer_update(timergb_t* timer);
uint8_t timer_is_enabled(timergb_t* timer);
void timer_counter_reg(timergb_t* timer, uint8_t cycles);
uint8_t timer_get_clock_freq(timergb_t* timer);
void timer_set_clock_freq(timergb_t* timer);
void timer_divider_reg(timergb_t* timer, uint8_t cycles);

#endif //TIMERS_H

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"
#include "bus.h"

#define IE  0xFFFF
#define IF  0xFF0F

struct interrupt {
    uint8_t IME;
    bus_t* bus;
};

void interrupt_set_IME(interrupt_t* interrupt, uint8_t value);
void interrupt_req(interrupt_t* interrupt, uint8_t id);
void interrupt_perform(interrupt_t* interrupt);
void interrupt_service(interrupt_t* interrupt, int type);

#endif //INTERRUPTS_H

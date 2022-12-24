#include "NapptendoGB/interrupts.h"

void interrupt_set_IME(interrupt_t* interrupt, uint8_t value) {
    interrupt->IME = value & 1;
}

void interrupt_req(interrupt_t* interrupt, uint8_t id) {
    uint8_t req = bus_read(interrupt->bus, IF);
    req |= (1 << id);
    bus_write(interrupt->bus, 0xFF0F, req);
}

void interrupt_perform(interrupt_t* interrupt) {
    if (interrupt->IME) {
        uint8_t req = bus_read(interrupt->bus, IF);
        uint8_t enabled = bus_read(interrupt->bus, IE);

        uint8_t i;
        if (req > 0) {
            for (i = 0; i < 5; i++) {
                if (req & (1 << i)) {
                    if (enabled & (1 << i)) {
                        interrupt_service(interrupt, i);
                    }
                }
            }
        }
    }
}

void interrupt_service(interrupt_t* interrupt, int type) {
    interrupt->bus->cpu->halted = 0;
    interrupt->IME = 0;

    uint8_t req = cpu_read(interrupt->bus->cpu, IF);
    req &= ~(1 << type);
    cpu_write(interrupt->bus->cpu, IF, req);

    PUSH_reg16(interrupt->bus->cpu, interrupt->bus->cpu->PC);

    interrupt->bus->cpu->PC = 0x40 + type * 8;
}
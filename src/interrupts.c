#include "interrupts.h"

void interrupt_set_IME(interrupt_t* interrupt, uint8_t value) {
    interrupt->bus->cpu->IME = value & 1;
}

void interrupt_req(interrupt_t* interrupt, uint8_t id) {
    uint8_t req = bus_read(interrupt->bus, IF);
    req |= (1 << id);
    bus_write(interrupt->bus, 0xFF0F, req);
}

void interrupt_perform(interrupt_t* interrupt) {
    uint8_t req = bus_read(interrupt->bus, IF);
    uint8_t enabled = bus_read(interrupt->bus, IE);
    uint8_t pending = req & enabled & 0x1F;
    
    // Wake from HALT if any interrupt is pending, regardless of IME
    if (pending && interrupt->bus->cpu->halted) {
        interrupt->bus->cpu->halted = 0;
    }
    
    // Only service interrupts if IME is set
    if (interrupt->bus->cpu->IME && pending) {
        // Service highest priority interrupt (lowest bit number)
        for (uint8_t i = 0; i < 5; i++) {
            if (pending & (1 << i)) {
                interrupt_service(interrupt, i);
                break; // Only service one interrupt per cycle
            }
        }
    }
}

void interrupt_service(interrupt_t* interrupt, int type) {
    // Clear halted state and disable interrupts
    interrupt->bus->cpu->halted = 0;
    interrupt->bus->cpu->IME = 0;

    // Clear the interrupt flag
    uint8_t req = bus_read(interrupt->bus, IF);
    req &= ~(1 << type);
    bus_write(interrupt->bus, IF, req);

    // Push PC to stack and jump to interrupt vector
    PUSH_reg16(interrupt->bus->cpu, interrupt->bus->cpu->PC);
    
    // Interrupt vectors: VBlank=0x40, STAT=0x48, Timer=0x50, Serial=0x58, Joypad=0x60
    interrupt->bus->cpu->PC = 0x40 + (type * 8);
    
    // Add cycles for interrupt handling
    interrupt->bus->cpu->cycles += 5;
}
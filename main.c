#include <stdio.h>
#include "bus.h"
#include "debug.h"

void draw_screen(gpu_t* gpu) {
    int i, j;
    for (i = 0; i < 144; i++) {
        for (j = 0; j < 160; j++) {
            uint8_t val = gpu->screen_data[i][j][0];
            switch(val) {
                case 0xFF:
                    printf("@@");
                    break;
                case 0xCC:
                    printf("##");
                    break;
                case 0x77:
                    printf(";;");
                    break;
                case 0x00:
                    printf("..");
                    break;
                default:
                    printf("  ");
                    break;
            }
        }
        printf("\n");
    }
    printf("\x1b[H\x1b[2J");
}

void gameboy_update(bus_t* bus) {
    const int MAXCYCLES = 69905;
    int curr_cycles = 0;
    while (curr_cycles < MAXCYCLES) {
        bus->cpu->cycles = 0;
        cpu_clock(bus->cpu);
        timer_update(bus->timer);
        gpu_update_graphics(bus->gpu);
        interrupt_perform(bus->interrupt);
        curr_cycles += bus->cpu->cycles;
    }
    draw_screen(bus->gpu);
}

void gameboy_update2(bus_t* bus) {
    bus->cpu->cycles = 0;
    cpu_clock(bus->cpu);
    timer_update(bus->timer);
    gpu_update_graphics(bus->gpu);
    interrupt_perform(bus->interrupt);
    draw_screen(bus->gpu);
}

void gameboy_serial(bus_t* bus) {
    bus->cpu->cycles = 0;
    cpu_clock(bus->cpu);
    timer_update(bus->timer);
    interrupt_perform(bus->interrupt);

//    debug_print_opcode(bus->cpu);
//    debug_print_fetched(bus->cpu);
//    debug_print_opcode(bus->cpu);
//    debug_print_registers(bus->cpu);
    if (bus_read(bus, 0xFF02) == 0x81) {
        char c = bus_read(bus, 0xFF01);
        printf("%c", c);
        bus_write(bus, 0xFF02, 0);
    }

    // printf("%d\n", bus->cpu->cycles);
}

bus_t mbus;
cpu_t mcpu;
interrupt_t minterrupt;
timergb_t mtimer;
gpu_t mgpu;
struct debug mdebug;

void connect() {
    mbus.cpu        = &mcpu;
    mbus.interrupt  = &minterrupt;
    mbus.timer      = &mtimer;
    mbus.gpu        = &mgpu;

    mcpu.bus        = &mbus;
    minterrupt.bus  = &mbus;
    mtimer.bus      = &mbus;
    mgpu.bus        = &mbus;
}

void reset_all() {
    bus_reset(&mbus);
    cpu_reset(&mcpu);
    timer_reset(&mtimer);
    gpu_reset_screen(&mgpu);

    bus_rombank_setup(&mbus);
}

int main(int argc, char** argv) {
    connect();

    bus_load_rom(&mbus, "C:\\Users\\John Connery\\Desktop\\cpu_instrs\\individual\\02-interrupts.gb");

    reset_all();

//    while (1)
//        gameboy_serial(&mbus);

    debug_init(&mdebug, "C:\\Users\\John Connery\\Downloads\\Blargg2LYStubbed\\EpicLog.txt");
    mbus.rom[LY] = 0x90;

    int i = 1;
    while (debug_load_line(&mdebug)) {
        cpu_clock(&mcpu);
        interrupt_perform(&minterrupt);
        timer_update(&mtimer);
        // !debug_check_line(&debug, &mcpu)

        if (!debug_check_line(&mdebug, &mcpu)) {
            printf("Line:\t%d\n", i);
            printf("Good:\t%s", mdebug.buffer);
            printf("Bad:\tA: %02x F: %02x B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x SP: %04x PC: 00:%04x (%02x %04x\n\n",
            mcpu.regs.afReg.b.A,
            mcpu.regs.afReg.b.F,
            mcpu.regs.bcReg.b.B,
            mcpu.regs.bcReg.b.C,
            mcpu.regs.deReg.b.D,
            mcpu.regs.deReg.b.E,
            mcpu.regs.hlReg.b.H,
            mcpu.regs.hlReg.b.L,
            mcpu.SP,
            mcpu.PC,
            mcpu.opcode,
            mcpu.fetched);

            exit(0);
        }
        i++;
        //else exit(0);
    }

//    for (;;) gameboy_serial(&mbus);
    // When done
    bus_free_rambanks(&mbus);

    return 0;
}

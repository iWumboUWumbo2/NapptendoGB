#ifndef BUS_H
#define BUS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

#include "cpu.h"
#include "interrupts.h"
#include "timers.h"
#include "gpu.h"
#include "controls.h"

#define ROM_SIZE    0x10000
#define GMB_SIZE    0x800000
#define BNK_SIZE    0x2000

#define SET_MBC(n)  (bus->MBC |= (1UL << (n)))
#define CLR_MBC(n)  (bus->MBC &= ~(1UL << (n)))
#define IS_MBC(n) ((bus->MBC >> (n)) & 1UL)

typedef struct rambank rambank_t;

struct rambank {
    uint8_t data[BNK_SIZE];
};

struct bus
{
    cpu_t* cpu;
    interrupt_t* interrupt;
    timergb_t* timer;
    gpu_t* gpu;

    uint8_t rom[ROM_SIZE];
    uint8_t game[GMB_SIZE];
    rambank_t* rambanks;

    uint8_t enab_rambank_write;
    uint8_t is_Rombanking;
    uint8_t num_rambanks;
    uint8_t MBC;

    uint16_t curr_rombank;
    uint8_t curr_rambank;
};

uint8_t bus_read(bus_t* bus, uint16_t addr);
void bus_write(bus_t* bus, uint16_t addr, uint8_t data);
void bus_MBC1(bus_t* bus, uint16_t addr, uint8_t data);
void bus_MBC2(bus_t* bus, uint16_t addr, uint8_t data);
void bus_MBC3(bus_t* bus, uint16_t addr, uint8_t data);
void bus_MBC5(bus_t* bus, uint16_t addr, uint8_t data);
void bus_handle_banking(bus_t* bus, uint16_t addr, uint8_t data);
void bus_rambank_enable(bus_t* bus, uint16_t addr, uint8_t data);
void bus_change_lo_rombank(bus_t* bus, uint8_t data);
void bus_change_hi_rombank(bus_t* bus, uint8_t data);
void bus_change_rambank(bus_t* bus, uint8_t data);
void bus_change_rom_ram_mode(bus_t* bus, uint8_t data);
void bus_load_rom(bus_t* bus, char* filename);
void bus_reset(bus_t* bus);
void bus_detect_rombank_mode(bus_t* bus);
void bus_detect_rambank_mode(bus_t* bus);
void bus_create_rambanks(bus_t* bus);
void bus_free_rambanks(bus_t* bus);
void bus_DMA_transfer(bus_t* bus, uint8_t data);
void bus_rombank_setup(bus_t* bus);

#endif //BUS_H

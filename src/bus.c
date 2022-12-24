#include "NapptendoGB/bus.h"

uint8_t bus_read(bus_t* bus, uint16_t addr) {
    if ((addr >= 0x4000) && (addr < 0x8000)) {
        return bus->game[addr + (bus->curr_rombank - 1) * 0x4000];
    }
    else if ((addr >= 0xA000) && (addr < 0xC000)) {
        return bus->rambanks[bus->curr_rambank].data[addr - 0xA000];
    }
    else if (addr == 0xFF00) {
        return controls_get_state(bus);
    }
    else {
        return bus->rom[addr];
    }
}

void bus_write(bus_t* bus, uint16_t addr, uint8_t data) {
    if (addr < 0x8000) {
        bus_handle_banking(bus, addr, data); // ROM
    }

    else if ((addr >= 0xA000) && (addr < 0xC000)) {
        if (bus->enab_rambank_write) {
            uint16_t new_addr = addr - 0xA000;
            bus->rambanks[bus->curr_rambank].data[new_addr] = data;
        }
    }

    else if ((addr >= 0xE000) && (addr < 0xFE00)) { // ECHO RAM
        bus->rom[addr] = data;
        bus->rom[addr - 0x2000] = data;
    }

    else if (( addr >= 0xFEA0 ) && (addr < 0xFEFF)) { // Restricted
        return;
    }

    else if (addr == DIV || addr == LY) { // Writes to DIV or LY register reset it
        bus->rom[addr] = 0x00;
    }

    else if (addr == TAC) { // Writes to TAC update the time freq
        uint8_t oldfreq = timer_get_clock_freq(bus->timer);
        bus->rom[TAC] = data;
        uint8_t newfreq = timer_get_clock_freq(bus->timer);
        if (oldfreq != newfreq)
            timer_set_clock_freq(bus->timer);
    }

    else if (addr == 0xFF46) {
        bus_DMA_transfer(bus, data);
    }

    else {
        bus->rom[addr] = data;
    }
}

void bus_MBC1(bus_t* bus, uint16_t addr, uint8_t data) {
    if (addr < 0x2000) {
        bus_rambank_enable(bus, addr, data);
    }
    else if ((addr >= 0x2000) && (addr < 0x4000)) {
        bus_change_lo_rombank(bus, data);
    }
    else if ((addr >= 0x4000) && (addr < 0x6000)) {
        if (bus->is_Rombanking) {
            bus_change_hi_rombank(bus, data);
        } else {
            bus_change_rambank(bus, data);
        }
    }
    else if ((addr >= 0x6000) && (addr < 0x8000)) {
        bus_change_rom_ram_mode(bus, data);
    }
    else return;
}

void bus_MBC2(bus_t* bus, uint16_t addr, uint8_t data) {
    if (addr < 0x2000) {
        bus_rambank_enable(bus, addr, data);
    }
    else if ((addr >= 0x2000) && (addr < 0x4000)) {
        bus_change_lo_rombank(bus, data);
    }
    else return;
}

void bus_MBC3(bus_t* bus, uint16_t addr, uint8_t data) {

}

void bus_MBC5(bus_t* bus, uint16_t addr, uint8_t data) {
//    if (addr < 0x2000) {
//        bus_rambank_enable(bus, addr, data);
//    }
//    else if ((addr >= 0x2000) && (addr < 0x3000) {
//        bus->rom[addr] =
//    }
}

void bus_handle_banking(bus_t* bus, uint16_t addr, uint8_t data) {
    if (IS_MBC(1)) bus_MBC1(bus, addr, data);
    else if (IS_MBC(2)) bus_MBC2(bus, addr, data);
    else if (IS_MBC(3)) bus_MBC3(bus, addr, data);
    else if (IS_MBC(5)) bus_MBC5(bus, addr, data);
    else return;
}

void bus_rambank_enable(bus_t* bus, uint16_t addr, uint8_t data) {
    if (IS_MBC(2)) {
        if ((addr & 0x10) != 0)
            return;
    }

    if ((data & 0x0F) == 0x0A)
        bus->enab_rambank_write = 1;
    else if ((data & 0x0F) == 0x00)
        bus->enab_rambank_write = 0;
    else
        return;
}

void bus_change_lo_rombank(bus_t* bus, uint8_t data) {
    if (IS_MBC(2)) {
        bus->curr_rombank = data & 0x0F;
        if (bus->curr_rombank == 0)
            bus->curr_rombank++;
        return;
    }

    bus->curr_rombank &= 0xE0; // clear lower 5 bits
    bus->curr_rombank |= (data & 0x1F);
    if (bus->curr_rombank == 0) bus->curr_rombank++;
}

void bus_change_hi_rombank(bus_t* bus, uint8_t data) {
    bus->curr_rombank &= 0x1F; // Clear upper 3 bits of current rombank
    bus->curr_rombank |= (data & 0xE0); // Clear lower 5 bits of data
    if (bus->curr_rombank == 0) bus->curr_rombank++;
}

void bus_change_rambank(bus_t* bus, uint8_t data) {
    bus->curr_rambank = data & 0x3;
}

void bus_change_rom_ram_mode(bus_t* bus, uint8_t data) {
    bus->is_Rombanking = !(data & 0x01);
    if (bus->is_Rombanking)
        bus->curr_rambank = 0;
}

void bus_load_rom(bus_t* bus, char* filename) {
    memset(bus->rom, 0, ROM_SIZE);
    memset(bus->game, 0, GMB_SIZE);

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("ROM not found!");
        exit(0);
    }
    fread(bus->game, 1, GMB_SIZE, fp);

    memcpy(bus->rom, bus->game, 0x8000);
}

void bus_reset(bus_t* bus) {
    bus->rom[0xFF05] = 0x00   ; // TIMA
    bus->rom[0xFF06] = 0x00   ; // TMA
    bus->rom[0xFF07] = 0x00   ; // TAC
    bus->rom[0xFF10] = 0x80   ; // NR10
    bus->rom[0xFF11] = 0xBF   ; // NR11
    bus->rom[0xFF12] = 0xF3   ; // NR12
    bus->rom[0xFF14] = 0xBF   ; // NR14
    bus->rom[0xFF16] = 0x3F   ; // NR21
    bus->rom[0xFF17] = 0x00   ; // NR22
    bus->rom[0xFF19] = 0xBF   ; // NR24
    bus->rom[0xFF1A] = 0x7F   ; // NR30
    bus->rom[0xFF1B] = 0xFF   ; // NR31
    bus->rom[0xFF1C] = 0x9F   ; // NR32
    bus->rom[0xFF1E] = 0xBF   ; // NR33
    bus->rom[0xFF20] = 0xFF   ; // NR41
    bus->rom[0xFF21] = 0x00   ; // NR42
    bus->rom[0xFF22] = 0x00   ; // NR43
    bus->rom[0xFF23] = 0xBF   ; // NR30
    bus->rom[0xFF24] = 0x77   ; // NR50
    bus->rom[0xFF25] = 0xF3   ; // NR51
    bus->rom[0xFF26] = 0xF1   ; // NR52
    bus->rom[0xFF40] = 0x91   ; // LCDC
    bus->rom[0xFF42] = 0x00   ; // SCY
    bus->rom[0xFF43] = 0x00   ; // SCX
    bus->rom[0xFF45] = 0x00   ; // LYC
    bus->rom[0xFF47] = 0xFC   ; // BGP
    bus->rom[0xFF48] = 0xFF   ; // OBP0
    bus->rom[0xFF49] = 0xFF   ; // OBP1
    bus->rom[0xFF4A] = 0x00   ; // WY
    bus->rom[0xFF4B] = 0x00   ; // WX
    bus->rom[0xFFFF] = 0x00   ; // IE
}

void bus_rombank_setup(bus_t* bus) {
    bus->curr_rombank = 1;
    bus->rambanks = NULL;

    bus_detect_rombank_mode(bus);
    bus_detect_rambank_mode(bus);
    if (bus->num_rambanks != 0)
        bus_create_rambanks(bus);
}

void bus_detect_rombank_mode(bus_t* bus) {
    bus->MBC = 0x00;

    switch (bus_read(bus, 0x147)) {
        case 0x00:
            SET_MBC(0);
            break;
        case 0x01:
        case 0x02:
        case 0x03:
            SET_MBC(1);
            break;
        case 0x05:
        case 0x06:
            SET_MBC(2);
            break;
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            SET_MBC(3);
            break;
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            SET_MBC(5);
        default:
            break;
    }
}

void bus_detect_rambank_mode(bus_t* bus) {
    bus->enab_rambank_write = 0;

    switch (bus_read(bus, 0x149)) {
        case 0x00:
            bus->num_rambanks = 0;
            break;
        case 0x01:
        case 0x02:
            bus->num_rambanks = 1;
            break;
        case 0x03:
            bus->num_rambanks = 4;
            break;
        case 0x04:
            bus->num_rambanks = 16;
            break;
        case 0x05:
            bus->num_rambanks = 8;
            break;
    }
}

void bus_create_rambanks(bus_t* bus) {
    bus->rambanks = malloc(bus->num_rambanks * sizeof(rambank_t));
    uint8_t i;

    // Clear Ram Banks
    for (i = 0; i < bus->num_rambanks; i++) {
        memset(bus->rambanks[i].data, 0, BNK_SIZE);
    }

    memcpy(bus->rambanks[0].data, &bus->rom[0xA000], 0x2000);
}

void bus_free_rambanks(bus_t* bus) {
    free(bus->rambanks);
}

void bus_DMA_transfer(bus_t* bus, uint8_t data) {
    uint16_t addr = data << 8;
    uint16_t i;

    for (i = 0xFE00; i <= 0xFE9F; i++) {
        bus_write(bus, i, bus_read(bus, addr + (i & 0x00FF)));
    }
}
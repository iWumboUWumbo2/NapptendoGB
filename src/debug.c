#include "cpu.h"
#include "debug.h"

void debug_print_opcode(cpu_t* cpu) {
    printf("OP: 0x%04x ", cpu->opcode);
}

void debug_print_fetched(cpu_t* cpu) {
    printf("Fetched: 0x%04x ", cpu->fetched);
}

void debug_print_registers(cpu_t* cpu) {
    printf("SP: 0x%04x ", cpu->SP);
    printf("PC: 0x%04x ", cpu->PC);
    printf("AF: 0x%04x ", cpu->regs.afReg.AF);
    printf("BC: 0x%04x ", cpu->regs.bcReg.BC);
    printf("DE: 0x%04x ", cpu->regs.deReg.DE);
    printf("HL: 0x%04x ", cpu->regs.hlReg.HL);
}

void debug_init(struct debug *debug, const char *logfile) {
    debug->fp = fopen(logfile, "r");
    if (!debug->fp) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }
}

uint8_t debug_load_line(struct debug *debug) {
    if (fgets(debug->buffer, BUFFSZ, debug->fp)) {
        sscanf(debug->buffer, "A: %s F: %s B: %s C: %s D: %s E: %s H: %s L: %s SP: %s PC: 00:%s (%s %s",
               debug->astr, debug->fstr, debug->bstr, debug->cstr, debug->dstr, debug->estr, debug->hstr, debug->lstr,
               debug->spstr, debug->pcstr, debug->opstr, debug->other);

        debug->aval = (int) strtol(debug->astr, NULL, 16);
        debug->fval = (int) strtol(debug->fstr, NULL, 16);
        debug->bval = (int) strtol(debug->bstr, NULL, 16);
        debug->cval = (int) strtol(debug->cstr, NULL, 16);
        debug->dval = (int) strtol(debug->dstr, NULL, 16);
        debug->eval = (int) strtol(debug->estr, NULL, 16);
        debug->hval = (int) strtol(debug->hstr, NULL, 16);
        debug->lval = (int) strtol(debug->lstr, NULL, 16);
        debug->spval = (int) strtol(debug->spstr, NULL, 16);
        debug->pcval = (int) strtol(debug->pcstr, NULL, 16);
        debug->opval = (int) strtol(debug->opstr, NULL, 16);

        return 1;
    }
    return 0;
}

uint8_t debug_check_line(struct debug *debug, cpu_t *cpu) {
    return debug->aval == cpu->regs.afReg.b.A
        && debug->fval == cpu->regs.afReg.b.F
        && debug->bval == cpu->regs.bcReg.b.B
        && debug->cval == cpu->regs.bcReg.b.C
        && debug->dval == cpu->regs.deReg.b.D
        && debug->eval == cpu->regs.deReg.b.E
        && debug->hval == cpu->regs.hlReg.b.H
        && debug->lval == cpu->regs.hlReg.b.L
        && debug->spval == cpu->SP
        && debug->pcval == cpu->PC;
        // && debug->opval == cpu->opcode;
}
#ifndef DEBUG_H
#define DEBUG_H

#define	BUFFSZ	100

struct debug {
    FILE *fp;
    char buffer[BUFFSZ];

    char astr[5], fstr[5], bstr[5], cstr[5], dstr[5], estr[5], hstr[5], lstr[5], spstr[5], pcstr[5], opstr[5], other[12];
    int aval, fval, bval, cval, dval, eval, hval, lval, spval, pcval, opval;
};

void debug_init(struct debug *debug, const char *filname);
uint8_t debug_load_line(struct debug *debug);
uint8_t debug_check_line(struct debug *debug, cpu_t *cpu);

void debug_print_opcode(cpu_t* cpu);
void debug_print_fetched(cpu_t* cpu);
void debug_print_registers(cpu_t* cpu);

#endif
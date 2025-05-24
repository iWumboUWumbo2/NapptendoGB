#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "types.h"
#include "bus.h"

// Useful for setting multiple flags at once
#define Z_FLAG  0x80
#define N_FLAG  0x40
#define H_FLAG  0x20
#define C_FLAG  0x10

#define FLAGS_ISSET(x) ((cpu->regs.afReg.b.F & (x))?1:0)
#define FLAGS_SET(x) (cpu->regs.afReg.b.F |= (x))
#define FLAGS_CLR(x) (cpu->regs.afReg.b.F &= ~(x))

// Useful for setting flag to specific value
#define Z_SHFT  0x07U
#define N_SHFT  0x06U
#define H_SHFT  0x05U
#define C_SHFT  0x04U

#define SHIFT_SET(s, v) \
((cpu->regs.afReg.b.F) = (((cpu->regs.afReg.b.F) & (~(1UL << (s)))) | (((v)?1:0) << (s))))

#define IS_CARRY_16_S8(a, b) (((((a)&0x00FF)+((b)&0x00FF))&0xFF00)?1:0)

#define IS_HALF_CARRY_8(a, b) (((((a)&0x0F)+((b)&0x0F))&0xF0)?1:0)
#define IS_HALF_CARRY_8_SUB(a, b) (((((a)&0x0F)-((b)&0x0F))&0xF0)?1:0)
#define IS_HALF_CARRY_8_3(a, b, c) (((((a)&0x0F)+((b)&0x0F)+((c)&0x0F))&0xF0)?1:0)
#define IS_HALF_CARRY_8_3_SUB(a, b, c) (((((a)&0x0F)-((b)&0x0F)-((c)&0x0F))&0xF0)?1:0)
#define IS_HALF_CARRY_16(a, b) (((((a)&0x0FFF)+((b)&0x0FFF))&0xF000)?1:0)

struct cpu
{
    uint16_t SP; // Stack Pointer
    uint16_t PC; // Program Counter

    // Registers
    struct {
        union {
            struct {
                uint8_t F;
                uint8_t A;
            } b;
            uint16_t AF;
        } afReg;

        union {
            struct {
                uint8_t C;
                uint8_t B;
            } b;
            uint16_t BC;
        } bcReg;

        union {
            struct {
                uint8_t E;
                uint8_t D;
            } b;
            uint16_t DE;
        } deReg;

        union {
            struct {
                uint8_t L;
                uint8_t H;
            } b;
            uint16_t HL;
        } hlReg;
    } regs;

    bus_t* bus;

    uint16_t fetched;
    uint16_t opcode;
    uint8_t cycles;
    uint8_t stopped;
    uint8_t halted;

    uint8_t IME;
};

void cpu_reset(cpu_t* cpu);
uint8_t cpu_read(cpu_t* cpu, uint16_t addr);
void cpu_write(cpu_t* cpu, uint16_t addr, uint8_t data);
uint16_t cpu_fetch(cpu_t* cpu);
void cpu_execute(cpu_t* cpu);
void cpu_clock(cpu_t* cpu);

void NOP(cpu_t* cpu);
void LD_reg16_d16(cpu_t* cpu, uint16_t* reg16);
void LD_mem16_reg8(cpu_t* cpu, uint16_t memreg16, uint8_t reg8);
void INC_reg16(cpu_t* cpu, uint16_t* reg16);
void INC_reg8(cpu_t* cpu, uint8_t* reg8);
void DEC_reg8(cpu_t* cpu, uint8_t* reg8);
void LD_reg8_d8(cpu_t* cpu, uint8_t* reg8);
void RLCA(cpu_t* cpu);
void LD_mema16_SP(cpu_t* cpu);
void ADD_HL_reg16(cpu_t* cpu, uint16_t reg16);
void LD_reg8_mem16(cpu_t* cpu, uint8_t* reg, uint16_t memreg16);
void DEC_reg16(cpu_t* cpu, uint16_t* reg16);
void RRCA(cpu_t* cpu);
void STOP(cpu_t* cpu);
void RLA(cpu_t* cpu);
void JR_s8(cpu_t* cpu);
void RRA(cpu_t* cpu);
void JR_Nflag_s8(cpu_t* cpu, uint8_t flag);
void DAA(cpu_t* cpu);
void JR_flag_s8(cpu_t* cpu, uint8_t flag);
void CPL(cpu_t* cpu);
void INC_memHL(cpu_t* cpu);
void DEC_memHL(cpu_t* cpu);
void LD_memHL_d8(cpu_t* cpu);
void SCF(cpu_t* cpu);
void CCF(cpu_t* cpu);
void LD_reg8_reg8(cpu_t* cpu, uint8_t* reg8to, uint8_t reg8from);
void HALT(cpu_t* cpu);
void ADD_A_reg8(cpu_t* cpu, uint8_t reg8);
void ADD_A_memHL(cpu_t* cpu);
void ADC_A_reg8(cpu_t* cpu, uint8_t reg8);
void ADC_A_memHL(cpu_t* cpu);
void SUB_reg8(cpu_t* cpu, uint8_t reg8);
void SUB_memHL(cpu_t* cpu);
void SBC_A_reg8(cpu_t* cpu, uint8_t reg8);
void SBC_A_memHL(cpu_t* cpu);
void AND_reg8(cpu_t* cpu, uint8_t reg8);
void AND_memHL(cpu_t* cpu);
void XOR_reg8(cpu_t* cpu, uint8_t reg8);
void XOR_memHL(cpu_t* cpu);
void OR_reg8(cpu_t* cpu, uint8_t reg8);
void OR_memHL(cpu_t* cpu);
void CP_reg8(cpu_t* cpu, uint8_t reg8);
void CP_memHL(cpu_t* cpu);
void RET_Nflag(cpu_t* cpu, uint8_t flag);
void POP_reg16(cpu_t* cpu, uint16_t* reg16);
void JP_Nflag_a16(cpu_t* cpu, uint8_t flag);
void JP_a16(cpu_t* cpu);
void CALL_Nflag_a16(cpu_t* cpu, uint8_t flag);
void PUSH_reg16(cpu_t* cpu, uint16_t reg16);
void ADD_A_d8(cpu_t* cpu);
void RST_n(cpu_t* cpu, uint8_t n);
void RET_flag(cpu_t* cpu, uint8_t flag);
void RET(cpu_t* cpu);
void JP_flag_a16(cpu_t* cpu, uint8_t flag);
void CALL_flag_a16(cpu_t* cpu, uint8_t flag);
void CALL_a16(cpu_t* cpu);
void ADC_A_d8(cpu_t* cpu);
void SUB_d8(cpu_t* cpu);
void RETI(cpu_t* cpu);
void SBC_A_d8(cpu_t* cpu);
void LDH_mema8_A(cpu_t* cpu);
void LD_memC_A(cpu_t* cpu);
void AND_d8(cpu_t* cpu);
void ADD_SP_s8(cpu_t* cpu);
void JP_HL(cpu_t* cpu);
void LD_mema16_A(cpu_t* cpu);
void XOR_d8(cpu_t* cpu);
void LDH_A_mema8(cpu_t* cpu);
void LD_A_memC(cpu_t* cpu);
void DI(cpu_t* cpu);
void OR_d8(cpu_t* cpu);
void LD_HL_SP_sum_s8(cpu_t* cpu);
void LD_SP_HL(cpu_t* cpu);
void LD_A_mema16(cpu_t* cpu);
void EI(cpu_t* cpu);
void CP_d8(cpu_t* cpu);

void RLC_reg8(cpu_t* cpu, uint8_t* reg8);
void RLC_memHL(cpu_t* cpu);
void RRC_reg8(cpu_t* cpu, uint8_t* reg8);
void RRC_memHL(cpu_t* cpu);
void RL_reg8(cpu_t* cpu, uint8_t* reg8);
void RL_memHL(cpu_t* cpu);
void RR_reg8(cpu_t* cpu, uint8_t* reg8);
void RR_memHL(cpu_t* cpu);
void SLA_reg8(cpu_t* cpu, uint8_t* reg8);
void SLA_memHL(cpu_t* cpu);
void SRA_reg8(cpu_t* cpu, uint8_t* reg8);
void SRA_memHL(cpu_t* cpu);
void SWAP_reg8(cpu_t* cpu, uint8_t* reg8);
void SWAP_memHL(cpu_t* cpu);
void SRL_reg8(cpu_t* cpu, uint8_t* reg8);
void SRL_memHL(cpu_t* cpu);
void BIT_n_reg8(cpu_t* cpu, uint8_t n, uint8_t reg8);
void BIT_n_memHL(cpu_t* cpu, uint8_t n);
void RES_n_reg8(cpu_t* cpu, uint8_t n, uint8_t* reg8);
void RES_n_memHL(cpu_t* cpu, uint8_t n);
void SET_n_reg8(cpu_t* cpu, uint8_t n, uint8_t* reg8);
void SET_n_memHL(cpu_t* cpu, uint8_t n);

#endif //CPU_H

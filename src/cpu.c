#include "NapptendoGB/cpu.h"

uint8_t cpu_read(cpu_t* cpu, uint16_t addr) {
    cpu->cycles++;
    return bus_read(cpu->bus, addr);
}

void cpu_write(cpu_t* cpu, uint16_t addr, uint8_t data) {
    cpu->cycles++;
    bus_write(cpu->bus, addr, data);
}

uint16_t cpu_fetch(cpu_t* cpu) {
    uint16_t lo, hi;

    switch (cpu->opcode)
    {
        // d16 or a16
        case 0x0001:
        case 0x0008:
        case 0x0011:
        case 0x0021:
        case 0x0031:
        case 0x00C2:
        case 0x00C3:
        case 0x00C4:
        case 0x00CA:
        case 0x00CC:
        case 0x00CD:
        case 0x00D2:
        case 0x00D4:
        case 0x00DA:
        case 0x00DC:
        case 0x00EA:
        case 0x00FA:
            lo = cpu_read(cpu, cpu->PC++);
            hi = cpu_read(cpu, cpu->PC++);
            cpu->fetched = (hi << 8) | lo;
            break;
        // d8 / a8
        case 0x0006:
        case 0x000E:
        case 0x0016:
        case 0x001E:
        case 0x0026:
        case 0x002E:
        case 0x0036:
        case 0x003E:
        case 0x00C6:
        case 0x00CE:
        case 0x00D6:
        case 0x00DE:
        case 0x00E0:
        case 0x00E6:
        case 0x00EE:
        case 0x00F0:
        case 0x00F6:
        case 0x00FE:
            cpu->fetched = cpu_read(cpu, cpu->PC++);
            break;
        // s8
        case 0x0018:
        case 0x0020:
        case 0x0028:
        case 0x0030:
        case 0x0038:
        case 0x00E8:
        case 0x00F8:
            cpu->fetched = cpu_read(cpu, cpu->PC++);
            cpu->fetched |= (cpu->fetched & 0x80) ? 0xFF00 : 0x0000;
            break;
        default:
            break;
    }

    return cpu->fetched;
}

void cpu_execute(cpu_t* cpu) {
    switch (cpu->opcode)
    {
        case 0x0000:
        default:
            NOP(cpu);
            break;
        case 0x0001:
            LD_reg16_d16(cpu, &cpu->regs.bcReg.BC);
            break;
        case 0x0002:
            LD_mem16_reg8(cpu, cpu->regs.bcReg.BC, cpu->regs.afReg.b.A);
            break;
        case 0x0003:
            INC_reg16(cpu, &cpu->regs.bcReg.BC);
            break;
        case 0x0004:
            INC_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0x0005:
            DEC_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0x0006:
            LD_reg8_d8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0x0007:
            RLCA(cpu);
            break;
        case 0x0008:
            LD_mema16_SP(cpu);
            break;
        case 0x0009:
            ADD_HL_reg16(cpu, cpu->regs.bcReg.BC);
            break;
        case 0x000A:
            LD_reg8_mem16(cpu, &cpu->regs.afReg.b.A, cpu->regs.bcReg.BC);
            break;
        case 0x000B:
            DEC_reg16(cpu, &cpu->regs.bcReg.BC);
            break;
        case 0x000C:
            INC_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0x000D:
            DEC_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0x000E:
            LD_reg8_d8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0x000F:
            RRCA(cpu);
            break;
        case 0x1000:
            STOP(cpu);
            break;
        case 0x0011:
            LD_reg16_d16(cpu, &cpu->regs.deReg.DE);
            break;
        case 0x0012:
            LD_mem16_reg8(cpu, cpu->regs.deReg.DE, cpu->regs.afReg.b.A);
            break;
        case 0x0013:
            INC_reg16(cpu, &cpu->regs.deReg.DE);
            break;
        case 0x0014:
            INC_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0x0015:
            DEC_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0x0016:
            LD_reg8_d8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0x0017:
            RLA(cpu);
            break;
        case 0x0018:
            JR_s8(cpu);
            break;
        case 0x0019:
            ADD_HL_reg16(cpu, cpu->regs.deReg.DE);
            break;
        case 0x001A:
            LD_reg8_mem16(cpu, &cpu->regs.afReg.b.A, cpu->regs.deReg.DE);
            break;
        case 0x001B:
            DEC_reg16(cpu, &cpu->regs.deReg.DE);
            break;
        case 0x001C:
            INC_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0x001D:
            DEC_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0x001E:
            LD_reg8_d8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0x001F:
            RRA(cpu);
            break;
        case 0x0020:
            JR_Nflag_s8(cpu, Z_FLAG);
            break;
        case 0x0021:
            LD_reg16_d16(cpu, &cpu->regs.hlReg.HL);
            break;
        case 0x0022:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.afReg.b.A);
            cpu->regs.hlReg.HL++;
            break;
        case 0x0023:
            INC_reg16(cpu, &cpu->regs.hlReg.HL);
            break;
        case 0x0024:
            INC_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0x0025:
            DEC_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0x0026:
            LD_reg8_d8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0x0027:
            DAA(cpu);
            break;
        case 0x0028:
            JR_flag_s8(cpu, Z_FLAG);
            break;
        case 0x0029:
            ADD_HL_reg16(cpu, cpu->regs.hlReg.HL);
            break;
        case 0x002A:
            LD_reg8_mem16(cpu, &cpu->regs.afReg.b.A, cpu->regs.hlReg.HL);
            cpu->regs.hlReg.HL++;
            break;
        case 0x002B:
            DEC_reg16(cpu, &cpu->regs.hlReg.HL);
            break;
        case 0x002C:
            INC_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0x002D:
            DEC_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0x002E:
            LD_reg8_d8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0x002F:
            CPL(cpu);
            break;
        case 0x0030:
            JR_Nflag_s8(cpu, C_FLAG);
            break;
        case 0x0031:
            LD_reg16_d16(cpu, &cpu->SP);
            break;
        case 0x0032:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.afReg.b.A);
            cpu->regs.hlReg.HL--;
            break;
        case 0x0033:
            INC_reg16(cpu, &cpu->SP);
            break;
        case 0x0034:
            INC_memHL(cpu);
            break;
        case 0x0035:
            DEC_memHL(cpu);
            break;
        case 0x0036:
            LD_memHL_d8(cpu);
            break;
        case 0x0037:
            SCF(cpu);
            break;
        case 0x0038:
            JR_flag_s8(cpu, C_FLAG);
            break;
        case 0x0039:
            ADD_HL_reg16(cpu, cpu->SP);
            break;
        case 0x003A:
            LD_reg8_mem16(cpu, &cpu->regs.afReg.b.A, cpu->regs.hlReg.HL);
            cpu->regs.hlReg.HL--;
            break;
        case 0x003B:
            DEC_reg16(cpu, &cpu->SP);
            break;
        case 0x003C:
            INC_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0x003D:
            DEC_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0x003E:
            LD_reg8_d8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0x003F:
            CCF(cpu);
            break;
        case 0x0040:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.bcReg.b.B);
            break;
        case 0x0041:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.bcReg.b.C);
            break;
        case 0x0042:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.deReg.b.D);
            break;
        case 0x0043:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.deReg.b.E);
            break;
        case 0x0044:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.hlReg.b.H);
            break;
        case 0x0045:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.hlReg.b.L);
            break;
        case 0x0046:
            LD_reg8_mem16(cpu, &cpu->regs.bcReg.b.B, cpu->regs.hlReg.HL);
            break;
        case 0x0047:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.B, cpu->regs.afReg.b.A);
            break;
        case 0x0048:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.bcReg.b.B);
            break;
        case 0x0049:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.bcReg.b.C);
            break;
        case 0x004A:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.deReg.b.D);
            break;
        case 0x004B:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.deReg.b.E);
            break;
        case 0x004C:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.hlReg.b.H);
            break;
        case 0x004D:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.hlReg.b.L);
            break;
        case 0x004E:
            LD_reg8_mem16(cpu, &cpu->regs.bcReg.b.C, cpu->regs.hlReg.HL);
            break;
        case 0x004F:
            LD_reg8_reg8(cpu, &cpu->regs.bcReg.b.C, cpu->regs.afReg.b.A);
            break;
        case 0x0050:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.bcReg.b.B);
            break;
        case 0x0051:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.bcReg.b.C);
            break;
        case 0x0052:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.deReg.b.D);
            break;
        case 0x0053:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.deReg.b.E);
            break;
        case 0x0054:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.hlReg.b.H);
            break;
        case 0x0055:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.hlReg.b.L);
            break;
        case 0x0056:
            LD_reg8_mem16(cpu, &cpu->regs.deReg.b.D, cpu->regs.hlReg.HL);
            break;
        case 0x0057:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.D, cpu->regs.afReg.b.A);
            break;
        case 0x0058:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.bcReg.b.B);
            break;
        case 0x0059:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.bcReg.b.C);
            break;
        case 0x005A:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.deReg.b.D);
            break;
        case 0x005B:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.deReg.b.E);
            break;
        case 0x005C:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.hlReg.b.H);
            break;
        case 0x005D:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.hlReg.b.L);
            break;
        case 0x005E:
            LD_reg8_mem16(cpu, &cpu->regs.deReg.b.E, cpu->regs.hlReg.HL);
            break;
        case 0x005F:
            LD_reg8_reg8(cpu, &cpu->regs.deReg.b.E, cpu->regs.afReg.b.A);
            break;
        case 0x0060:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.bcReg.b.B);
            break;
        case 0x0061:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.bcReg.b.C);
            break;
        case 0x0062:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.deReg.b.D);
            break;
        case 0x0063:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.deReg.b.E);
            break;
        case 0x0064:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.hlReg.b.H);
            break;
        case 0x0065:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.hlReg.b.L);
            break;
        case 0x0066:
            LD_reg8_mem16(cpu, &cpu->regs.hlReg.b.H, cpu->regs.hlReg.HL);
            break;
        case 0x0067:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.H, cpu->regs.afReg.b.A);
            break;
        case 0x0068:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.bcReg.b.B);
            break;
        case 0x0069:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.bcReg.b.C);
            break;
        case 0x006A:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.deReg.b.D);
            break;
        case 0x006B:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.deReg.b.E);
            break;
        case 0x006C:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.hlReg.b.H);
            break;
        case 0x006D:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.hlReg.b.L);
            break;
        case 0x006E:
            LD_reg8_mem16(cpu, &cpu->regs.hlReg.b.L, cpu->regs.hlReg.HL);
            break;
        case 0x006F:
            LD_reg8_reg8(cpu, &cpu->regs.hlReg.b.L, cpu->regs.afReg.b.A);
            break;
        case 0x0070:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.bcReg.b.B);
            break;
        case 0x0071:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.bcReg.b.C);
            break;
        case 0x0072:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.deReg.b.D);
            break;
        case 0x0073:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.deReg.b.E);
            break;
        case 0x0074:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.hlReg.b.H);
            break;
        case 0x0075:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.hlReg.b.L);
            break;
        case 0x0076:
            HALT(cpu);
            break;
        case 0x0077:
            LD_mem16_reg8(cpu, cpu->regs.hlReg.HL, cpu->regs.afReg.b.A);
            break;
        case 0x0078:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.bcReg.b.B);
            break;
        case 0x0079:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.bcReg.b.C);
            break;
        case 0x007A:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.deReg.b.D);
            break;
        case 0x007B:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.deReg.b.E);
            break;
        case 0x007C:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.hlReg.b.H);
            break;
        case 0x007D:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.hlReg.b.L);
            break;
        case 0x007E:
            LD_reg8_mem16(cpu, &cpu->regs.afReg.b.A, cpu->regs.hlReg.HL);
            break;
        case 0x007F:
            LD_reg8_reg8(cpu, &cpu->regs.afReg.b.A, cpu->regs.afReg.b.A);
            break;
        case 0x0080:
            ADD_A_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x0081:
            ADD_A_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x0082:
            ADD_A_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x0083:
            ADD_A_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x0084:
            ADD_A_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x0085:
            ADD_A_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x0086:
            ADD_A_memHL(cpu);
            break;
        case 0x0087:
            ADD_A_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x0088:
            ADC_A_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x0089:
            ADC_A_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x008A:
            ADC_A_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x008B:
            ADC_A_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x008C:
            ADC_A_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x008D:
            ADC_A_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x008E:
            ADC_A_memHL(cpu);
            break;
        case 0x008F:
            ADC_A_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x0090:
            SUB_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x0091:
            SUB_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x0092:
            SUB_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x0093:
            SUB_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x0094:
            SUB_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x0095:
            SUB_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x0096:
            SUB_memHL(cpu);
            break;
        case 0x0097:
            SUB_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x0098:
            SBC_A_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x0099:
            SBC_A_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x009A:
            SBC_A_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x009B:
            SBC_A_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x009C:
            SBC_A_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x009D:
            SBC_A_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x009E:
            SBC_A_memHL(cpu);
            break;
        case 0x009F:
            SBC_A_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x00A0:
            AND_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x00A1:
            AND_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x00A2:
            AND_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x00A3:
            AND_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x00A4:
            AND_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x00A5:
            AND_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x00A6:
            AND_memHL(cpu);
            break;
        case 0x00A7:
            AND_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x00A8:
            XOR_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x00A9:
            XOR_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x00AA:
            XOR_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x00AB:
            XOR_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x00AC:
            XOR_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x00AD:
            XOR_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x00AE:
            XOR_memHL(cpu);
            break;
        case 0x00AF:
            XOR_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x00B0:
            OR_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x00B1:
            OR_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x00B2:
            OR_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x00B3:
            OR_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x00B4:
            OR_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x00B5:
            OR_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x00B6:
            OR_memHL(cpu);
            break;
        case 0x00B7:
            OR_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x00B8:
            CP_reg8(cpu, cpu->regs.bcReg.b.B);
            break;
        case 0x00B9:
            CP_reg8(cpu, cpu->regs.bcReg.b.C);
            break;
        case 0x00BA:
            CP_reg8(cpu, cpu->regs.deReg.b.D);
            break;
        case 0x00BB:
            CP_reg8(cpu, cpu->regs.deReg.b.E);
            break;
        case 0x00BC:
            CP_reg8(cpu, cpu->regs.hlReg.b.H);
            break;
        case 0x00BD:
            CP_reg8(cpu, cpu->regs.hlReg.b.L);
            break;
        case 0x00BE:
            CP_memHL(cpu);
            break;
        case 0x00BF:
            CP_reg8(cpu, cpu->regs.afReg.b.A);
            break;
        case 0x00C0:
            RET_Nflag(cpu, Z_FLAG);
            break;
        case 0x00C1:
            POP_reg16(cpu, &cpu->regs.bcReg.BC);
            break;
        case 0x00C2:
            JP_Nflag_a16(cpu, Z_FLAG);
            break;
        case 0x00C3:
            JP_a16(cpu);
            break;
        case 0x00C4:
            CALL_Nflag_a16(cpu, Z_FLAG);
            break;
        case 0x00C5:
            PUSH_reg16(cpu, cpu->regs.bcReg.BC);
            break;
        case 0x00C6:
            ADD_A_d8(cpu);
            break;
        case 0x00C7:
            RST_n(cpu, 0x00);
            break;
        case 0x00C8:
            RET_flag(cpu, Z_FLAG);
            break;
        case 0x00C9:
            RET(cpu);
            break;
        case 0x00CA:
            JP_flag_a16(cpu, Z_FLAG);
            break;
        case 0x00CC:
            CALL_flag_a16(cpu, Z_FLAG);
            break;
        case 0x00CD:
            CALL_a16(cpu);
            break;
        case 0x00CE:
            ADC_A_d8(cpu);
            break;
        case 0x00CF:
            RST_n(cpu, 0x08);
            break;
        case 0x00D0:
            RET_Nflag(cpu, C_FLAG);
            break;
        case 0x00D1:
            POP_reg16(cpu, &cpu->regs.deReg.DE);
            break;
        case 0x00D2:
            JP_Nflag_a16(cpu, C_FLAG);
            break;
        case 0x00D4:
            CALL_Nflag_a16(cpu, C_FLAG);
            break;
        case 0x00D5:
            PUSH_reg16(cpu, cpu->regs.deReg.DE);
            break;
        case 0x00D6:
            SUB_d8(cpu);
            break;
        case 0x00D7:
            RST_n(cpu, 0x10);
            break;
        case 0x00D8:
            RET_flag(cpu, C_FLAG);
            break;
        case 0x00D9:
            RETI(cpu);
            break;
        case 0x00DA:
            JP_flag_a16(cpu, C_FLAG);
            break;
        case 0x00DC:
            CALL_flag_a16(cpu, C_FLAG);
            break;
        case 0x00DE:
            SBC_A_d8(cpu);
            break;
        case 0x00DF:
            RST_n(cpu, 0x18);
            break;
        case 0x00E0:
            LDH_mema8_A(cpu);
            break;
        case 0x00E1:
            POP_reg16(cpu, &cpu->regs.hlReg.HL);
            break;
        case 0x00E2:
            LD_memC_A(cpu);
            break;
        case 0x00E5:
            PUSH_reg16(cpu, cpu->regs.hlReg.HL);
            break;
        case 0x00E6:
            AND_d8(cpu);
            break;
        case 0x00E7:
            RST_n(cpu, 0x20);
            break;
        case 0x00E8:
            ADD_SP_s8(cpu);
            break;
        case 0x00E9:
            JP_HL(cpu);
            break;
        case 0x00EA:
            LD_mema16_A(cpu);
            break;
        case 0x00EE:
            XOR_d8(cpu);
            break;
        case 0x00EF:
            RST_n(cpu, 0x28);
            break;
        case 0x00F0:
            LDH_A_mema8(cpu);
            break;
        case 0x00F1:
            POP_reg16(cpu, &cpu->regs.afReg.AF);
            break;
        case 0x00F2:
            LD_A_memC(cpu);
            break;
        case 0x00F3:
            DI(cpu);
            break;
        case 0x00F5:
            PUSH_reg16(cpu, cpu->regs.afReg.AF);
            break;
        case 0x00F6:
            OR_d8(cpu);
            break;
        case 0x00F7:
            RST_n(cpu, 0x30);
            break;
        case 0x00F8:
            LD_HL_SP_sum_s8(cpu);
            break;
        case 0x00F9:
            LD_SP_HL(cpu);
            break;
        case 0x00FA:
            LD_A_mema16(cpu);
            break;
        case 0x00FB:
            EI(cpu);
            break;
        case 0x00FE:
            CP_d8(cpu);
            break;
        case 0x00FF:
            RST_n(cpu, 0x38);
            break;
        case 0xCB00:
            RLC_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB01:
            RLC_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB02:
            RLC_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB03:
            RLC_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB04:
            RLC_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB05:
            RLC_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB06:
            RLC_memHL(cpu);
            break;
        case 0xCB07:
            RLC_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB08:
            RRC_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB09:
            RRC_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB0A:
            RRC_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB0B:
            RRC_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB0C:
            RRC_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB0D:
            RRC_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB0E:
            RRC_memHL(cpu);
            break;
        case 0xCB0F:
            RRC_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB10:
            RL_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB11:
            RL_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB12:
            RL_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB13:
            RL_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB14:
            RL_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB15:
            RL_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB16:
            RL_memHL(cpu);
            break;
        case 0xCB17:
            RL_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB18:
            RR_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB19:
            RR_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB1A:
            RR_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB1B:
            RR_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB1C:
            RR_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB1D:
            RR_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB1E:
            RR_memHL(cpu);
            break;
        case 0xCB1F:
            RR_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB20:
            SLA_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB21:
            SLA_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB22:
            SLA_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB23:
            SLA_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB24:
            SLA_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB25:
            SLA_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB26:
            SLA_memHL(cpu);
            break;
        case 0xCB27:
            SLA_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB28:
            SRA_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB29:
            SRA_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB2A:
            SRA_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB2B:
            SRA_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB2C:
            SRA_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB2D:
            SRA_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB2E:
            SRA_memHL(cpu);
            break;
        case 0xCB2F:
            SRA_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB30:
            SWAP_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB31:
            SWAP_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB32:
            SWAP_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB33:
            SWAP_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB34:
            SWAP_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB35:
            SWAP_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB36:
            SWAP_memHL(cpu);
            break;
        case 0xCB37:
            SWAP_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB38:
            SRL_reg8(cpu, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB39:
            SRL_reg8(cpu, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB3A:
            SRL_reg8(cpu, &cpu->regs.deReg.b.D);
            break;
        case 0xCB3B:
            SRL_reg8(cpu, &cpu->regs.deReg.b.E);
            break;
        case 0xCB3C:
            SRL_reg8(cpu, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB3D:
            SRL_reg8(cpu, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB3E:
            SRL_memHL(cpu);
            break;
        case 0xCB3F:
            SRL_reg8(cpu, &cpu->regs.afReg.b.A);
            break;
        case 0xCB40:
        case 0xCB50:
        case 0xCB60:
        case 0xCB70:
        case 0xCB48:
        case 0xCB58:
        case 0xCB68:
        case 0xCB78:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0040) >> 3, cpu->regs.bcReg.b.B);
            break;
        case 0xCB41:
        case 0xCB51:
        case 0xCB61:
        case 0xCB71:
        case 0xCB49:
        case 0xCB59:
        case 0xCB69:
        case 0xCB79:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0041) >> 3, cpu->regs.bcReg.b.C);
            break;
        case 0xCB42:
        case 0xCB52:
        case 0xCB62:
        case 0xCB72:
        case 0xCB4A:
        case 0xCB5A:
        case 0xCB6A:
        case 0xCB7A:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0042) >> 3, cpu->regs.deReg.b.D);
            break;
        case 0xCB43:
        case 0xCB53:
        case 0xCB63:
        case 0xCB73:
        case 0xCB4B:
        case 0xCB5B:
        case 0xCB6B:
        case 0xCB7B:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0043) >> 3, cpu->regs.deReg.b.E);
            break;
        case 0xCB44:
        case 0xCB54:
        case 0xCB64:
        case 0xCB74:
        case 0xCB4C:
        case 0xCB5C:
        case 0xCB6C:
        case 0xCB7C:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0044) >> 3, cpu->regs.hlReg.b.H);
            break;
        case 0xCB45:
        case 0xCB55:
        case 0xCB65:
        case 0xCB75:
        case 0xCB4D:
        case 0xCB5D:
        case 0xCB6D:
        case 0xCB7D:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0045) >> 3, cpu->regs.hlReg.b.L);
            break;
        case 0xCB46:
        case 0xCB56:
        case 0xCB66:
        case 0xCB76:
        case 0xCB4E:
        case 0xCB5E:
        case 0xCB6E:
        case 0xCB7E:
            BIT_n_memHL(cpu, ((cpu->opcode & 0x00FF) - 0x0046) >> 3);
            break;
        case 0xCB47:
        case 0xCB57:
        case 0xCB67:
        case 0xCB77:
        case 0xCB4F:
        case 0xCB5F:
        case 0xCB6F:
        case 0xCB7F:
            BIT_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0047) >> 3, cpu->regs.afReg.b.A);
            break;
        case 0xCB80:
        case 0xCB90:
        case 0xCBA0:
        case 0xCBB0:
        case 0xCB88:
        case 0xCB98:
        case 0xCBA8:
        case 0xCBB8:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0080) >> 3, &cpu->regs.bcReg.b.B);
            break;
        case 0xCB81:
        case 0xCB91:
        case 0xCBA1:
        case 0xCBB1:
        case 0xCB89:
        case 0xCB99:
        case 0xCBA9:
        case 0xCBB9:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0081) >> 3, &cpu->regs.bcReg.b.C);
            break;
        case 0xCB82:
        case 0xCB92:
        case 0xCBA2:
        case 0xCBB2:
        case 0xCB8A:
        case 0xCB9A:
        case 0xCBAA:
        case 0xCBBA:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0082) >> 3, &cpu->regs.deReg.b.D);
            break;
        case 0xCB83:
        case 0xCB93:
        case 0xCBA3:
        case 0xCBB3:
        case 0xCB8B:
        case 0xCB9B:
        case 0xCBAB:
        case 0xCBBB:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0083) >> 3, &cpu->regs.deReg.b.E);
            break;
        case 0xCB84:
        case 0xCB94:
        case 0xCBA4:
        case 0xCBB4:
        case 0xCB8C:
        case 0xCB9C:
        case 0xCBAC:
        case 0xCBBC:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0084) >> 3, &cpu->regs.hlReg.b.H);
            break;
        case 0xCB85:
        case 0xCB95:
        case 0xCBA5:
        case 0xCBB5:
        case 0xCB8D:
        case 0xCB9D:
        case 0xCBAD:
        case 0xCBBD:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0085) >> 3, &cpu->regs.hlReg.b.L);
            break;
        case 0xCB86:
        case 0xCB96:
        case 0xCBA6:
        case 0xCBB6:
        case 0xCB8E:
        case 0xCB9E:
        case 0xCBAE:
        case 0xCBBE:
            RES_n_memHL(cpu, ((cpu->opcode & 0x00FF) - 0x0086) >> 3);
            break;
        case 0xCB87:
        case 0xCB97:
        case 0xCBA7:
        case 0xCBB7:
        case 0xCB8F:
        case 0xCB9F:
        case 0xCBAF:
        case 0xCBBF:
            RES_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x0087) >> 3, &cpu->regs.afReg.b.A);
            break;
        case 0xCBC0:
        case 0xCBD0:
        case 0xCBE0:
        case 0xCBF0:
        case 0xCBC8:
        case 0xCBD8:
        case 0xCBE8:
        case 0xCBF8:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C0) >> 3, &cpu->regs.bcReg.b.B);
            break;
        case 0xCBC1:
        case 0xCBD1:
        case 0xCBE1:
        case 0xCBF1:
        case 0xCBC9:
        case 0xCBD9:
        case 0xCBE9:
        case 0xCBF9:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C1) >> 3, &cpu->regs.bcReg.b.C);
            break;
        case 0xCBC2:
        case 0xCBD2:
        case 0xCBE2:
        case 0xCBF2:
        case 0xCBCA:
        case 0xCBDA:
        case 0xCBEA:
        case 0xCBFA:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C2) >> 3, &cpu->regs.deReg.b.D);
            break;
        case 0xCBC3:
        case 0xCBD3:
        case 0xCBE3:
        case 0xCBF3:
        case 0xCBCB:
        case 0xCBDB:
        case 0xCBEB:
        case 0xCBFB:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C3) >> 3, &cpu->regs.deReg.b.E);
            break;
        case 0xCBC4:
        case 0xCBD4:
        case 0xCBE4:
        case 0xCBF4:
        case 0xCBCC:
        case 0xCBDC:
        case 0xCBEC:
        case 0xCBFC:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C4) >> 3, &cpu->regs.hlReg.b.H);
            break;
        case 0xCBC5:
        case 0xCBD5:
        case 0xCBE5:
        case 0xCBF5:
        case 0xCBCD:
        case 0xCBDD:
        case 0xCBED:
        case 0xCBFD:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C5) >> 3, &cpu->regs.hlReg.b.L);
            break;
        case 0xCBC6:
        case 0xCBD6:
        case 0xCBE6:
        case 0xCBF6:
        case 0xCBCE:
        case 0xCBDE:
        case 0xCBEE:
        case 0xCBFE:
            SET_n_memHL(cpu, ((cpu->opcode & 0x00FF) - 0x00C6) >> 3);
            break;
        case 0xCBC7:
        case 0xCBD7:
        case 0xCBE7:
        case 0xCBF7:
        case 0xCBCF:
        case 0xCBDF:
        case 0xCBEF:
        case 0xCBFF:
            SET_n_reg8(cpu, ((cpu->opcode & 0x00FF) - 0x00C7) >> 3, &cpu->regs.afReg.b.A);
            break;
    }
}

void cpu_reset(cpu_t* cpu) {
    cpu->SP = 0xFFFE;
    cpu->PC = 0x0100;
    cpu->fetched = 0x0000;
    cpu->opcode = 0x00;
    cpu->cycles = 0x00;
    cpu->stopped = 0x00;
    cpu->halted = 0x00;

    cpu->regs.afReg.AF = 0x01B0;
    cpu->regs.bcReg.BC = 0x0013;
    cpu->regs.deReg.DE = 0x00D8;
    cpu->regs.hlReg.HL = 0x014D;
}

//void cpu_clock(cpu_t* cpu) {
//    if (cpu->stopped) return;
//    if (cpu->halted && cpu->bus->interrupt->IME) return;
//
//    if (cpu->cycles == 0) {
//        printf("PC: 0x%04x,\tSP: 0x%04x,\t", cpu->PC, cpu->SP);
//
//        uint8_t tmpcode = cpu_read(cpu, cpu->PC++);
//        cpu->opcode = (tmpcode == 0xCB || tmpcode == 0x10)
//                ? ((tmpcode << 8) + cpu_read(cpu, cpu->PC++))
//                : tmpcode & 0x00FF;
//
//        printf("Opcode: 0x%04x,\t", cpu->opcode);
//
//        cpu_fetch(cpu);
//
//        printf("Fetched: 0x%04x,\t", cpu->fetched);
//
//        cpu_execute(cpu);
//        cpu->cycles <<= 2; // Scale machine cycles to clock cycles
//        cpu->regs.afReg.AF &= 0xFFF0; // Zero our LSN of AF register
//
//        printf("AF: 0x%04x,\tBC: 0x%04x,\tDE: 0x%04x,\tHL: 0x%04x,\t",
//               cpu->regs.afReg.AF, cpu->regs.bcReg.BC,
//               cpu->regs.deReg.DE, cpu->regs.hlReg.HL);
//
//        printf("CYC: %d\n", cpu->cycles);
//    }
//
//    cpu->cycles--;
//}

void cpu_clock(cpu_t* cpu) {
    if (cpu->stopped) return;
    if (cpu->halted && cpu->bus->interrupt->IME) return;

    uint8_t tmpcode = cpu_read(cpu, cpu->PC++);
    cpu->opcode = (tmpcode == 0xCB || tmpcode == 0x10)
                  ? ((tmpcode << 8) + cpu_read(cpu, cpu->PC++))
                  : tmpcode & 0x00FF;

    cpu_fetch(cpu);
    cpu_execute(cpu);

    // Scale machine cycle to clock cycle
    cpu->cycles *= 4;

    cpu->regs.afReg.AF &= 0xFFF0;
}

void NOP(cpu_t* cpu) {
}

void LD_reg16_d16(cpu_t* cpu, uint16_t* reg16) {
    *reg16 = cpu->fetched;
}

void LD_mem16_reg8(cpu_t* cpu, uint16_t memreg16, uint8_t reg8) {
    cpu_write(cpu, memreg16, reg8);
}

void INC_reg16(cpu_t* cpu, uint16_t* reg16) {
    (*reg16)++;
    cpu->cycles += 1;
}

void INC_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(Z_FLAG | N_FLAG | H_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8(*reg8, 1));
    (*reg8)++;
    SHIFT_SET(Z_SHFT, *reg8 == 0x00);
}

void DEC_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(Z_FLAG | H_FLAG);
    FLAGS_SET(N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8_SUB(*reg8, 1));
    (*reg8)--;
    SHIFT_SET(Z_SHFT, *reg8 == 0x00);
}

void LD_reg8_d8(cpu_t* cpu, uint8_t* reg8) {
    *reg8 = (uint8_t) (cpu->fetched & 0x00FF);
}

void RLCA(cpu_t* cpu) {
    FLAGS_CLR(Z_FLAG | N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, cpu->regs.afReg.b.A & 0x80);
    cpu->regs.afReg.b.A <<= 1;
    cpu->regs.afReg.b.A |= FLAGS_ISSET(C_FLAG);
}

void LD_mema16_SP(cpu_t* cpu) {
    uint8_t lo = cpu->SP & 0x00FF;
    uint8_t hi = (cpu->SP & 0xFF00) >> 8;

    cpu_write(cpu, cpu->fetched, lo);
    cpu_write(cpu, cpu->fetched + 1, hi);
}

void ADD_HL_reg16(cpu_t* cpu, uint16_t reg16) {
    FLAGS_CLR(N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_16(cpu->regs.hlReg.HL, reg16));
    uint32_t temp = cpu->regs.hlReg.HL + reg16;
    SHIFT_SET(C_SHFT, temp & 0x10000);
    cpu->regs.hlReg.HL = temp & 0xFFFF;
    cpu->cycles += 1;
}

void LD_reg8_mem16(cpu_t* cpu, uint8_t* reg, uint16_t memreg16) {
    *reg = cpu_read(cpu, memreg16);
}

void DEC_reg16(cpu_t* cpu, uint16_t* reg16) {
    (*reg16)--;
    cpu->cycles += 1;
}

void RRCA(cpu_t* cpu) {
    FLAGS_CLR(Z_FLAG | N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, cpu->regs.afReg.b.A & 1);
    cpu->regs.afReg.b.A >>= 1;
    cpu->regs.afReg.b.A |= (FLAGS_ISSET(C_FLAG) << 7);
}

void STOP(cpu_t* cpu) {
    // TODO
    cpu->stopped = 1;
    cpu->cycles -= 1;
}

void RLA(cpu_t* cpu) {
    FLAGS_CLR(Z_FLAG | N_FLAG | H_FLAG);
    uint16_t temp = (uint16_t) cpu->regs.afReg.b.A << 1;
    temp |= FLAGS_ISSET(C_FLAG);
    SHIFT_SET(C_SHFT, temp & 0xFF00);
    cpu->regs.afReg.b.A = temp & 0x00FF;
}

void JR_s8(cpu_t* cpu) {
    cpu->PC += cpu->fetched;
    cpu->cycles += 1;
}

void RRA(cpu_t* cpu) {
    FLAGS_CLR(Z_FLAG | N_FLAG | H_FLAG);
    uint8_t temp = cpu->regs.afReg.b.A >> 1;
    temp |= FLAGS_ISSET(C_FLAG) << 7;
    SHIFT_SET(C_SHFT, cpu->regs.afReg.b.A & 1);
    cpu->regs.afReg.b.A = temp;
}

void JR_Nflag_s8(cpu_t* cpu, uint8_t flag) {
    uint8_t flag_set = FLAGS_ISSET(flag);
    cpu->PC += (!flag_set) ? cpu->fetched : 0;
    cpu->cycles += (!flag_set);
}

void DAA(cpu_t* cpu) {
    if (!FLAGS_ISSET(N_FLAG)) {
        if (FLAGS_ISSET(C_FLAG) || cpu->regs.afReg.b.A > 0x99) {
            cpu->regs.afReg.b.A += 0x60;
            FLAGS_SET(C_FLAG);
        }

        if (FLAGS_ISSET(H_FLAG) || (cpu->regs.afReg.b.A & 0x0F) > 0x09) {
            cpu->regs.afReg.b.A += 0x06;
        }
    }
    else {
        if (FLAGS_ISSET(C_FLAG)) {
            cpu->regs.afReg.b.A -= 0x60;
        }

        if (FLAGS_ISSET(H_FLAG)) {
            cpu->regs.afReg.b.A -= 0x06;
        }
    }

    SHIFT_SET(Z_SHFT, cpu->regs.afReg.b.A == 0);
    FLAGS_CLR(H_FLAG);
}

void JR_flag_s8(cpu_t* cpu, uint8_t flag) {
    uint8_t flag_set = FLAGS_ISSET(flag);
    cpu->PC += (flag_set) ? cpu->fetched : 0;
    cpu->cycles += (flag_set);
}

void CPL(cpu_t* cpu) {
    FLAGS_SET(N_FLAG | H_FLAG);
    cpu->regs.afReg.b.A = ~cpu->regs.afReg.b.A;
}

void INC_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    INC_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void DEC_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    DEC_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void LD_memHL_d8(cpu_t* cpu) {
    cpu_write(cpu, cpu->regs.hlReg.HL, (uint8_t) cpu->fetched);
}

void SCF(cpu_t* cpu) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    FLAGS_SET(C_FLAG);
}

void CCF(cpu_t* cpu) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, !FLAGS_ISSET(C_FLAG));
}

void LD_reg8_reg8(cpu_t* cpu, uint8_t* reg8to, uint8_t reg8from) {
    *reg8to = reg8from;
}

void HALT(cpu_t* cpu) {
    cpu->halted = 1;
}

void ADD_A_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_CLR(N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8(cpu->regs.afReg.b.A, reg8));
    uint16_t temp = (uint16_t) cpu->regs.afReg.b.A + (uint16_t) reg8;
    SHIFT_SET(C_SHFT, temp & 0xFF00);
    SHIFT_SET(Z_SHFT, (temp & 0x00FF) == 0);
    cpu->regs.afReg.b.A = temp & 0x00FF;
}

void ADD_A_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    ADD_A_reg8(cpu, read);
}

void ADC_A_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_CLR(N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8_3(cpu->regs.afReg.b.A, reg8, FLAGS_ISSET(C_FLAG)));
    uint16_t temp = (uint16_t) cpu->regs.afReg.b.A + (uint16_t) reg8 + (uint16_t) FLAGS_ISSET(C_FLAG);
    SHIFT_SET(C_SHFT, temp & 0xFF00);
    SHIFT_SET(Z_SHFT, (temp & 0x00FF) == 0);
    cpu->regs.afReg.b.A = temp & 0x00FF;
}

void ADC_A_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    ADC_A_reg8(cpu, read);
}

void SUB_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_SET(N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8_SUB(cpu->regs.afReg.b.A, reg8));
    uint16_t temp = (uint16_t) cpu->regs.afReg.b.A - (uint16_t) reg8;
    SHIFT_SET(C_SHFT, temp & 0xFF00);
    SHIFT_SET(Z_SHFT, (temp & 0x00FF) == 0);
    cpu->regs.afReg.b.A = temp & 0x00FF;
}

void SUB_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    SUB_reg8(cpu, read);
}

void SBC_A_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_SET(N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8_3_SUB(cpu->regs.afReg.b.A, reg8, FLAGS_ISSET(C_FLAG)));
    uint16_t temp = (uint16_t) cpu->regs.afReg.b.A + (uint16_t) -reg8 + (uint16_t) -FLAGS_ISSET(C_FLAG);
    SHIFT_SET(C_SHFT, temp & 0xFF00);
    SHIFT_SET(Z_SHFT, (temp & 0x00FF) == 0);
    cpu->regs.afReg.b.A = temp & 0x00FF;
}

void SBC_A_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    SBC_A_reg8(cpu, read);
}

void AND_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_CLR(N_FLAG | C_FLAG);
    FLAGS_SET(H_FLAG);
    cpu->regs.afReg.b.A &= reg8;
    SHIFT_SET(Z_SHFT, cpu->regs.afReg.b.A == 0);
}

void AND_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    AND_reg8(cpu, read);
}

void XOR_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG | C_FLAG);
    cpu->regs.afReg.b.A ^= reg8;
    SHIFT_SET(Z_SHFT, cpu->regs.afReg.b.A == 0);
}

void XOR_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    XOR_reg8(cpu, read);
}

void OR_reg8(cpu_t* cpu, uint8_t reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG | C_FLAG);
    cpu->regs.afReg.b.A |= reg8;
    SHIFT_SET(Z_SHFT, cpu->regs.afReg.b.A == 0);
}

void OR_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    OR_reg8(cpu, read);
}

void CP_reg8(cpu_t* cpu, uint8_t reg8) {
    SHIFT_SET(Z_SHFT, cpu->regs.afReg.b.A == reg8);
    FLAGS_SET(N_FLAG);
    SHIFT_SET(H_SHFT, (cpu->regs.afReg.b.A & 0xF) < (reg8 & 0xF));
    SHIFT_SET(C_SHFT, cpu->regs.afReg.b.A < reg8);
}

void CP_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    CP_reg8(cpu, read);
}

void RET_Nflag(cpu_t* cpu, uint8_t flag) {
    if (!FLAGS_ISSET(flag)) {
        POP_reg16(cpu, &cpu->PC);
        cpu->cycles += 2;
    } else cpu->cycles += 1;
}

void POP_reg16(cpu_t* cpu, uint16_t* reg16) {
    uint16_t lo = cpu_read(cpu, cpu->SP++);
    uint16_t hi = cpu_read(cpu, cpu->SP++);
    *reg16 = (hi << 8) | lo;
}

void JP_Nflag_a16(cpu_t* cpu, uint8_t flag) {
    if (!FLAGS_ISSET(flag)) JP_a16(cpu);
}

void JP_a16(cpu_t* cpu) {
    cpu->PC = cpu->fetched;
    cpu->cycles += 1;
}

void CALL_Nflag_a16(cpu_t* cpu, uint8_t flag) {
    if (!FLAGS_ISSET(flag)) CALL_a16(cpu);
}

void PUSH_reg16(cpu_t* cpu, uint16_t reg16) {
    uint8_t hi = (reg16 & 0xFF00) >> 8;
    uint8_t lo = reg16 & 0x00FF;
    cpu_write(cpu, --cpu->SP, hi);
    cpu_write(cpu, --cpu->SP, lo);
    cpu->cycles += 1;
}

void ADD_A_d8(cpu_t* cpu) {
    ADD_A_reg8(cpu, (uint8_t) cpu->fetched);
}

void RST_n(cpu_t* cpu, uint8_t n) {
    PUSH_reg16(cpu, cpu->PC);
    cpu->PC = n;
}

void RET_flag(cpu_t* cpu, uint8_t flag) {
    if (FLAGS_ISSET(flag)) {
        POP_reg16(cpu, &cpu->PC);
        cpu->cycles += 2;
    } else cpu->cycles += 1;
}

void RET(cpu_t* cpu) {
    POP_reg16(cpu, &cpu->PC);
    cpu->cycles += 2 ;
}

void JP_flag_a16(cpu_t* cpu, uint8_t flag) {
    if (FLAGS_ISSET(flag)) JP_a16(cpu);
}

void CALL_flag_a16(cpu_t* cpu, uint8_t flag) {
    if (FLAGS_ISSET(flag)) CALL_a16(cpu);
}

void CALL_a16(cpu_t* cpu) {
    PUSH_reg16(cpu, cpu->PC);
    cpu->PC = cpu->fetched;
}

void ADC_A_d8(cpu_t* cpu) {
    ADC_A_reg8(cpu, (uint8_t) cpu->fetched);
}

void SUB_d8(cpu_t* cpu) {
    SUB_reg8(cpu, (uint8_t) cpu->fetched);
}

void RETI(cpu_t* cpu) {
    interrupt_set_IME(cpu->bus->interrupt, 1);
    RET(cpu);
}

void SBC_A_d8(cpu_t* cpu) {
    SBC_A_reg8(cpu, (uint8_t) cpu->fetched);
}

void LDH_mema8_A(cpu_t* cpu) {
    cpu_write(cpu, 0xFF00 | (cpu->fetched & 0x00FF), cpu->regs.afReg.b.A);
}

void LD_memC_A(cpu_t* cpu) {
    cpu_write(cpu, 0xFF00 | cpu->regs.bcReg.b.C, cpu->regs.afReg.b.A);
}

void AND_d8(cpu_t* cpu) {
    AND_reg8(cpu, (uint8_t) cpu->fetched);
}

uint16_t SP_sum_s8(cpu_t* cpu) {
    FLAGS_CLR(Z_FLAG | N_FLAG);
    SHIFT_SET(H_SHFT, IS_HALF_CARRY_8(cpu->SP, cpu->fetched));
    SHIFT_SET(C_SHFT, IS_CARRY_16_S8(cpu->SP, cpu->fetched));

    uint16_t temp = cpu->SP + cpu->fetched;

    cpu->cycles += 1;
    return temp;
}

void ADD_SP_s8(cpu_t* cpu) {
    cpu->SP = SP_sum_s8(cpu);
    cpu->cycles += 1;
}

void JP_HL(cpu_t* cpu) {
    cpu->PC = cpu->regs.hlReg.HL;
}

void LD_mema16_A(cpu_t* cpu) {
    cpu_write(cpu, cpu->fetched, cpu->regs.afReg.b.A);
}

void XOR_d8(cpu_t* cpu) {
    XOR_reg8(cpu, (uint8_t) cpu->fetched);
}

void LDH_A_mema8(cpu_t* cpu) {
    cpu->regs.afReg.b.A = cpu_read(cpu, 0xFF00 + (cpu->fetched & 0x00FF));
}

void LD_A_memC(cpu_t* cpu) {
    cpu->regs.afReg.b.A = cpu_read(cpu, 0xFF00 + cpu->regs.bcReg.b.C);
}

void DI(cpu_t* cpu) {
    interrupt_set_IME(cpu->bus->interrupt, 0);
}

void OR_d8(cpu_t* cpu) {
    OR_reg8(cpu, (uint8_t) cpu->fetched);
}

void LD_HL_SP_sum_s8(cpu_t* cpu) {
    cpu->regs.hlReg.HL = SP_sum_s8(cpu);
}

void LD_SP_HL(cpu_t* cpu) {
    cpu->SP = cpu->regs.hlReg.HL;
    cpu->cycles += 1;
}

void LD_A_mema16(cpu_t* cpu) {
    cpu->regs.afReg.b.A = cpu_read(cpu, cpu->fetched);
}

void EI(cpu_t* cpu) {
    interrupt_set_IME(cpu->bus->interrupt, 1);
}

void CP_d8(cpu_t* cpu) {
    CP_reg8(cpu, (uint8_t) cpu->fetched);
}

void RLC_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR( N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, *reg8 & 0x80);
    *reg8 <<= 1;
    *reg8 |= FLAGS_ISSET(C_FLAG);
    SHIFT_SET(Z_SHFT, *reg8 == 0);
}

void RLC_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    RLC_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void RRC_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, *reg8 & 1);
    *reg8 >>= 1;
    *reg8 |= (FLAGS_ISSET(C_FLAG) << 7);
    SHIFT_SET(Z_SHFT, *reg8 == 0);
}

void RRC_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    RRC_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void RL_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    uint16_t temp = (uint16_t) (*reg8) << 1;
    temp |= FLAGS_ISSET(C_FLAG);
    SHIFT_SET(C_SHFT, temp & 0xFF00);
    *reg8 = temp & 0x00FF;
    SHIFT_SET(Z_SHFT, *reg8 == 0);
}

void RL_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    RL_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void RR_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    uint8_t temp = (*reg8) >> 1;
    temp |= FLAGS_ISSET(C_FLAG) << 7;
    SHIFT_SET(C_SHFT, *reg8 & 1);
    *reg8 = temp;
    SHIFT_SET(Z_SHFT, *reg8 == 0);
}

void RR_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    RR_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void SLA_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, (*reg8) & 0x80);
    *reg8 <<= 1;
    SHIFT_SET(Z_SHFT, (*reg8) == 0);
}

void SLA_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    SLA_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void SRA_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, (*reg8) & 1);
    *reg8 >>= 1;
    *reg8 = (*reg8 & ~(1UL << 7)) | (((*reg8 & 0x40)?1:0) << 7);
    SHIFT_SET(Z_SHFT, (*reg8) == 0);
}

void SRA_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    SRA_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void SWAP_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG | C_FLAG);
    *reg8 = (((*reg8) & 0x0F) << 4) | (((*reg8) & 0xF0) >> 4);
    SHIFT_SET(Z_SHFT, *reg8 == 0);
}

void SWAP_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    SWAP_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void SRL_reg8(cpu_t* cpu, uint8_t* reg8) {
    FLAGS_CLR(N_FLAG | H_FLAG);
    SHIFT_SET(C_SHFT, (*reg8) & 1);
    *reg8 >>= 1;
    SHIFT_SET(Z_SHFT, (*reg8) == 0);
}

void SRL_memHL(cpu_t* cpu) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    SRL_reg8(cpu, &read);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void BIT_n_reg8(cpu_t* cpu, uint8_t n, uint8_t reg8) {
    SHIFT_SET(Z_SHFT, !((reg8 >> n) & 1));
    FLAGS_CLR(N_FLAG);
    FLAGS_SET(H_FLAG);
}

void BIT_n_memHL(cpu_t* cpu, uint8_t n) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    BIT_n_reg8(cpu, n, read);
}

void RES_n_reg8(cpu_t* cpu, uint8_t n, uint8_t* reg8) {
    *reg8 &= ~(1 << n);
}

void RES_n_memHL(cpu_t* cpu, uint8_t n) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    read &= ~(1 << n);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}

void SET_n_reg8(cpu_t* cpu, uint8_t n, uint8_t* reg8) {
    *reg8 |= (1 << n);
}

void SET_n_memHL(cpu_t* cpu, uint8_t n) {
    uint8_t read = cpu_read(cpu, cpu->regs.hlReg.HL);
    read |= (1 << n);
    cpu_write(cpu, cpu->regs.hlReg.HL, read);
}
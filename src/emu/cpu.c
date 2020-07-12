/* ************************************************************************* */
/* CPU.C - Processor                                                         */
/* All state is kept in this module for easy access                          */
/* ************************************************************************* */
#include "psx.h"


extern u32 REG_CPU[32+4];
extern u32 REG_GTE[32];
extern u32 REG_COP0[32];


#define CPU_OPCODE     0xFC000000
#define CPU_RS         0x03E00000
#define CPU_RT         0x001F0000
#define CPU_IMM        0x0000FFFF
#define CPU_RD         0x0000F800
#define CPU_SA         0x000007C0
#define CPU_FUNCTION   0x0000003F

static u32 pc;
static u32 delay;
static u32 opcode;
static u32 rs;
static u32 rt;
static u32 rd;
static u32 sa;
static u32 uimm;
static u32 simm;


static u32 SLL(u32 opcode);
static u32 SRL(u32 opcode);
static u32 SRA(u32 opcode);
static u32 SLLV(u32 opcode);
static u32 SRLV(u32 opcode);
static u32 SRAV(u32 opcode);
static u32 JR(u32 opcode);
static u32 JALR(u32 opcode);
static u32 SYSCALL(u32 opcode);
static u32 BREAK(u32 opcode);
static u32 MFHI(u32 opcode);
static u32 MTHI(u32 opcode);
static u32 MFLO(u32 opcode);
static u32 MTLO(u32 opcode);
static u32 MULT(u32 opcode);
static u32 MULTU(u32 opcode);
static u32 DIV(u32 opcode);
static u32 DIVU(u32 opcode);
static u32 ADD(u32 opcode);
static u32 ADDU(u32 opcode);
static u32 SUB(u32 opcode);
static u32 SUBU(u32 opcode);
static u32 AND(u32 opcode);
static u32 OR(u32 opcode);
static u32 XOR(u32 opcode);
static u32 NOR(u32 opcode);
static u32 SLT(u32 opcode);
static u32 SLTU(u32 opcode);
static u32 EXCEPTION(u32 err);
static u32 BCOND(u32 opcode);
static u32 J(u32 opcode);
static u32 JAL(u32 opcode);
static u32 BEQ(u32 opcode);
static u32 BNE(u32 opcode);
static u32 BLEZ(u32 opcode);
static u32 BGTZ(u32 opcode);
static u32 ADDI(u32 opcode);
static u32 ADDIU(u32 opcode);
static u32 SLTI(u32 opcode);
static u32 SLTIU(u32 opcode);
static u32 ANDI(u32 opcode);
static u32 ORI(u32 opcode);
static u32 XORI(u32 opcode);
static u32 LUI(u32 opcode);
static u32 MFC0(u32 opcode);
static u32 MTC0(u32 opcode);
static u32 RFE(u32 opcode);
static u32 COP1(u32 opcode);
static u32 MFC2(u32 opcode);
static u32 CFC2(u32 opcode);
static u32 MTC2(u32 opcode);
static u32 CTC2(u32 opcode);
static u32 GTE_Execute(u32 opcode);
static u32 COP3(u32 opcode);
static u32 LB(u32 opcode);
static u32 LH(u32 opcode);
static u32 LWL(u32 opcode);
static u32 LW(u32 opcode);
static u32 LBU(u32 opcode);
static u32 LHU(u32 opcode);
static u32 LWR(u32 opcode);
static u32 SB(u32 opcode);
static u32 SH(u32 opcode);
static u32 SWL(u32 opcode);
static u32 SW(u32 opcode);
static u32 SWR(u32 opcode);
static u32 LWC0(u32 opcode);
static u32 LWC1(u32 opcode);
static u32 LWC2(u32 opcode);
static u32 LWC3(u32 opcode);
static u32 SWC0(u32 opcode);
static u32 SWC1(u32 opcode);
static u32 SWC2(u32 opcode);
static u32 SWC3(u32 opcode);


API int ABI CPU_Reset(void) {
    BIOS_InsertRom("bios/scph1001.bin");
    BUS_Reset();
    return 1;
}


API int ABI CPU_Step(void) {
    pc = BUS_GetReg(PC);
    opcode = BUS_Get32(pc);
    rs    = (opcode >> 21) & 0x1F;
    rt    = (opcode >> 16) & 0x1F;
    rd    = (opcode >> 11) & 0x1F;
    sa    = (opcode >>  6) & 0x1F;
    uimm  = opcode & 0xFFFF;
    simm  = (int16_t)opcode;

    switch (opcode & CPU_OPCODE) {
    case 0x00000000:
        switch (opcode & CPU_FUNCTION) {
        case 0x00000000: SLL(opcode); break;
        case 0x00000002: SRL(opcode); break;
        case 0x00000003: SRA(opcode); break;
        case 0x00000004: SLLV(opcode); break;
        case 0x00000006: SRLV(opcode); break;
        case 0x00000007: SRAV(opcode); break;
        case 0x00000008: JR(opcode); break;
        case 0x00000009: JALR(opcode); break;
        case 0x0000000C: SYSCALL(opcode); break;
        case 0x0000000D: BREAK(opcode); break;
        case 0x00000010: MFHI(opcode); break;
        case 0x00000011: MTHI(opcode); break;
        case 0x00000012: MFLO(opcode); break;
        case 0x00000013: MTLO(opcode); break;
        case 0x00000018: MULT(opcode); break;
        case 0x00000019: MULTU(opcode); break;
        case 0x0000001A: DIV(opcode); break;
        case 0x0000001B: DIVU(opcode); break;
        case 0x00000020: ADD(opcode); break;
        case 0x00000021: ADDU(opcode); break;
        case 0x00000022: SUB(opcode); break;
        case 0x00000023: SUBU(opcode); break;
        case 0x00000024: AND(opcode); break;
        case 0x00000025: OR(opcode); break;
        case 0x00000026: XOR(opcode); break;
        case 0x00000027: NOR(opcode); break;
        case 0x0000002A: SLT(opcode); break;
        case 0x0000002B: SLTU(opcode); break;
        default:
            EXCEPTION(ILLEGAL_OPCODE);
            break;
        }
        break;
    case 0x04000000: BCOND(opcode); break;
    case 0x08000000: J(opcode); break;
    case 0x0C000000: JAL(opcode); break;
    case 0x10000000: BEQ(opcode); break;
    case 0x14000000: BNE(opcode); break;
    case 0x18000000: BLEZ(opcode); break;
    case 0x1C000000: BGTZ(opcode); break;
    case 0x20000000: ADDI(opcode); break;
    case 0x24000000: ADDIU(opcode); break;
    case 0x28000000: SLTI(opcode); break;
    case 0x2C000000: SLTIU(opcode); break;
    case 0x30000000: ANDI(opcode); break;
    case 0x34000000: ORI(opcode); break;
    case 0x38000000: XORI(opcode); break;
    case 0x3C000000: LUI(opcode); break;
    case 0x40000000:
        switch (opcode & CPU_RS) {
        case 0x00000000: MFC0(opcode); break;
        case 0x00800000: MTC0(opcode); break;
        case 0x02000000: RFE(opcode); break;
        default: EXCEPTION(ILLEGAL_OPCODE); break;
        }
        break;
    case 0x44000000: /*COP1(opcode);*/ break;
    case 0x48000000:
        if ((opcode & 0x0200000) == 0) {
            switch (opcode & CPU_RS) {
            case 0x00000000: MFC2(opcode); break;
            case 0x00400000: CFC2(opcode); break;
            case 0x00800000: MTC2(opcode); break;
            case 0x00C00000: CTC2(opcode); break;
            default: EXCEPTION(ILLEGAL_OPCODE); break;
            }
        } else {
            GTE_Execute(opcode);
        }
        break;
    case 0x4C000000: /*COP3(opcode);*/ break;
    case 0x80000000: LB(opcode); break;
    case 0x84000000: LH(opcode); break;
    case 0x88000000: LWL(opcode); break;
    case 0x8C000000: LW(opcode); break;
    case 0x90000000: LBU(opcode); break;
    case 0x94000000: LHU(opcode); break;
    case 0x98000000: LWR(opcode); break;
    case 0xA0000000: SB(opcode); break;
    case 0xA4000000: SH(opcode); break;
    case 0xA8000000: SWL(opcode); break;
    case 0xAC000000: SW(opcode); break;
    case 0xB8000000: SWR(opcode); break;
    case 0xC0000000: //LWC0(opcode);
    case 0xC4000000: //LWC1(opcode);
    case 0xC8000000: LWC2(opcode); break;
    case 0xCC000000: //LWC3(opcode);
    case 0xE0000000: //SWC0(opcode);
    case 0xE4000000: //SWC1(opcode);
    case 0xE8000000: SWC2(opcode); break;
    case 0xEC000000: /*SWC3(opcode);*/ break;
    default:
        EXCEPTION(ILLEGAL_OPCODE);
        break;
    }
    return 1;
}


static u32 SLL(u32 opcode) {
    REG_CPU[rd] = REG_CPU[rt] << (int)sa;
    return 0;
}

static u32 SRL(u32 opcode) { return 0; }
static u32 SRA(u32 opcode) { return 0; }
static u32 SLLV(u32 opcode) { return 0; }
static u32 SRLV(u32 opcode) { return 0; }
static u32 SRAV(u32 opcode) { return 0; }
static u32 JR(u32 opcode) { return 0; }
static u32 JALR(u32 opcode) { return 0; }
static u32 SYSCALL(u32 opcode) { return 0; }
static u32 BREAK(u32 opcode) { return 0; }
static u32 MFHI(u32 opcode) { return 0; }
static u32 MTHI(u32 opcode) { return 0; }
static u32 MFLO(u32 opcode) { return 0; }
static u32 MTLO(u32 opcode) { return 0; }
static u32 MULT(u32 opcode) { return 0; }
static u32 MULTU(u32 opcode) { return 0; }
static u32 DIV(u32 opcode) { return 0; }
static u32 DIVU(u32 opcode) { return 0; }

static u32 ADD(u32 opcode) {
    u64 val = REG_CPU[rs] + REG_CPU[rt];
    if (val > 0xFFFFFFFF) {
        EXCEPTION(OVERFLOW);
    } else {
        REG_CPU[rt] = (u32)val;
    }
    return delay = pc + 4;
}

static u32 ADDU(u32 opcode) {
    REG_CPU[rd] = REG_CPU[rs] + REG_CPU[rt];
    return delay = pc + 4;
}

static u32 SUB(u32 opcode) {
    u64 val = REG_CPU[rs] - REG_CPU[rt];
    if (val > 0xFFFFFFFF) {
        EXCEPTION(OVERFLOW);
    } else {
        REG_CPU[rt] = (u32)val;
    }
    return delay = pc + 4;

}

static u32 SUBU(u32 opcode) {
    REG_CPU[rd] = REG_CPU[rs] - REG_CPU[rt];
    return delay = pc + 4;
}

static u32 AND(u32 opcode) {
    REG_CPU[rd] = REG_CPU[rs] & REG_CPU[rt];
    return delay = pc + 4;
}

static u32 OR(u32 opcode) {
    REG_CPU[rd] = REG_CPU[rs] | REG_CPU[rt];
    return delay = pc + 4;
}

static u32 XOR(u32 opcode) {
    REG_CPU[rd] = REG_CPU[rs] ^ REG_CPU[rt];
    return delay = pc + 4;
}

static u32 NOR(u32 opcode) { return 0; }
static u32 SLT(u32 opcode) { return 0; }
static u32 SLTU(u32 opcode) { return 0; }
static u32 EXCEPTION(u32 err) { return 0; }
static u32 BCOND(u32 opcode) { return 0; }

static u32 J(u32 opcode) {
    u32 addr = (opcode & 0x03FFFFFF) << 2;
    return delay = (delay & 0xF0000000) | addr;
}

static u32 JAL(u32 opcode) { return 0; }
static u32 BEQ(u32 opcode) { return 0; }

static u32 BNE(u32 opcode) {
    //opcodeIsBranch = true;
    if (REG_CPU[rs] != REG_CPU[rt]) {
        //BRANCH();
    }
    return delay = pc + 4;
}

static u32 BLEZ(u32 opcode) { return 0; }
static u32 BGTZ(u32 opcode) { return 0; }

static u32 ADDI(u32 opcode) {
    u64 val = REG_CPU[rs] + simm;
    if (val > 0xFFFFFFFF) {
        EXCEPTION(OVERFLOW);
    } else {
        REG_CPU[rt] = (u32)val;
    }
    return delay = pc + 4;
}

static u32 ADDIU(u32 opcode) {
    REG_CPU[rt] = REG_CPU[rs] + simm;
    return delay = pc + 4;
}

static u32 SLTI(u32 opcode) { return 0; }
static u32 SLTIU(u32 opcode) { return 0; }

static u32 ANDI(u32 opcode) {
    REG_CPU[rt] = REG_CPU[rs] & uimm;
    return delay = pc + 4;
}

static u32 ORI(u32 opcode) {
    REG_CPU[rt] = REG_CPU[rs] | uimm;
    return delay = pc + 4;
}

static u32 XORI(u32 opcode) {
    REG_CPU[rt] = REG_CPU[rs] ^ uimm;
    return delay = pc + 4;
}

static u32 LUI(u32 opcode) {
    REG_CPU[rt] = uimm << 16;
    return delay = pc + 4;
}

static u32 MFC0(u32 opcode) { return 0; }

static u32 MTC0(u32 opcode) {
    u32 val = REG_CPU[rt];
    u32 reg = rd;
    int iec = (REG_COP0[SR] & 1);
    if (reg == CAUSE) {
        REG_COP0[CAUSE] &= ~0x00000300;
        REG_COP0[CAUSE] |= val & 0x00000300;
    } else {
        REG_COP0[reg] = val;
    }

    u32 im = (REG_COP0[SR] >> 8) & 0x3;
    u32 ip = (REG_COP0[CAUSE] >> 8) & 0x3;

    if ((!iec) && ((REG_COP0[SR] & 0x1) == 1) && ((im & ip) > 0)) {
        pc = delay;
        EXCEPTION(INTERRUPT);
    }
    return delay = pc + 4;
}

static u32 RFE(u32 opcode) { return 0; }
static u32 COP1(u32 opcode) { return 0; }
static u32 MFC2(u32 opcode) { return 0; }
static u32 CFC2(u32 opcode) { return 0; }
static u32 MTC2(u32 opcode) { return 0; }
static u32 CTC2(u32 opcode) { return 0; }
static u32 GTE_Execute(u32 opcode) { return 0; }
static u32 COP3(u32 opcode) { return 0; }
static u32 LB(u32 opcode) { return 0; }
static u32 LH(u32 opcode) { return 0; }
static u32 LWL(u32 opcode) { return 0; }

static u32 LW(u32 opcode) {
    if ((REG_COP0[SR] & 0x10000) == 0) {
        u32 addr = REG_CPU[rs] + simm;
        if ((addr & 0x3) == 0) {
            u32 val = BUS_Get32(addr);
            //delayedLoad(instr.rt, value);
            REG_CPU[rt] = val;
        } else {
            REG_COP0[BADA] = addr;
            EXCEPTION(BAD_ADDR);
        }
    }
    return 0;
}

static u32 LBU(u32 opcode) { return 0; }
static u32 LHU(u32 opcode) { return 0; }
static u32 LWR(u32 opcode) { return 0; }
static u32 SB(u32 opcode) { return 0; }
static u32 SH(u32 opcode) { return 0; }
static u32 SWL(u32 opcode) { return 0; }

static u32 SW(u32 opcode) {
    if ((REG_COP0[SR] & 0x00010000) == 0) {
        u32 addr = REG_CPU[rs] + simm;
        if ((addr & 0x3) == 0) {
            BUS_Set32(addr, REG_CPU[rt]);
        } else {
            REG_COP0[BADA] = addr;
            EXCEPTION(BAD_ADDR);
        }
    }
    return delay = pc + 4;
}

static u32 SWR(u32 opcode) { return 0; }
static u32 LWC0(u32 opcode) { return 0; }
static u32 LWC1(u32 opcode) { return 0; }
static u32 LWC2(u32 opcode) { return 0; }
static u32 LWC3(u32 opcode) { return 0; }
static u32 SWC0(u32 opcode) { return 0; }
static u32 SWC1(u32 opcode) { return 0; }

static u32 SWC2(u32 opcode) {
    u32 addr = REG_CPU[rs] + simm;
    if ((addr & 0x3) == 0) {
        BUS_Set32(addr, REG_GTE[rt]);
    } else {
        REG_COP0[BADA] = addr;
        EXCEPTION(BAD_ADDR);
    }
    return delay = pc + 4;
}

static u32 SWC3(u32 opcode) { return 0; }

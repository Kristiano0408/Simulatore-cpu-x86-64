
// -----------------------------------------------------------------------------
// AND,NOT,OR,XOR 
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"



std::vector<TestCase> buildBooleanTests() {

    std::vector<TestCase> tests{};


    // ==============================================================================
    // AND - immediate to accumulator (I mode)
    // =====================================================================================

    tests.push_back({
        .description = "AND AL, imm8",
        .program = {0x24, 0x05},
        .initial_regs = {{Register::RAX, 0xA3}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x01}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::PF, false}, 
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }

    });

    tests.push_back({
        .description = "AND AX, imm16",
        .program = {0x66, 0x25, 0x6D},
        .initial_regs = {{Register::RAX, 0xD7}},
        .initial_flags = {{Flagbit::CF, true}},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x45}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::PF, false}, 
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }

    });

    tests.push_back({
        .description = "AND EAX, imm32",
        .program = {0x25,  0x75, 0x0F, 0x3F, 0x6D},
        .initial_regs = {{Register::RAX, 0xD7A36C5F}},
        .initial_flags = {{Flagbit::CF, true}},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x45230C55}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::PF, true}, 
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }

    });

    tests.push_back({
        .description = "AND RAX, imm32 sign extended 0",
        .program = {0x48, 0x25,  0x75, 0x0F, 0x3F, 0x6D},
        .initial_regs = {{Register::RAX, 0xFFFFFFFFD7A36C5F}},
        .initial_flags = {{Flagbit::CF, true}},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x45230C55}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::PF, true}, 
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }

    });

    tests.push_back({
        .description = "AND RAX, imm32 sign extended 1",
        .program = {0x48, 0x25, 0x75, 0x0F, 0x3F, 0xED},
        .initial_regs = {{Register::RAX, 0xFFFFFFFFD7A36C5F}},
        .initial_flags = {{Flagbit::CF, true}},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xFFFFFFFFC5230C55}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::PF, true},
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // =====================================================================================
    // AND - immediate to r/m (MI mode: 80 /4, 81 /4, 83 /4)
    // =====================================================================================

    tests.push_back({
        .description = "AND CL, imm8 (result zero -> ZF=1)",
        .program = {0x80, 0xE1, 0x0C}, // 80 /4 ib: AND r/m8, imm8 -> CL &= 0x0C
        .initial_regs = {{Register::RCX, 0xF3}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RCX, 0x00}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    tests.push_back({
        .description = "AND CL, imm8 (sign extended negative imm -> SF=1)",
        .program = {0x80, 0xE1, 0xF0}, // AND CL, 0xF0
        .initial_regs = {{Register::RCX, 0x3C}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RCX, 0x30}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, false}, // bit 7 del risultato (8-bit) = 0
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x30: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "AND EBX, imm32 (81 /4 id)",
        .program = {0x81, 0xE3, 0x0F, 0x00, 0x00, 0x80}, // AND EBX, 0x8000000F
        .initial_regs = {{Register::RBX, 0xFFFFFFFFFFFFFFFFULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0x8000000FULL}}, // 32-bit: upper bits zeroed
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 31 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0x0F: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "AND RDI, imm8 sign extended (83 /4 ib, REX.W)",
        .program = {0x48, 0x83, 0xE7, 0xF0}, // REX.W: AND RDI, -16 (imm8 -> 0xFFFFFFFFFFFFFFF0)
        .initial_regs = {{Register::RDI, 0x0000000012345678ULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RDI, 0x0000000012345670ULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, false}, // bit 63 del risultato = 0
            {Flagbit::OF, false},
            {Flagbit::PF, false} // byte basso 0x70: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "AND byte in memory [RSI] (80 /4 ) + flags",
        .program = {0x80, 0x26, 0x3C}, // AND BYTE PTR [RSI], 0x3C
        .initial_regs = {{Register::RSI, 0x2000}},
        .initial_flags = {},
        .initial_mem = {{0x2000, 0xFF}},
        .expected_regs = {},
        .expected_mem = {{0x2000, 0x3C}},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x3C: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "AND qword [RBX+RCX*4+0x10], imm8 (83 /4, REX.W, SIB)",

        // 48 83 A4 8B 10 00 00 00 F0
        //
        // REX  : 48 -> W=1 (operand size 64 bit)
        // Opcode: 83 -> Group 1, imm8 sign-extended
        //
        // ModRM: 10 100 100
        //        mod=10  -> disp32
        //        reg=100 -> /4 (AND)
        //        r/m=100 -> SIB
        //
        // SIB  : 10 001 011
        //        scale=10  -> *4
        //        index=001 -> RCX
        //        base=011  -> RBX
        //
        // disp32: 10 00 00 00 -> 0x10
        // imm8  : F0 -> sign-extended to 64 bit

        .program = {0x48, 0x83, 0xA4, 0x8B, 0x10, 0x00, 0x00, 0x00, 0xF0},

        .initial_regs = {
            {Register::RBX, 0x1000},
            {Register::RCX, 0x800}
        },

        .initial_flags = {},

        // EA = 0x1000 + 0x800*4 + 0x10 = 0x3010
        .initial_mem = {
            {0x3010, 0xFFu}, {0x3011, 0xFFu},
            {0x3012, 0xFFu}, {0x3013, 0xFFu},
            {0x3014, 0xFFu}, {0x3015, 0xFFu},
            {0x3016, 0xFFu}, {0x3017, 0xFFu}
        },

        .expected_regs = {},

        // FF FF FF FF FF FF FF FF
        // AND
        // FF FF FF FF FF FF FF F0
        // =
        // FF FF FF FF FF FF FF F0
        .expected_mem = {
            {0x3010, 0xF0u}, {0x3011, 0xFFu},
            {0x3012, 0xFFu}, {0x3013, 0xFFu},
            {0x3014, 0xFFu}, {0x3015, 0xFFu},
            {0x3016, 0xFFu}, {0x3017, 0xFFu}
        },

        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // =====================================================================================
    // AND - register/memory forms (MR/RM: 20-23)
    // =====================================================================================

    tests.push_back({
        .description = "AND RBX, RAX (REX.W + 21 /r, MR: dest=r/m)",
        // ModRM: 11 000 011 -> mod=11, reg=0 (RAX source), r/m=3 (RBX dest)
        .program = {0x48, 0x21, 0xC3},
        .initial_regs = {{Register::RAX, 0x00000000FFFFFFFFULL}, {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0x00000000FFFFFFFFULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, false}, // bit 63 del risultato = 0
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0xFF: 8 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "AND RAX, [RSI] (REX.W + 23 /r, RM: dest=reg, src=mem)",
        // ModRM: 00 000 110 -> mod=00, reg=0 (RAX dest), r/m=110 (RSI) -> [RSI]
        .program = {0x48, 0x23, 0x06},
        .initial_regs = {{Register::RAX, 0xFFFFFFFFFFFFFFFFULL}, {Register::RSI, 0x4000}},
        .initial_flags = {},
        .initial_mem = {{0x4000, 0x0Fu}, {0x4001, 0x00u}, {0x4002, 0x00u}, {0x4003, 0x80u},
                        {0x4004, 0x00u}, {0x4005, 0x00u}, {0x4006, 0x00u}, {0x4007, 0x00u}}, // qword 0x800000000000000F
        .expected_regs = {{Register::RAX, 0x8000000FULL}},
        .expected_mem = {}, // memoria letta sola: invariata
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, false}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0x0F: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "AND AL, BL (20 /r, 8-bit)",
        // ModRM: 11 000 011 -> reg=RAX(source), r/m=RBX(dest)
        .program = {0x20, 0xC3},
        .initial_regs = {{Register::RAX, 0xF0}, {Register::RBX, 0x0F}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0x00}}, // AL = AL & BL = 0xF0 & 0x0F = 0
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    // =====================================================================================
    // OR - immediate to accumulator (I mode: 0C / ib, 0D / id)
    // =====================================================================================

    tests.push_back({
        .description = "OR AL, imm8",
        .program = {0x0C, 0x0F}, // OR AL, 0x0F
        .initial_regs = {{Register::RAX, 0xF0}},
        .initial_flags = {{Flagbit::CF, true}}, // CF viene azzerato da OR
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xFF}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 7 di AL = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0xFF: 8 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "OR EAX, imm32",
        .program = {0x0D, 0x00, 0x00, 0x00, 0x80}, // OR EAX, 0x80000000
        .initial_regs = {{Register::RAX, 0x12345678}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x92345678ULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 31 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0x78: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "OR RAX, imm32 sign extended (REX.W + 0D id)",
        .program = {0x48, 0x0D, 0xF0, 0xFF, 0xFF, 0xFF}, // imm32=-16 -> 0xFFFFFFFFFFFFFFF0
        .initial_regs = {{Register::RAX, 0x000000000000000FULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xFFFFFFFFFFFFFFFFULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0xFF: even parity
        }
    });

    // =====================================================================================
    // OR - immediate to r/m (MI mode: 80 /1, 81 /1, 83 /1)
    // =====================================================================================

    tests.push_back({
        .description = "OR CL, imm8",
        .program = {0x80, 0xc9, 0x0C}, // OR CL, 0x0C
        .initial_regs = {{Register::RCX, 0xF3}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RCX, 0xFF}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 7 di CL = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0xFF: even parity
        }
    });

    tests.push_back({
        .description = "OR EBX, imm32 (81 /1 id)",
        .program = {0x81, 0xCB, 0x0F, 0x00, 0x00, 0x80}, // OR EBX, 0x8000000F
        .initial_regs = {{Register::RBX, 0x12345678}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0x9234567FULL}}, // 32-bit: upper bits zeroed
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 31 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, false} 
        }
    });

    tests.push_back({
        .description = "OR RDI, imm8 sign extended (83 /1 ib, REX.W)",
        .program = {0x48, 0x83, 0xCF, 0x0F}, // REX.W: OR RDI, 0x0F
        .initial_regs = {{Register::RDI, 0xFFFFFFFFFFFFFF00ULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RDI, 0xFFFFFFFFFFFFFF0FULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0xFF: even parity
        }
    });

    tests.push_back({
        .description = "OR qword in memory [RBP+disp8] (83 /1 ib, REX.W)",
        // 48 83 45 10 0F -> OR QWORD PTR [RBP+0x10], imm8(0x0F)
        .program = {0x48, 0x83, 0x45, 0x10, 0x0F},
        .initial_regs = {{Register::RBP, 0x3000}},
        .initial_flags = {},
        .initial_mem = {{0x3010, 0xF0u}, {0x3011, 0x00u}, {0x3012, 0x00u}, {0x3013, 0x00u},
                        {0x3014, 0x00u}, {0x3015, 0x00u}, {0x3016, 0x00u}, {0x3017, 0x00u}},
        .expected_regs = {},
        .expected_mem = {{0x3010, 0xFFu}, {0x3011, 0x00u}, {0x3012, 0x00u}, {0x3013, 0x00u},
                         {0x3014, 0x00u}, {0x3015, 0x00u}, {0x3016, 0x00u}, {0x3017, 0x00u}},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, false}, // bit 63 (byte alto 0x00) = 0
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0xFF: even parity
        }
    });

    // =====================================================================================
    // OR - register/memory forms (MR/RM: 08-0B)
    // =====================================================================================

    tests.push_back({
        .description = "OR RBX, RAX (REX.W + 09 /r, MR)",
        // ModRM: 11 000 011 -> reg=RAX(source), r/m=RBX(dest)
        .program = {0x48, 0x09, 0xC3},
        .initial_regs = {{Register::RAX, 0x00000000FFFFFFFFULL}, {Register::RBX, 0xFFFFFFFF00000000ULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0xFFFFFFFFFFFFFFFFULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0xFF: even parity
        }
    });

    tests.push_back({
        .description = "OR RAX, [RSI] (REX.W + 0B /r, RM: dest=reg, src=mem)",
        // ModRM: 00 000 110 -> mod=00, reg=RAX(dest), r/m=RSI -> [RSI]
        .program = {0x48, 0x0B, 0x06},
        .initial_regs = {{Register::RAX, 0x1234567890ABCDEFULL}, {Register::RSI, 0x4000}},
        .initial_flags = {},
        .initial_mem = {{0x4000, 0x00u}, {0x4001, 0x00u}, {0x4002, 0x00u}, {0x4003, 0x00u},
                        {0x4004, 0x00u}, {0x4005, 0x00u}, {0x4006, 0x00u}, {0x4007, 0xF0u}}, // qword 0xF000000000000000
        .expected_regs = {{Register::RAX, 0xF234567890ABCDEFULL}},
        .expected_mem = {}, // memoria letta sola: invariata
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, false} // byte basso 0xEF: 7 set bits -> odd parity
        }
    });

    tests.push_back({
        .description = "OR RAX, RBX (risultato zero impossibile per OR; ZF=0)",
        // ModRM: 11 000 011 -> reg=RAX(source), r/m=RBX(dest), REX.W
        .program = {0x48, 0x09, 0xD8}, // OR RBX, RAX -> mod=11 reg=3? no: D8 = 11 011 000 -> reg=RBX(source), r/m=RAX(dest)
        .initial_regs = {{Register::RAX, 0x0ULL}, {Register::RBX, 0x0ULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x0ULL}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true}, // 0 | 0 = 0 -> ZF=1 (caso limite: OR di due zero)
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    // =====================================================================================
    // XOR - immediate to accumulator (I mode: 34 / ib, 35 / id)
    // =====================================================================================

    tests.push_back({
        .description = "XOR AL, imm8",
        .program = {0x34, 0xFF}, // XOR AL, 0xFF
        .initial_regs = {{Register::RAX, 0x0F}},
        .initial_flags = {{Flagbit::CF, true}}, // CF viene azzerato da XOR
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xF0}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 7 di AL = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0xF0: 4 set bits -> even parity
        }
    });

    tests.push_back({
        .description = "XOR EAX, imm32 (self-clear pattern)",
        .program = {0x35, 0x78, 0x56, 0x34, 0x12}, // XOR EAX, 0x12345678
        .initial_regs = {{Register::RAX, 0x12345678}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x00}}, // x ^ x = 0 (pattern XOR-zero idiomatico)
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    tests.push_back({
        .description = "XOR RAX, imm32 sign extended (REX.W + 35 id)",
        .program = {0x48, 0x35, 0xFF, 0xFF, 0xFF, 0xFF}, // imm32=-1 -> 0xFFFFFFFFFFFFFFFF
        .initial_regs = {{Register::RAX, 0x00000000FFFFFFFFULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xFFFFFFFF00000000ULL}}, // ~lower half
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0x00: even parity
        }
    });

    // =====================================================================================
    // XOR - immediate to r/m (MI mode: 80 /6, 81 /6, 83 /6)
    // =====================================================================================

    tests.push_back({
        .description = "XOR CL, imm8 (risultato zero -> ZF=1)",
        .program = {0x80, 0xF1, 0xF3}, // XOR CL, 0xF3
        .initial_regs = {{Register::RCX, 0xF3}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RCX, 0x00}}, // x ^ x = 0
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    tests.push_back({
        .description = "XOR EBX, imm32 (81 /6 id)",
        .program = {0x81, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF}, // XOR EBX, -1 -> NOT basso 32 bit
        .initial_regs = {{Register::RBX, 0xFFFFFFFF0000FFFFULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0xFFFF0000ULL}}, // EBX ^= 0xFFFFFFFF (32-bit)
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 31 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0xFF: even parity
        }
    });

    tests.push_back({
        .description = "XOR RDI, imm8 sign extended (83 /6 ib, REX.W)",
        .program = {0x48, 0x83, 0xF7, 0xFF}, // REX.W: XOR RDI, -1 -> toggle lower byte
        .initial_regs = {{Register::RDI, 0x0000000000000000ULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RDI, 0xFFFFFFFFFFFFFFFFULL}}, // imm8 0xFF -> -1 -> 0xFFFF...FF
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // byte basso 0x00: even parity
        }
    });

    tests.push_back({
        .description = "XOR byte in memory [RSI] (80 /6 mod=00)",
        .program = {0x80, 0x36, 0xFF}, // XOR BYTE PTR [RSI], 0xFF (NOT del byte)
        .initial_regs = {{Register::RSI, 0x2000}},
        .initial_flags = {},
        .initial_mem = {{0x2000, 0x0F}},
        .expected_regs = {},
        .expected_mem = {{0x2000, 0xF0}},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 7 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0xF0: 4 set bits -> even parity
        }
    });

    // =====================================================================================
    // XOR - register/memory forms (MR/RM: 30-33)
    // =====================================================================================

    tests.push_back({
        .description = "XOR RBX, RAX (REX.W + 31 /r, MR)",
        // ModRM: 11 000 011 -> reg=RAX(source), r/m=RBX(dest)
        .program = {0x48, 0x31, 0xC3},
        .initial_regs = {{Register::RAX, 0xFFFFFFFFFFFFFFFFULL}, {Register::RBX, 0x1234567890ABCDEFULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0xEDCBA9876F543210ULL}}, // RBX = ~RBX
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 63 del risultato = 1
            {Flagbit::OF, false},
            {Flagbit::PF, false} // byte basso 0x00: even parity
        }
    });

    tests.push_back({
        .description = "XOR RAX, RAX (self-clearing idiom -> ZF=1)",
        // ModRM: 11 000 000 -> reg=RAX(source), r/m=RAX(dest)
        .program = {0x48, 0x33, 0xC0}, // XOR RAX, RAX (33 /r RM: dest=reg, src=r/m)
        .initial_regs = {{Register::RAX, 0xDEADBEEFCAFEBABEULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0x00}},
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    tests.push_back({
        .description = "XOR RAX, [RSI] (REX.W + 33 /r, RM: dest=reg, src=mem)",
        // ModRM: 00 000 000 -> mod=00, reg=RAX(dest), r/m=RSI -> [RSI]
        .program = {0x48, 0x33, 0x06},
        .initial_regs = {{Register::RAX, 0xFFFFFFFFFFFFFFFFULL}, {Register::RSI, 0x4000}},
        .initial_flags = {},
        .initial_mem = {{0x4000, 0xFFu}, {0x4001, 0xFFu}, {0x4002, 0xFFu}, {0x4003, 0xFFu},
                        {0x4004, 0xFFu}, {0x4005, 0xFFu}, {0x4006, 0xFFu}, {0x4007, 0xFFu}}, // qword all 1
        .expected_regs = {{Register::RAX, 0x00}},
        .expected_mem = {}, // memoria letta sola: invariata
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0x00: even parity
        }
    });

    tests.push_back({
        .description = "XOR AL, BL (30 /r, 8-bit)",
        // ModRM: 11 000 011 -> reg=RAX(source), r/m=RBX(dest)
        .program = {0x30, 0xC3},
        .initial_regs = {{Register::RAX, 0xAA}, {Register::RBX, 0x55}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RBX, 0xFF}}, // 0xAA ^ 0x55 = 0xFF
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}, // bit 7 di AL = 1
            {Flagbit::OF, false},
            {Flagbit::PF, true} // 0xFF: 8 set bits -> even parity
        }
    });

    // =====================================================================================
    // NOT - single operand r/m (F6 /2 byte, F7 /2 v; M mode)
    // NOT NON modifica i flag di stato: con i flag inizializzati a un valore
    // arbitrario i flag devono restare invariati.
    // =====================================================================================

    tests.push_back({
        .description = "NOT AL (F6 /2, 8-bit) - flags preserved",
        .program = {0xF6, 0xD0}, // F6 /2: NOT r/m8 -> mod=11, reg=2(NOT), r/m=RAX
        .initial_regs = {{Register::RAX, 0x0F}},
        .initial_flags = {{Flagbit::CF, true}, {Flagbit::ZF, true}, {Flagbit::SF, true},
                          {Flagbit::OF, true}, {Flagbit::PF, true}},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xF0}}, // ~0x0F (8-bit)
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true}, // invariati
            {Flagbit::ZF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, true},
            {Flagbit::PF, true}
        }
    });

    tests.push_back({
        .description = "NOT CL (F6 /2, 8-bit) - risultato FF",
        .program = {0xF6, 0xD1}, // mod=11, reg=2(NOT), r/m=RCX
        .initial_regs = {{Register::RCX, 0x00}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RCX, 0xFF}}, // ~0x00 (8-bit) -> CL=0xFF, RCX zero-extended upper bits unchanged? test lower: value raw masked compare expected via reg read
        .expected_mem = {},
        .expected_flags = {}
    });

    tests.push_back({
        .description = "NOT AX (66 F7 /2, 16-bit operand-size prefix)",
        .program = {0x66, 0xF7, 0xD0}, // 66 F7 /2: NOT r/m16 -> mod=11 reg=2 r/m=RAX
        .initial_regs = {{Register::RAX, 0x1234}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xEDCB}}, // AX = ~0x1234 (16-bit); upper bits invariati
        .expected_mem = {},
        .expected_flags = {}
    });

    tests.push_back({
        .description = "NOT EAX (F7 /2 default 32-bit)",
        .program = {0xF7, 0xD0}, // F7 /2: NOT r/m16/32/64 -> default operand 32-bit
        .initial_regs = {{Register::RAX, 0x12345678}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xEDCBA987ULL}}, // EAX = ~0x12345678 (32-bit), upper zeroed
        .expected_mem = {},
        .expected_flags = {}
    });

    tests.push_back({
        .description = "NOT RAX (REX.W + F7 /2, 64-bit)",
        .program = {0x48, 0xF7, 0xD0}, // REX.W: NOT RAX
        .initial_regs = {{Register::RAX, 0x1234567890ABCDEFULL}},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {{Register::RAX, 0xEDCBA9876f543210ULL}}, // ~0x1234567890ABCDEF
        .expected_mem = {},
        .expected_flags = {}
    });

    tests.push_back({
        .description = "NOT byte in memory [RSI] (F6 /2 mod=00)",
        .program = {0xF6, 0x16}, // F6 /2: NOT BYTE PTR [RSI] -> mod=00 reg=2 r/m=RSI
        .initial_regs = {{Register::RSI, 0x2000}},
        .initial_flags = {},
        .initial_mem = {{0x2000, 0x0F}},
        .expected_regs = {},
        .expected_mem = {{0x2000, 0xF0}}, // ~0x0F (8-bit)
        .expected_flags = {}
    });

    tests.push_back({
        .description = "NOT qword in memory [RBP+disp8] (REX.W + F7 /2)",
        // 48 F7 55 10 -> NOT QWORD PTR [RBP+0x10]
        .program = {0x48, 0xF7, 0x55, 0x10},
        .initial_regs = {{Register::RBP, 0x3000}},
        .initial_flags = {},
        .initial_mem = {{0x3010, 0xEFu}, {0x3011, 0xCDu}, {0x3012, 0xABu}, {0x3013, 0x90u},
                        {0x3014, 0x78u}, {0x3015, 0x56u}, {0x3016, 0x34u}, {0x3017, 0x12u}}, // qword 0x1234567890ABCDEF
        .expected_regs = {},
        .expected_mem = {{0x3010, 0x10u}, {0x3011, 0x32u}, {0x3012, 0x54u}, {0x3013, 0x6fu},
                         {0x3014, 0x87u}, {0x3015, 0xA9u}, {0x3016, 0xCBu}, {0x3017, 0xEDu}}, // ~qword = 0xEDCBA9876f543210
        .expected_flags = {}
    });

    return tests;
}
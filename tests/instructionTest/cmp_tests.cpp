// -----------------------------------------------------------------------------
// CMP: flags-only compare
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildCmpTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // CMP - confronta due operandi (dest - src) e aggiorna SOLO i flag:
    // registri e memoria non devono mai essere modificati
    // =========================================================================

    // -------------------------------------------------------------------------
    // CMP AL, imm8 (operandi uguali -> ZF = 1)
    //
    // 3C 20
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP AL, imm8 (equal -> ZF=1, AL unchanged)",
        .program = {
            0x3C, 0x20 // 3C ib -> CMP AL, 0x20
        },
        .initial_regs = {
            {Register::RAX, 0x20}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x20} // AL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // -------------------------------------------------------------------------
    // CMP AL, imm8 (dest < src -> CF = 1, SF = 1)
    //
    // 3C 20
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP AL, imm8 (dest < src -> CF=1, SF=1, AL unchanged)",
        .program = {
            0x3C, 0x20 // 3C ib -> CMP AL, 0x20
        },
        .initial_regs = {
            {Register::RAX, 0x10}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x10} // AL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (64-bit, REX.W + 39 /r)
    //
    // ModRM: 11 011 000 = D8
    //   MOD = 11 -> register
    //   REG = 011 -> RBX (source)
    //   R/M = 000 -> RAX (destination)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (64-bit, both operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x100},
            {Register::RBX, 0x50}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x100}, // RAX invariato
            {Register::RBX, 0x50}   // RBX invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (dest < src -> CF = 1, OF = 0)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (dest < src -> CF=1, SF=1, operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x50},
            {Register::RBX, 0x100}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x50},
            {Register::RBX, 0x100}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP DWORD PTR [RSI], imm8 (Gruppo 1, /7)
    //
    // 83 /7 ib
    //
    // ModRM: 00 111 110 = 3E
    //   MOD = 00 -> memory
    //   REG = 111 -> /7 (CMP)
    //   R/M = 110 -> RSI
    //
    // 83 3E 05
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP DWORD [RSI], imm8 (equal -> ZF=1, memory unchanged)",
        .program = {
            0x83, 0x3E, 0x05 // 83 /7 ib -> CMP DWORD PTR [RSI], 5
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x05},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x2000} // RSI invariato
        },
        .expected_mem = {
            // la memoria NON viene sovrascritta
            {0x2000, 0x05},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP DWORD PTR [RSI], imm8 (dest < src -> CF = 1)
    //
    // 83 3E 0A
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP DWORD [RSI], imm8 (dest < src -> CF=1, memory unchanged)",
        .program = {
            0x83, 0x3E, 0x0A // 83 /7 ib -> CMP DWORD PTR [RSI], 10
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x03},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, 0x03},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP BYTE PTR [RSI], imm8 (8-bit, Gruppo 1 /7)
    //
    // 80 /7 ib
    //
    // ModRM: 00 111 110 = 3E
    //
    // 80 3E 02
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP BYTE [RSI], imm8 (dest > src -> CF=0, ZF=0, memory unchanged)",
        .program = {
            0x80, 0x3E, 0x02 // 80 /7 ib -> CMP BYTE PTR [RSI], 2
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, static_cast<uint8_t>(0x07)}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, static_cast<uint8_t>(0x07)} // invariato
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP R8, r/m8 (RM mode, 3A /r)
    //
    // ModRM: 11 000 001 = C1
    //   MOD = 11 -> register
    //   REG = 000 -> RAX (destination)
    //   R/M = 001 -> RCX (source)
    //
    // 3A C1 -> CMP AL, CL
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP AL, CL (RM 8-bit, dest < src -> CF=1, operands unchanged)",
        .program = {
            0x3A, 0xC1 // 3A /r -> CMP AL, CL
        },
        .initial_regs = {
            {Register::RAX, 0x05},
            {Register::RCX, 0x09}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x05},
            {Register::RCX, 0x09}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RDX, QWORD PTR [RSI] (RM mode, memoria come sorgente, 64-bit)
    //
    // REX.W = 48
    // Opcode = 3B
    // ModRM: 00 010 110 = 26
    //   MOD = 00 -> memory
    //   REG = 010 -> RDX (destination)
    //   R/M = 110 -> RSI (source)
    //
    // 48 3B 26
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RDX, QWORD [RSI] (mem source, dest > src -> CF=0, memory unchanged)",
        .program = {
            0x48, // REX.W
            0x3B, 0x26 // 3B /r -> CMP RDX, QWORD PTR [RSI]
        },
        .initial_regs = {
            {Register::RDX, 0x20},
            {Register::RSI, 0x4000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x4000, static_cast<uint8_t>(0x10)}, // value = 0x10
            {0x4001, 0x00},
            {0x4002, 0x00},
            {0x4003, 0x00},
            {0x4004, 0x00},
            {0x4005, 0x00},
            {0x4006, 0x00},
            {0x4007, 0x00}
        },
        .expected_regs = {
            {Register::RDX, 0x20}, // RDX invariato
            {Register::RSI, 0x4000}
        },
        .expected_mem = {
            {0x4000, static_cast<uint8_t>(0x10)}, // memoria invariata
            {0x4001, 0x00},
            {0x4002, 0x00},
            {0x4003, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP QWORD PTR [RSI + RDI*8], RDX (MR mode, SIB senza displacement, 64-bit)
    //
    // REX.W = 48
    // Opcode = 39
    //
    // ModRM: 00 010 100 = 24
    //   MOD = 00 -> memory
    //   REG = 010 -> RDX (source)
    //   R/M = 100 -> SIB follows
    //
    // SIB:
    //   scale = 11 -> x8
    //   index = RDI = 111
    //   base  = RSI = 110
    //   11 111 110 = FE
    //
    // Address = 0x3000 + 0x1 * 8 = 0x3008
    //
    // 48 39 24 FE
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP QWORD [RSI + RDI*8], RDX (SIB, dest < src -> CF=1, memory unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0x24, 0xFE // 39 /r + SIB -> CMP QWORD PTR [RSI + RDI*8], RDX
        },
        .initial_regs = {
            {Register::RSI, 0x3000},
            {Register::RDI, 0x1},
            {Register::RDX, 0x20}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x3008, static_cast<uint8_t>(0x10)}, // value = 0x10 (little endian)
            {0x3009, 0x00},
            {0x300A, 0x00},
            {0x300B, 0x00},
            {0x300C, 0x00},
            {0x300D, 0x00},
            {0x300E, 0x00},
            {0x300F, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x3000}, // RSI invariato
            {Register::RDI, 0x1},    // RDI invariato
            {Register::RDX, 0x20}    // RDX invariato
        },
        .expected_mem = {
            // [0x3008] NON viene sovrascritto
            {0x3008, static_cast<uint8_t>(0x10)},
            {0x3009, 0x00},
            {0x300A, 0x00},
            {0x300B, 0x00},
            {0x300C, 0x00},
            {0x300D, 0x00},
            {0x300E, 0x00},
            {0x300F, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // -------------------------------------------------------------------------
    // CMP EAX, imm32 (3D id)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP EAX, imm32 (equal -> ZF=1, EAX unchanged)",
        .program = {
            0x3D,
            0x00, 0x01, 0x00, 0x00 // imm32 = 0x100
        },
        .initial_regs = {
            {Register::RAX, 0x100}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x100}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, imm32 (REX.W + 3D id, operanda a 64 bit)
    //
    // NOTA: il decoder del simulatore carica l'immediato zero-extended
    // (info.bit_extension non viene applicato), quindi l'immediato e' 0x20.
    // 0x10 - 0x20 = -0x10 -> borrow (CF=1), risultato 0xFFFFFFFFFFFFFFF0 (SF=1)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, imm32 (REX.W 64-bit, dest < src -> CF=1, SF=1, RAX unchanged)",
        .program = {
            0x48, // REX.W
            0x3D,
            0x20, 0x00, 0x00, 0x00 // imm32 = 0x20
        },
        .initial_regs = {
            {Register::RAX, 0x10}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x10}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (dest positivo, src negativo)
    //
    // RAX = 0x0000000000000001, RBX = 0xFFFFFFFFFFFFFFFF (-1)
    // unsigned: 1 < 0xFFFFFFFFFFFFFFFF -> CF = 1 (borrow)
    // signed:   1 - (-1) = 2 -> positivo, nessun overflow -> OF = 0, SF = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (positive - negative -> CF=1, OF=0, operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x1},
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1},
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (dest negativo, src positivo)
    //
    // RAX = 0xFFFFFFFFFFFFFFFF (-1), RBX = 0x1
    // signed:   -1 - 1 = -2 -> 0xFFFFFFFFFFFFFFFE -> SF = 1, nessun overflow -> OF = 0
    // unsigned: 0xFFFFFFFFFFFFFFFF >= 1 -> nessun borrow -> CF = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (negative - positive -> SF=1, OF=0, CF=0, operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL},
            {Register::RBX, 0x1}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL},
            {Register::RBX, 0x1}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });


    return tests;
}

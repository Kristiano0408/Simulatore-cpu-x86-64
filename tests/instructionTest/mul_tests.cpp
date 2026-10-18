// -----------------------------------------------------------------------------
// MUL / IMUL: multiplication group (1, 2 and 3 operands)
//
// MUL  1-op: F6 /4 (r/m8), F7 /4 (r/m16/32/64)  -> M_ACC
// IMUL 1-op: F6 /5 (r/m8), F7 /5 (r/m16/32/64)  -> M_ACC
// IMUL 2-op: 0F AF /r                           -> RM
// IMUL 3-op: 6B /r ib, 69 /r id                 -> RMI
//
// NOTE: the simulator does not model AH/AL separately. For the 1-operand form
// the low half (resultValue) is written to RAX and the high half
// (resultHighValue) is written to RDX for every width (including 8-bit),
// instead of the real AH:AL / DX:AX split.
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildMulTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // MUL - 1 operand, implicit accumulator (M_ACC)
    //   F6 /4 = MUL r/m8,  F7 /4 = MUL r/m16/32/64
    // =========================================================================

    // -------------------------------------------------------------------------
    // MUL BL (8-bit, no overflow)
    //
    // F6 E3 -> F6 /4, mod=11, reg=4(MUL), r/m=011(RBX/BL)
    // RAX = 10, RBX = 5  ->  product = 50
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "MUL BL (8-bit, no overflow) -> RAX=50, RDX=0, CF=0, OF=0",
        .program = {
            0xF6, 0xE3 // F6 E3 -> MUL r/m8 (BL)
        },
        .initial_regs = {
            {Register::RAX, 10},
            {Register::RBX, 5}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 50},
            {Register::RBX, 5},   // source unchanged
            {Register::RDX, 0}    // high half = 0
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // MUL BL (8-bit, with overflow)
    //
    // F6 E3 -> MUL r/m8 (BL)
    // RAX = 200, RBX = 10 -> product = 2000 = 0x07D0
    // low  = 0xD0, high = 0x07
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "MUL BL (8-bit, overflow) -> RAX=0xD0, RDX=0x07, CF=1, OF=1",
        .program = {
            0xF6, 0xE3 // F6 E3 -> MUL r/m8 (BL)
        },
        .initial_regs = {
            {Register::RAX, 200},
            {Register::RBX, 10}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xD0},
            {Register::RBX, 10},
            {Register::RDX, 0x07}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}
        }
    });

    // -------------------------------------------------------------------------
    // MUL RBX (64-bit, multi-precision)
    //
    // REX.W + F7 C3 -> F7 /4, mod=11, reg=4(MUL), r/m=000(RAX)
    // RAX = 0x100000000, RBX = 0x100000000
    // product = 0x1_00000000_00000000 -> RAX = 0, RDX = 1
    //
    // ModRM C3: mod=11, reg=100(/4 MUL), r/m=011(RBX)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "MUL RBX (64-bit) -> RAX=0, RDX=1, CF=1, OF=1",
        .program = {
            0x48,       // REX.W
            0xF7, 0xE3  // F7 /4, mod=11, reg=100(/4 MUL), r/m=011(RBX) -> MUL RBX
        },
        .initial_regs = {
            {Register::RAX, 0x100000000ULL},
            {Register::RBX, 0x100000000ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0},
            {Register::RBX, 0x100000000ULL},
            {Register::RDX, 1}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}
        }
    });

    // -------------------------------------------------------------------------
    // MUL qword ptr [RSI] (memory source, M_ACC)
    //
    // REX.W + F7 26 -> F7 /4, mod=00, r/m=110(RSI) -> MUL qword [RSI]
    // RAX = 6, [0x200] = 7 -> product = 42 -> RAX = 42, RDX = 0
    // Memory must NOT be written (source only).
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "MUL qword [RSI] (memory source) -> RAX=42, RDX=0, memory unchanged",
        .program = {
            0x48,       // REX.W
            0xF7, 0x26  // F7 /4, mod=00, r/m=110(RSI) -> MUL [RSI]
        },
        .initial_regs = {
            {Register::RAX, 6},
            {Register::RSI, 0x200}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x200, 7} // single byte value 7 at the address (little-endian low byte)
        },
        .expected_regs = {
            {Register::RAX, 42},
            {Register::RDX, 0}
        },
        .expected_mem = {
            {0x200, 7} // memory unchanged
        },
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // =========================================================================
    // IMUL - 1 operand, implicit accumulator (M_ACC)
    //   F6 /5 = IMUL r/m8,  F7 /5 = IMUL r/m16/32/64
    // =========================================================================

    // -------------------------------------------------------------------------
    // IMUL BL (signed 1-op, negative result, no overflow)
    //
    // F6 EB -> F6 /5, mod=11, reg=5(IMUL), r/m=011(RBX/BL)
    // AL = -5 (0xFB), BL = 4 -> product = -20 = 0xFFEC
    // low byte = 0xEC, high byte = 0xFF
    // sign-extend(low byte) = -20 == full product -> CF=0, OF=0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL BL (signed 1-op, -5*4) -> RAX=0xEC, RDX=0xFF, CF=0, OF=0",
        .program = {
            0xF6, 0xEB // F6 /5, mod=11, r/m=011(RBX) -> IMUL r/m8 (BL)
        },
        .initial_regs = {
            {Register::RAX, 0xFB}, // AL = -5
            {Register::RBX, 4}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xEC},  // low byte of -20
            {Register::RBX, 4},
            {Register::RDX, 0xFF}   // high byte of -20
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // IMUL RBX (signed 1-op, 64-bit, overflow)
    //
    // REX.W + F7 CB -> F7 /5, mod=11, reg=5(IMUL), r/m=011(RBX)
    // RAX = 0x7FFFFFFFFFFFFFFF, RBX = 2
    // product = 0xFFFFFFFFFFFFFFFE -> does not fit signed 64-bit
    // low = 0xFFFFFFFFFFFFFFFE, high = 0x01 -> CF=1, OF=1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL RBX (signed 1-op 64-bit overflow) -> CF=1, OF=1",
        .program = {
            0x48,       // REX.W
            0xF7, 0xEB  // F7 /5, mod=11, reg=101(/5 IMUL), r/m=011(RBX) -> IMUL RBX
        },
        .initial_regs = {
            {Register::RAX, 0x7FFFFFFFFFFFFFFFULL},
            {Register::RBX, 2}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFEULL},
            {Register::RBX, 2},
            {Register::RDX, 0}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}
        }
    });

    // =========================================================================
    // IMUL - 2 operands: IMUL reg, r/m (RM)
    //   0F AF /r
    // =========================================================================

    // -------------------------------------------------------------------------
    // IMUL EAX, EBX (2-op, no overflow)
    //
    // 0F AF C3 -> mod=11, reg=000(EAX dest), r/m=011(EBX src)
    // EAX = 100, EBX = -2 (0xFFFFFFFE) -> EAX = -200 = 0xFFFFFF38
    // product fits 32-bit signed -> CF=0, OF=0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL EAX, EBX (2-op, 100*-2) -> EAX=0xFFFFFF38, CF=0, OF=0",
        .program = {
            0x0F, 0xAF, 0xC3 // 0F AF /r -> IMUL EAX, EBX
        },
        .initial_regs = {
            {Register::RAX, 100},
            {Register::RBX, 0xFFFFFFFFFFFFFFFEULL} // EBX = -2
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFF38ULL}, // -200 truncated to 32-bit (zero-extended store)
            {Register::RBX, 0xFFFFFFFFFFFFFFFEULL} // source unchanged
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // IMUL RAX, RBX (2-op, 64-bit, no overflow)
    //
    // REX.W + 0F AF C3 -> IMUL RAX, RBX
    // RAX = 100, RBX = -2 -> RAX = -200 = 0xFFFFFFFFFFFFFF38
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL RAX, RBX (2-op 64-bit, 100*-2) -> RAX=0xFFFFFFFFFFFFFF38, CF=0, OF=0",
        .program = {
            0x48,             // REX.W
            0x0F, 0xAF, 0xC3  // 0F AF /r -> IMUL RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 100},
            {Register::RBX, 0xFFFFFFFFFFFFFFFEULL} // RBX = -2
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFF38ULL}, // -200
            {Register::RBX, 0xFFFFFFFFFFFFFFFEULL}  // source unchanged
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // IMUL RAX, RBX (2-op, 64-bit, overflow)
    //
    // REX.W + 0F AF C3 -> IMUL RAX, RBX
    // RAX = 0x7FFFFFFFFFFFFFFF, RBX = 4 -> product does not fit signed 64-bit
    // low = 0xFFFFFFFFFFFFFFFC -> CF=1, OF=1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL RAX, RBX (2-op 64-bit overflow) -> CF=1, OF=1",
        .program = {
            0x48,             // REX.W
            0x0F, 0xAF, 0xC3  // 0F AF /r -> IMUL RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x7FFFFFFFFFFFFFFFULL},
            {Register::RBX, 4}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFCULL},
            {Register::RBX, 4}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}
        }
    });

    // =========================================================================
    // IMUL - 3 operands: IMUL reg, r/m, imm (RMI)
    //   6B /r ib (imm8, sign-extended), 69 /r id (imm16/32)
    // =========================================================================

    // -------------------------------------------------------------------------
    // IMUL RAX, RBX, 10 (3-op, imm8)
    //
    // REX.W + 6B C3 0A -> mod=11, reg=000(RAX dest), r/m=011(RBX src), imm8=10
    // RBX = 50 -> RAX = 500, RBX unchanged
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL RAX, RBX, 10 (3-op imm8) -> RAX=500, RBX unchanged, CF=0, OF=0",
        .program = {
            0x48,             // REX.W
            0x6B, 0xC3, 0x0A  // 6B /r ib -> IMUL RAX, RBX, 10
        },
        .initial_regs = {
            {Register::RAX, 0},
            {Register::RBX, 50}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 500},
            {Register::RBX, 50} // source unchanged
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // IMUL EAX, EBX, 1000 (3-op, imm32)
    //
    // 69 C3 E8 03 00 00 -> mod=11, reg=000(EAX dest), r/m=011(EBX src), imm32=1000
    // EBX = 30 -> EAX = 30000
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL EAX, EBX, 1000 (3-op imm32) -> EAX=30000, CF=0, OF=0",
        .program = {
            0x69, 0xC3, 0xE8, 0x03, 0x00, 0x00 // 69 /r id -> IMUL EAX, EBX, 1000
        },
        .initial_regs = {
            {Register::RAX, 0},
            {Register::RBX, 30}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 30000},
            {Register::RBX, 30}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // IMUL EAX, dword [RSI], 1000 (3-op with memory source)
    //
    // 69 26 E8 03 00 00 -> mod=00, reg=000(EAX dest), r/m=110(RSI), imm32=1000
    // [0x200] = 30 -> EAX = 30000, memory unchanged
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IMUL EAX, [RSI], 1000 (3-op memory) -> EAX=30000, memory unchanged",
        .program = {
            0x69, 0x06, 0xE8, 0x03, 0x00, 0x00 // 69 /r id -> IMUL EAX, [RSI], 1000
        },
        .initial_regs = {
            {Register::RAX, 0},
            {Register::RSI, 0x200}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x200, 30} // low byte 30 at the address
        },
        .expected_regs = {
            {Register::RAX, 30000}
        },
        .expected_mem = {
            {0x200, 30} // memory unchanged
        },
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false}
        }
    });

    return tests;
}

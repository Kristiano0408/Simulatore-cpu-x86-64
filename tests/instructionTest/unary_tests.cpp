// -----------------------------------------------------------------------------
// INC / DEC / NEG: single operand group instructions
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildUnaryTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // INC - Single operand r/m instructions (Gruppo 4 e Gruppo 5)
    //   FE /0 = INC r/m8,  FE /1 = DEC r/m8
    //   FF /0 = INC r/m16/32/64,  FF /1 = DEC r/m16/32/64
    // =========================================================================

    tests.push_back({
        .description = "INC AL (register)",
        .program = {
            0xFE, 0xC0 // FE C0 -> INC r/m8, mod=11, reg=0(INC), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC RCX (register)",
        .program = {
            0xFF, 0xC1 // FF C1 -> INC r/m32/64, mod=11, reg=0(INC), r/m=RCX(1)
        },
        .initial_regs = {
            {Register::RCX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RCX, 0x0B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC AX (operand size override)",
        .program = {
            0x66, 0xFF, 0xC0 // 66 FF C0 -> INC r/m16 with operand-size prefix
        },
        .initial_regs = {
            {Register::RAX, 0x1234}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1235} // only lower 16 bits change: AX goes from 0x1234 to 0x1235
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC RAX (64-bit with REX.W)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xFF, 0xC0 // FF C0 -> INC r/m64, mod=11, reg=0(INC), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "DEC RAX (64-bit with REX.W)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xFF, 0xC8 // FF C8 -> DEC r/m64, mod=11, reg=1(DEC), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0B}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0A}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "DEC RCX (register)",
        .program = {
            0xFF, 0xC9 // FF C9 -> DEC r/m32/64, mod=11, reg=1(DEC), r/m=RCX(1)
        },
        .initial_regs = {
            {Register::RCX, 0x0B}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RCX, 0x0A}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC QWORD PTR [RSI] (memory)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xFF, 0x06 // FF 06 -> INC r/m64 at memory address in RSI
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, static_cast<uint8_t>(0xFF)}, // value = 0x00...00FF at [RSI] -> after INC: 0x00...0100
            {0x1001, 0x00},
            {0x1002, 0x00},
            {0x1003, 0x00},
            {0x1004, 0x00},
            {0x1005, 0x00},
            {0x1006, 0x00},
            {0x1007, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x1000} // RSI unchanged
        },
        .expected_mem = {
            {0x1000, static_cast<uint8_t>(0x00)},
            {0x1001, static_cast<uint8_t>(0x01)}, // 0xFF + 1 = 0x100 -> byte 1 = 0x01
            {0x1002, 0x00},
            {0x1003, 0x00},
            {0x1004, 0x00},
            {0x1005, 0x00},
            {0x1006, 0x00},
            {0x1007, 0x00}
        }
    });

    tests.push_back({
        .description = "DEC DWORD PTR [RDI] (memory)",
        .program = {
            0xFF, 0x0F // FF OF -> DEC r/m32 at memory address in RDI (no operand-size prefix needed for default)
        },
        .initial_regs = {
            {Register::RDI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, static_cast<uint8_t>(0x05)}, // value = 0x05 at [RDI] -> after DEC: 0x04
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_regs = {
            {Register::RDI, 0x2000} // RDI unchanged
        },
        .expected_mem = {
            {0x2000, static_cast<uint8_t>(0x04)},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        }
    });

    // =========================================================================
    // NEG - Two's complement negate (Gruppo 3, reg = 011b)
    //   F6 /3 = NEG r/m8
    //   F7 /3 = NEG r/m16/32/64
    // =========================================================================

    // -------------------------------------------------------------------------
    // NEG AL
    //
    // F6 D8
    //
    // ModRM:
    //   11 011 000 = D8
    //   MOD = 11 -> register-direct
    //   REG = 011 -> /3 (NEG)
    //   R/M = 000 -> RAX (AL)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG AL (positive value)",
        .program = {
            0xF6, 0xD8 // F6 D8 -> NEG r/m8, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x05}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFB} // -5 in two's complement (8-bit), upper bits untouched
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // dest != 0 -> borrow from a higher bit
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    tests.push_back({
        .description = "NEG AL (zero)",
        .program = {
            0xF6, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0x00}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // 0 - 0 needs no borrow
            {Flagbit::OF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false}
        }
    });

    tests.push_back({
        .description = "NEG AL (most negative value -> overflow)",
        .program = {
            0xF6, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0x80} // -128
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x80} // -(-128) = 128 is not representable in 8 bits
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}, // overflow: operand is -2^(N-1)
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG AX (16-bit, operand-size prefix 0x66)
    //
    // 66 F7 D8
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG AX (operand size override)",
        .program = {
            0x66, // operand-size prefix -> 16-bit operand
            0xF7, 0xD8 // F7 D8 -> NEG r/m16, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0001}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFF} // only lower 16 bits change
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG EAX (32-bit default, no REX.W and no 0x66)
    //
    // F7 D8
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG EAX (32-bit default)",
        .program = {
            0xF7, 0xD8 // F7 D8 -> NEG r/m32, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0000002A} // 42
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFD6ULL} // -42 (32-bit write clears the upper half)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG RAX (64-bit, REX.W = 0x48)
    //
    // 48 F7 D8
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG RAX (64-bit with REX.W)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xF7, 0xD8 // F7 D8 -> NEG r/m64, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0000000000000001ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL} // -1
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    tests.push_back({
        .description = "NEG RAX (64-bit overflow, 0x8000000000000000)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xF7, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0x8000000000000000ULL} // -2^63
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x8000000000000000ULL} // unchanged: 2^63 is not representable
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}, // overflow: operand is -2^(N-1)
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG BYTE PTR [RSI] (8-bit memory operand)
    //
    // F6 1E
    //
    // ModRM:
    //   00 011 110 = 1E
    //   MOD = 00 -> memory
    //   REG = 011 -> /3 (NEG)
    //   R/M = 110 -> RSI
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG BYTE PTR [RSI] (memory)",
        .program = {
            0xF6, 0x1E // F6 1E -> NEG r/m8 at memory address in RSI
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, static_cast<uint8_t>(0x0A)} // 10 -> after NEG: 0xF6 (-10)
        },
        .expected_regs = {
            {Register::RSI, 0x2000} // RSI unchanged
        },
        .expected_mem = {
            {0x2000, static_cast<uint8_t>(0xF6)}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG QWORD PTR [RSI + RDI*8 + 0x10] (64-bit memory operand with SIB + disp8)
    //
    // 48 F7 5C E6 10
    //
    // REX.W = 48
    // Opcode = F7
    //
    // ModRM:
    //   MOD = 01 -> disp8
    //   REG = 011 -> /3 (NEG)
    //   R/M = 100 -> SIB follows
    //
    // 01 011 100 = 5C
    //
    // SIB:
    //   scale = 11 -> x8
    //   index = RDI = 111
    //   base  = RSI = 110
    //
    // 11 111 110 = FE
    //
    // disp8 = 10
    //
    // Address = 0x3000 + 0x2 * 8 + 0x10 = 0x3020
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG QWORD PTR [RSI + RDI*8 + 0x10] (memory, SIB + disp8)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xF7, 0x5C, 0xFE, 0x10 // F7 5C FE 10 -> NEG r/m64 at [RSI + RDI*8 + 0x10]
        },
        .initial_regs = {
            {Register::RSI, 0x3000},
            {Register::RDI, 0x2}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x3020, static_cast<uint8_t>(0x00)}, // value = 0x100 (little endian)
            {0x3021, static_cast<uint8_t>(0x01)},
            {0x3022, 0x00},
            {0x3023, 0x00},
            {0x3024, 0x00},
            {0x3025, 0x00},
            {0x3026, 0x00},
            {0x3027, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x3000}, // RSI unchanged
            {Register::RDI, 0x2}     // RDI unchanged
        },
        .expected_mem = {
            // -0x100 = 0xFFFFFFFFFFFFFF00 (little endian)
            {0x3020, static_cast<uint8_t>(0x00)},
            {0x3021, static_cast<uint8_t>(0xFF)},
            {0x3022, static_cast<uint8_t>(0xFF)},
            {0x3023, static_cast<uint8_t>(0xFF)},
            {0x3024, static_cast<uint8_t>(0xFF)},
            {0x3025, static_cast<uint8_t>(0xFF)},
            {0x3026, static_cast<uint8_t>(0xFF)},
            {0x3027, static_cast<uint8_t>(0xFF)}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });


    return tests;
}

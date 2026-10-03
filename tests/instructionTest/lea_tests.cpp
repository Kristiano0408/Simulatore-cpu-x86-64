// -----------------------------------------------------------------------------
// LEA: load effective address (opcode 0x8D)
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildLeaTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // LEA - Load Effective Address (opcode 0x8D)
    // =========================================================================
    //
    // 64-bit LEA uses:
    //
    // REX.W + 8D /r
    //
    // ModRM:
    //   mod = addressing mode
    //   reg = destination register
    //   r/m = base register, or 100 to indicate a SIB byte
    //
    // SIB:
    //   scale | index | base
    //
    // In 64-bit mode:
    //   MOD = 00 -> memory, normally no displacement
    //   MOD = 01 -> disp8
    //   MOD = 10 -> disp32
    //   MOD = 11 -> register-direct encoding
    //
    // SIB is present when ModRM.r/m = 100.

    // -------------------------------------------------------------------------
    // LEA RAX, [RBX]
    //
    // REX.W = 48
    // Opcode = 8D
    //
    // ModRM:
    //   MOD = 00
    //   REG = RAX = 000
    //   R/M = RBX = 011
    //
    // 00 000 011 = 03
    //
    // Final encoding:
    // 48 8D 03
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [RBX]",
        .program = {
            0x48, 0x8D, 0x03
        },
        .initial_regs = {
            {Register::RBX, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1000},
            {Register::RBX, 0x1000}
        },
        .expected_mem = {}
    });

    // -------------------------------------------------------------------------
    // LEA RAX, [RBX + 0x20]
    //
    // REX.W = 48
    // Opcode = 8D
    //
    // ModRM:
    //   MOD = 01 -> disp8
    //   REG = RAX = 000
    //   R/M = RBX = 011
    //
    // 01 000 011 = 43
    //
    // disp8 = 20
    //
    // Final encoding:
    // 48 8D 43 20
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [RBX + 0x20]",
        .program = {
            0x48, 0x8D, 0x43, 0x20
        },
        .initial_regs = {
            {Register::RBX, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1020},
            {Register::RBX, 0x1000}
        },
        .expected_mem = {}
    });

    // -------------------------------------------------------------------------
    // LEA RAX, [RBX + RCX]
    //
    // REX.W = 48
    // Opcode = 8D
    //
    // ModRM:
    //   MOD = 00
    //   REG = RAX = 000
    //   R/M = 100 -> SIB follows
    //
    // 00 000 100 = 04
    //
    // SIB:
    //   scale = 00 -> x1
    //   index = RCX = 001
    //   base  = RBX = 011
    //
    // 00 001 011 = 0B
    //
    // Final encoding:
    // 48 8D 04 0B
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [RBX + RCX]",
        .program = {
            0x48, 0x8D, 0x04, 0x0B
        },
        .initial_regs = {
            {Register::RBX, 0x2000},
            {Register::RCX, 0x0100}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x2100},
            {Register::RBX, 0x2000},
            {Register::RCX, 0x0100}
        },
        .expected_mem = {}
    });

    // -------------------------------------------------------------------------
    // LEA RAX, [RBX + RCX*2]
    //
    // REX.W = 48
    // Opcode = 8D
    //
    // ModRM:
    //   00 000 100 = 04
    //
    // SIB:
    //   scale = 01 -> x2
    //   index = RCX = 001
    //   base  = RBX = 011
    //
    // 01 001 011 = 4B
    //
    // Final encoding:
    // 48 8D 04 4B
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [RBX + RCX*2]",
        .program = {
            0x48, 0x8D, 0x04, 0x4B
        },
        .initial_regs = {
            {Register::RBX, 0x2000},
            {Register::RCX, 0x0100}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x2200},
            {Register::RBX, 0x2000},
            {Register::RCX, 0x0100}
        },
        .expected_mem = {}
    });

    // -------------------------------------------------------------------------
    // LEA RAX, [RBX + RDX*4]
    //
    // REX.W = 48
    // Opcode = 8D
    //
    // ModRM:
    //   00 000 100 = 04
    //
    // SIB:
    //   scale = 10 -> x4
    //   index = RDX = 010
    //   base  = RBX = 011
    //
    // 10 010 011 = 93
    //
    // Final encoding:
    // 48 8D 04 93
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [RBX + RDX*4]",
        .program = {
            0x48, 0x8D, 0x04, 0x93
        },
        .initial_regs = {
            {Register::RBX, 0x2000},
            {Register::RDX, 0x0050}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x2140},
            {Register::RBX, 0x2000},
            {Register::RDX, 0x0050}
        },
        .expected_mem = {}
    });

    // -------------------------------------------------------------------------
    // LEA RAX, [RSI + 0x12345678]
    //
    // REX.W = 48
    // Opcode = 8D
    //
    // ModRM:
    //   MOD = 10 -> disp32
    //   REG = RAX = 000
    //   R/M = RSI = 110
    //
    // 10 000 110 = 86
    //
    // disp32 = 0x12345678
    // Little endian:
    // 78 56 34 12
    //
    // Final encoding:
    // 48 8D 86 78 56 34 12
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [RSI + 0x12345678]",
        .program = {
            0x48, 0x8D, 0x86,
            0x78, 0x56, 0x34, 0x12
        },
        .initial_regs = {
            {Register::RSI, 0x0000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x12345678ULL},
            {Register::RSI, 0x0000}
        },
        .expected_mem = {}
    });

    // -------------------------------------------------------------------------
    // LEA RAX, [R8]
    //
    // R8 requires REX.B = 1.
    // RAX destination with 64-bit operand requires REX.W = 1.
    //
    // REX.W + REX.B:
    //
    // 0100 WRXB
    //      1 001
    //
    // = 49
    //
    // Opcode = 8D
    //
    // ModRM:
    //   MOD = 00
    //   REG = RAX = 000
    //   R/M = 000
    //
    // REX.B extends R/M from RAX to R8.
    //
    // 00 000 000 = 00
    //
    // Final encoding:
    // 49 8D 00
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "LEA RAX, [R8]",
        .program = {
            0x49, 0x8D, 0x00
        },
        .initial_regs = {
            {Register::R8, 0x0A00}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0A00},
            {Register::R8, 0x0A00}
        },
        .expected_mem = {}
    });

    return tests;
}

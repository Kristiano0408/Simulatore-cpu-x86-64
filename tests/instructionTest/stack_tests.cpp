// -----------------------------------------------------------------------------
// PUSH / POP: stack data movement (RSP managed only by the RegisterFile)
// -----------------------------------------------------------------------------
//
// x86-64 semantics used by these tests:
//   PUSH src : RSP <- RSP - 8 ; Mem[RSP] <- src   (always 8 bytes, also for
//              "PUSH r32": the 32-bit value is zero-extended to 64 bits)
//   POP  dst : dst <- Mem[RSP] ; RSP <- RSP + 8
//
// PUSH/POP never modify the status flags: with the flags initialized to 0 every
// flag must remain 0 after execution.
//
// RSP is initialized by the test loader (the CPU never sets it by itself).

#include "../instructionTest.hpp"

std::vector<TestCase> buildStackTests()
{
    std::vector<TestCase> tests{};

    // PUSH / POP do not modify the status flags.
    const std::vector<std::pair<Flagbit, bool>> flagsUnchanged = {
        {Flagbit::CF, false},
        {Flagbit::PF, false},
        {Flagbit::AF, false},
        {Flagbit::ZF, false},
        {Flagbit::SF, false},
        {Flagbit::OF, false}
    };

    // Helper: expected 8 bytes (little-endian) of a 64-bit value stored at addr.
    auto expectQword = [](uint64_t addr, uint64_t value)
    {
        std::vector<std::pair<uint64_t, uint8_t>> bytes;
        for (unsigned i = 0; i < 8; ++i)
        {
            bytes.push_back({addr + i, static_cast<uint8_t>((value >> (i * 8)) & 0xFF)});
        }
        return bytes;
    };

    // =========================================================================
    // PUSH reg (0x50 + rd)
    // =========================================================================

    // 50
    //
    // PUSH RAX
    //
    tests.push_back({
        .description = "PUSH RAX (RSP 0x1000 -> 0xFF8, Mem[0xFF8] = RAX)",
        .program = {0x50},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RAX, 0x1122334455667788ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8},
            {Register::RAX, 0x1122334455667788ULL}
        },
        .expected_mem = expectQword(0xFF8, 0x1122334455667788ULL),
        .expected_flags = flagsUnchanged
    });

    // 53
    //
    // PUSH RBX
    //
    tests.push_back({
        .description = "PUSH RBX (RSP 0x1000 -> 0xFF8, Mem[0xFF8] = RBX)",
        .program = {0x53},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RBX, 0x00000000DEADBEEFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8},
            {Register::RBX, 0x00000000DEADBEEFULL}
        },
        .expected_mem = expectQword(0xFF8, 0x00000000DEADBEEFULL),
        .expected_flags = flagsUnchanged
    });

    // 41 50
    //
    // PUSH R8   (REX.B extends the opcode register to R8)
    //
    tests.push_back({
        .description = "PUSH R8 (REX.B, 41 50, RSP 0x1000 -> 0xFF8)",
        .program = {0x41, 0x50},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::R8, 0xCAFEBABEULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8},
            {Register::R8, 0xCAFEBABEULL}
        },
        .expected_mem = expectQword(0xFF8, 0xCAFEBABEULL),
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // PUSH r/m64 (FF /6)
    // =========================================================================

    // FF 36
    //
    // PUSH qword ptr [RSI]
    //
    // ModRM: 00 110 110 -> mod=mem, reg=/6 (PUSH), rm=RSI
    //
    tests.push_back({
        .description = "PUSH qword [RSI] (FF 36, memory value pushed, RSI unchanged)",
        .program = {0xFF, 0x36},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x88}, {0x2001, 0x77}, {0x2002, 0x66}, {0x2003, 0x55},
            {0x2004, 0x44}, {0x2005, 0x33}, {0x2006, 0x22}, {0x2007, 0x11}
        },
        .expected_regs = {
            {Register::RSP, 0xFF8},
            {Register::RSI, 0x2000}
        },
        .expected_mem = expectQword(0xFF8, 0x1122334455667788ULL),
        .expected_flags = flagsUnchanged
    });

    // FF 76 10
    //
    // PUSH qword ptr [RSI + 0x10]   (mod=01, disp8)
    //
    // ModRM: 01 110 110 -> mod=disp8, reg=/6 (PUSH), rm=RSI
    //
    tests.push_back({
        .description = "PUSH qword [RSI + 0x10] (FF 7E 10, disp8)",
        .program = {0xFF, 0x76, 0x10},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2010, 0x01}, {0x2011, 0x02}, {0x2012, 0x03}, {0x2013, 0x04},
            {0x2014, 0x05}, {0x2015, 0x06}, {0x2016, 0x07}, {0x2017, 0x08}
        },
        .expected_regs = {
            {Register::RSP, 0xFF8},
            {Register::RSI, 0x2000}
        },
        .expected_mem = expectQword(0xFF8, 0x0807060504030201ULL),
        .expected_flags = flagsUnchanged
    });

    // FF 74 FE 10
    //
    // PUSH qword ptr [RSI + RDI*8 + 0x10]   (SIB: scale=8, index=RDI, base=RSI)
    //
    // ModRM: 01 110 100 -> mod=disp8, reg=/6 (PUSH), rm=SIB ; SIB: 11 111 110
    //
    tests.push_back({
        .description = "PUSH qword [RSI + RDI*8 + 0x10] (FF 74 FE 10, SIB + disp8)",
        .program = {0xFF, 0x74, 0xFE, 0x10},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RSI, 0x2000},
            {Register::RDI, 0x0010}
        },
        .initial_flags = {},
        .initial_mem = {
            // 0x2000 + 0x0010*8 + 0x10 = 0x2090
            {0x2090, 0xAA}, {0x2091, 0xBB}, {0x2092, 0xCC}, {0x2093, 0xDD},
            {0x2094, 0xEE}, {0x2095, 0xFF}, {0x2096, 0x00}, {0x2097, 0x00}
        },
        .expected_regs = {
            {Register::RSP, 0xFF8},
            {Register::RSI, 0x2000},
            {Register::RDI, 0x0010}
        },
        .expected_mem = expectQword(0xFF8, 0x0000FFEEDDCCBBAAULL),
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // PUSH imm (6A ib / 68 id)
    // =========================================================================

    // 6A 7F
    //
    // PUSH 0x7F   (imm8, positive -> zero upper bits)
    //
    tests.push_back({
        .description = "PUSH imm8 0x7F (6A 7F -> 0x7F on stack)",
        .program = {0x6A, 0x7F},
        .initial_regs = {
            {Register::RSP, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8}
        },
        .expected_mem = expectQword(0xFF8, 0x7F),
        .expected_flags = flagsUnchanged
    });

    // 6A 82
    //
    // PUSH -126   (imm8 sign-extended to 64 bits)
    //
    tests.push_back({
        .description = "PUSH imm8 0x82 (6A 82 -> 0xFFFFFFFFFFFFFF82 on stack)",
        .program = {0x6A, 0x82},
        .initial_regs = {
            {Register::RSP, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8}
        },
        .expected_mem = expectQword(0xFF8, 0xFFFFFFFFFFFFFF82ULL),
        .expected_flags = flagsUnchanged
    });

    // 68 78 56 34 12
    //
    // PUSH 0x12345678   (imm32, positive)
    //
    tests.push_back({
        .description = "PUSH imm32 0x12345678 (68 -> 0x12345678 on stack)",
        .program = {0x68, 0x78, 0x56, 0x34, 0x12},
        .initial_regs = {
            {Register::RSP, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8}
        },
        .expected_mem = expectQword(0xFF8, 0x12345678),
        .expected_flags = flagsUnchanged
    });

    // 68 FF FF FF FF
    //
    // PUSH -1   (imm32 sign-extended to 64 bits)
    //
    tests.push_back({
        .description = "PUSH imm32 0xFFFFFFFF (68 -> 0xFFFFFFFFFFFFFFFF on stack)",
        .program = {0x68, 0xFF, 0xFF, 0xFF, 0xFF},
        .initial_regs = {
            {Register::RSP, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSP, 0xFF8}
        },
        .expected_mem = expectQword(0xFF8, 0xFFFFFFFFFFFFFFFFULL),
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // POP reg (0x58 + rd)
    // =========================================================================

    // 58
    //
    // POP RAX
    //
    tests.push_back({
        .description = "POP RAX (Mem[0x1000] -> RAX, RSP 0x1000 -> 0x1008)",
        .program = {0x58},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RAX, 0}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x88}, {0x1001, 0x77}, {0x1002, 0x66}, {0x1003, 0x55},
            {0x1004, 0x44}, {0x1005, 0x33}, {0x1006, 0x22}, {0x1007, 0x11}
        },
        .expected_regs = {
            {Register::RSP, 0x1008},
            {Register::RAX, 0x1122334455667788ULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 5B
    //
    // POP RBX
    //
    tests.push_back({
        .description = "POP RBX (Mem[0x1000] -> RBX, RSP 0x1000 -> 0x1008)",
        .program = {0x5B},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::RBX, 0}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xEF}, {0x1001, 0xBE}, {0x1002, 0xAD}, {0x1003, 0xDE},
            {0x1004, 0x00}, {0x1005, 0x00}, {0x1006, 0x00}, {0x1007, 0x00}
        },
        .expected_regs = {
            {Register::RSP, 0x1008},
            {Register::RBX, 0xDEADBEEF}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 41 58
    //
    // POP R8   (REX.B)
    //
    tests.push_back({
        .description = "POP R8 (REX.B, 41 58, RSP 0x1000 -> 0x1008)",
        .program = {0x41, 0x58},
        .initial_regs = {
            {Register::RSP, 0x1000},
            {Register::R8, 0}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xBE}, {0x1001, 0xBA}, {0x1002, 0xFE}, {0x1003, 0xCA},
            {0x1004, 0x00}, {0x1005, 0x00}, {0x1006, 0x00}, {0x1007, 0x00}
        },
        .expected_regs = {
            {Register::RSP, 0x1008},
            {Register::R8, 0xCAFEBABE}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    return tests;
}

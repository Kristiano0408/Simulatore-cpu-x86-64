// -----------------------------------------------------------------------------
// DIV / IDIV: division group (single explicit r/m operand, M_ACC)
//
// DIV  : F6 /6 (r/m8),  F7 /6 (r/m16/32/64)
// IDIV : F6 /7 (r/m8),  F7 /7 (r/m16/32/64)
//
// NOTE: like MUL/IMUL, the simulator does not model AH/AL or DX:AX separately.
// The quotient (resultValue) is written to RAX and the remainder
// (resultHighValue) to RDX for every width. The dividend high half is read
// from RDX (0 for the 8-bit form).
//
// Per Intel, DIV/IDIV leave CF/OF/SF/ZF/AF/PF undefined; the simulator does
// not commit any flags for these instructions, so the tests do not check them.
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildDivTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // DIV - unsigned division (M_ACC)
    //   F6 /6 = DIV r/m8,  F7 /6 = DIV r/m16/32/64
    // =========================================================================

    // -------------------------------------------------------------------------
    // DIV BL (8-bit unsigned)
    //
    // F6 F3 -> F6 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX/BL)
    // AX = 100 (0x0064), BL = 7 -> quotient = 14, remainder = 2
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "DIV BL (8-bit, 100/7) -> RAX=14, RDX=2",
        .program = {
            0xF6, 0xF3 // F6 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX) -> DIV r/m8 (BL)
        },
        .initial_regs = {
            {Register::RAX, 0x0064}, // AX = 100
            {Register::RBX, 7}       // BL = 7
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 14},  // quotient -> AL
            {Register::RBX, 7},   // divisor unchanged
            {Register::RDX, 2}    // remainder -> AH
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // -------------------------------------------------------------------------
    // DIV BX (16-bit unsigned, DX:AX = 32-bit dividend)
    //
    // 66 F7 E3 -> 0x66 prefix (16-bit), F7 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX)
    // DX = 1, AX = 0 -> DX:AX = 65536; BX = 2 -> quotient = 32768, remainder = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "DIV BX (16-bit, 65536/2) -> AX=32768, DX=0",
        .program = {
            0x66,       // 0x66 -> 16-bit operand size
            0xF7, 0xFB  // F7 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX) -> DIV r/m16 (BX)
        },
        .initial_regs = {
            {Register::RAX, 0},      // AX = 0
            {Register::RDX, 1},      // DX = 1 -> DX:AX = 65536
            {Register::RBX, 2}       // BX = 2
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 32768}, // quotient -> AX (0x8000)
            {Register::RDX, 0}      // remainder -> DX
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // -------------------------------------------------------------------------
    // DIV RBX (64-bit unsigned, RDX:RAX = 128-bit dividend)
    //
    // REX.W + F7 E3 -> F7 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX)
    // RDX = 1, RAX = 0 -> dividend = 2^64; RBX = 2 -> quotient = 2^63, remainder = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "DIV RBX (64-bit, 2^64/2) -> RAX=0x8000000000000000, RDX=0",
        .program = {
            0x48,       // REX.W
            0xF7, 0xFB  // F7 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX) -> DIV r/m64 (RBX)
        },
        .initial_regs = {
            {Register::RAX, 0},
            {Register::RDX, 1},      // RDX:RAX = 2^64
            {Register::RBX, 2}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x8000000000000000ULL}, // quotient = 2^63
            {Register::RDX, 0}                      // remainder
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // -------------------------------------------------------------------------
    // DIV DWORD [RSI] (32-bit unsigned, memory divisor)
    //
    // F7 26 -> F7 /6, mod=00, reg=110(/6 DIV), r/m=110(RSI) -> DIV dword [RSI]
    // EDX:EAX = 1000, [0x200] = 10 -> quotient = 100, remainder = 0
    // Memory must NOT be written (divisor only).
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "DIV DWORD [RSI] (32-bit, 1000/10) -> EAX=100, EDX=0, memory unchanged",
        .program = {
            0xF7, 0x36 // F7 /6, mod=00, reg=110(/6 DIV), r/m=110(RSI) -> DIV dword [RSI]
        },
        .initial_regs = {
            {Register::RAX, 1000}, // dividend low half (EDX = 0)
            {Register::RSI, 0x200}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x200, 10} // divisor = 10 (little-endian low byte)
        },
        .expected_regs = {
            {Register::RAX, 100}, // quotient -> EAX
            {Register::RDX, 0}    // remainder -> EDX
        },
        .expected_mem = {
            {0x200, 10} // memory unchanged
        },
        .expected_flags = {}
    });

    // =========================================================================
    // IDIV - signed division (M_ACC)
    //   F6 /7 = IDIV r/m8,  F7 /7 = IDIV r/m16/32/64
    // =========================================================================

    // -------------------------------------------------------------------------
    // IDIV BL (8-bit signed, negative dividend)
    //
    // F6 FB -> F6 /7, mod=11, reg=111(/7 IDIV), r/m=011(RBX/BL)
    // AX = 0xFFF6 (-10), BL = 3 -> quotient = -3 (0xFD), remainder = -1 (0xFF)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IDIV BL (8-bit, -10/3) -> AL=0xFD(-3), AH=0xFF(-1)",
        .program = {
            0xF6, 0xFB // F6 /7, mod=11, reg=111(/7 IDIV), r/m=011(RBX) -> IDIV r/m8 (BL)
        },
        .initial_regs = {
            {Register::RAX, 0xFFF6}, // AX = -10
            {Register::RBX, 3}       // BL = 3
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFD}, // quotient -> AL (-3)
            {Register::RBX, 3},    // divisor unchanged
            {Register::RDX, 0xFF}  // remainder -> AH (-1)
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // -------------------------------------------------------------------------
    // IDIV EBX (32-bit signed, dividend and divisor both negative)
    //
    // F7 E3 -> F7 /7, mod=11, reg=111(/7 IDIV), r/m=011(RBX)
    // EDX:EAX = -100 (EDX=0xFFFFFFFF, EAX=0xFFFFFF9C), EBX = -5 -> quotient = 20, remainder = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IDIV EBX (32-bit, -100/-5) -> EAX=20, EDX=0",
        .program = {
            0xF7, 0xFB // F7 /7, mod=11, reg=111(/7 IDIV), r/m=011(RBX) -> IDIV r/m32 (EBX)
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFF9CULL}, // EAX = -100 (low half)
            {Register::RDX, 0xFFFFFFFFULL}, // EDX = sign extension (high half)
            {Register::RBX, 0xFFFFFFFBULL}  // EBX = -5
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 20}, // quotient -> EAX
            {Register::RDX, 0}   // remainder -> EDX
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // =========================================================================
    // Extra cases: 64-bit signed division and remainder sign check
    // =========================================================================

    // -------------------------------------------------------------------------
    // IDIV RBX (64-bit signed, negative dividend)
    //
    // REX.W + F7 E3 -> F7 /7, mod=11, reg=111(/7 IDIV), r/m=011(RBX)
    // RDX:RAX = -100 (RDX = sign extension), RBX = 7 -> quotient = -14, remainder = -2
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "IDIV RBX (64-bit, -100/7) -> RAX=-14, RDX=-2",
        .program = {
            0x48,       // REX.W
            0xF7, 0xFB  // F7 /7, mod=11, reg=111(/7 IDIV), r/m=011(RBX) -> IDIV r/m64 (RBX)
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFF9CULL}, // RAX = -100 (low half)
            {Register::RDX, 0xFFFFFFFFFFFFFFFFULL}, // RDX = sign extension (high half)
            {Register::RBX, 7}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFF2ULL}, // quotient = -14
            {Register::RDX, 0xFFFFFFFFFFFFFFFEULL}  // remainder = -2
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // -------------------------------------------------------------------------
    // DIV RBX (64-bit unsigned, exact quotient with non-zero remainder)
    //
    // REX.W + F7 E3 -> F7 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX)
    // RDX:RAX = 1000000007, RBX = 1000000 -> quotient = 1000, remainder = 7
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "DIV RBX (64-bit, 1000000007/1000000) -> RAX=1000, RDX=7",
        .program = {
            0x48,       // REX.W
            0xF7, 0xFB  // F7 /6, mod=11, reg=110(/6 DIV), r/m=011(RBX) -> DIV r/m64 (RBX)
        },
        .initial_regs = {
            {Register::RAX, 1000000007ULL}, // dividend low half (RDX = 0)
            {Register::RBX, 1000000ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 1000}, // quotient
            {Register::RDX, 7}     // remainder
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    return tests;
}

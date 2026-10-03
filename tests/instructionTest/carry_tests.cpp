// -----------------------------------------------------------------------------
// ADC / SBB: carry chain plus 128-bit chains
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildCarryTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // ADC - immediate to accumulator (I mode)
    // =========================================================================

    tests.push_back({
        .description = "ADC AL, imm8 (CF=0)",
        .program = {
            0x14, 0x05
        },
        .initial_regs = {
            {Register::RAX, 0x10}
        },
        .initial_flags = {
            {Flagbit::CF, false}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x15}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "ADC AL, imm8 (CF=1, carry propagates)",
        .program = {
            0x14, 0x05
        },
        .initial_regs = {
            {Register::RAX, 0x10}
        },
         .initial_flags = {
            {Flagbit::CF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x16}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "ADC AL, imm8 (overflow, 8-bit)",
        .program = {
            0x14, 0x00
        },
        .initial_regs = {
            {Register::RAX, 0xFF}
        },
        .initial_flags = {
            {Flagbit::CF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "ADC RAX, RBX (64-bit multi-precision)",
        .program = {
            0x48, 0x11, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL},
            {Register::RBX, 0x0}
        },
        .initial_flags = {
            {Flagbit::CF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0000000000000000ULL},
            {Register::RBX, 0x0}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "ADC QWORD [RSI], imm8 (memory)",
        .program = {
            0x48, 0x83, 0x16, 0x04
        },
        .initial_regs = {
            {Register::RSI, 0x100}
        },
        .initial_flags = {
            {Flagbit::CF, true}
        },
        .initial_mem = {
            {0x100, 0x10}
        },
        .expected_regs = {
            {Register::RSI, 0x100}
        },
        .expected_mem = {
            {0x100, 0x15}
        }
    });

    // =========================================================================
    // SBB - immediate to accumulator (I mode)
    // =========================================================================

    tests.push_back({
        .description = "SBB AL, imm8 (CF=0)",
        .program = {
            0x1C, 0x05
        },
        .initial_regs = {
            {Register::RAX, 0x20}
        },
        .initial_flags = {
            {Flagbit::CF, false}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "SBB AL, imm8 (CF=1, borrow)",
        .program = {
            0x1C, 0x05
        },
        .initial_regs = {
            {Register::RAX, 0x20}
        },
        .initial_flags = {
            {Flagbit::CF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1A}
        },
        .expected_mem = {}
    });

    // SBB RAX, imm8  ->  REX.W + 83 /3 ib, ModRM mod=11 reg=3 r/m=0 = 0xD8
    // 0 - 1 = -1 -> 0xFFFFFFFFFFFFFFFF (sottoflusso a 64 bit)
    tests.push_back({
        .description = "SBB RAX, imm8 (underflow 64-bit)",
        .program = {
            0x48, 0x83, 0xD8, 0x01
        },
        .initial_regs = {
            {Register::RAX, 0x0}
        },
        .initial_flags = {
            {Flagbit::CF, false}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // Catena 128-bit: ADD low + ADC high
    // =========================================================================

    // ADC RDX, RCX  ->  REX.W + 11 /r con r/m=dest(RDX=010), reg=src(RCX=001)
    // ModRM = 11 001 010 = 0xCA
    tests.push_back({
        .description = "Catena: ADD RAX,RBX then ADC RDX,RCX",
        .program = {
            0x48, 0x01, 0xD8, // ADD RAX, RBX
            0x48, 0x11, 0xCA  // ADC RDX, RCX
        },
        .initial_regs = {
            {Register::RAX, 0x0000000000000005ULL},
            {Register::RBX, 0x0000000000000003ULL},
            {Register::RDX, 0x0000000000000001ULL},
            {Register::RCX, 0x0000000000000002ULL}
        },
        .initial_flags = {
            {Flagbit::CF, false}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0000000000000008ULL},
            {Register::RBX, 0x0000000000000003ULL},
            // 5 + 3 = 8 -> nessun carry, quindi RDX = 1 + 2 + 0
            {Register::RDX, 0x0000000000000003ULL},
            {Register::RCX, 0x0000000000000002ULL}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // Catena 128-bit: SUB low + SBB high
    // =========================================================================

    // SBB RDX, RCX  ->  REX.W + 19 /r (SBB r/m, r): r/m=DEST(RDX=010), reg=SRC(RCX=001)
    // ModRM = 11 001 010 = 0xCA
    tests.push_back({
        .description = "Catena: SUB RAX,RBX then SBB RDX,RCX",
        .program = {
            0x48, 0x29, 0xD8, // SUB RAX, RBX
            0x48, 0x19, 0xCA  // SBB RDX, RCX
        },
        .initial_regs = {
            // 5 - 10 sottofluisce -> CF = 1 (borrow propagato nella parte alta)
            {Register::RAX, 0x0000000000000005ULL},
            {Register::RBX, 0x0000000000000010ULL},
            {Register::RDX, 0x0000000000000001ULL},
            {Register::RCX, 0x0000000000000002ULL}
        },
        .initial_flags = {
            {Flagbit::CF, false}
        },
        .initial_mem = {},
        .expected_regs = {
            // 5 - 16 = -11
            {Register::RAX, 0xFFFFFFFFFFFFFFF5ULL},
            {Register::RBX, 0x0000000000000010ULL},
            // 1 - 2 - CF(1) = -2
            {Register::RDX, 0xFFFFFFFFFFFFFFFEULL},
            {Register::RCX, 0x0000000000000002ULL}
        },
        .expected_mem = {}
    });


    return tests;
}

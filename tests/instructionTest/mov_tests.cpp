// -----------------------------------------------------------------------------
// MOV: immediate/reg/memory moves
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildMovTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // MOV - immediate -> register
    // =========================================================================

    tests.push_back({
        .description = "MOV AL, imm8",
        .program = {
            0xB0, 0x11
        },
        .initial_regs = {},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x11}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "MOV CL, imm8",
        .program = {
            0xB1, 0x22
        },
        .initial_regs = {},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RCX, 0x22}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "MOV R8, imm64",
        .program = {
            0x41, 0xB8,
            0x78, 0x56, 0x34, 0x12,
            0x00, 0x00, 0x00, 0x00
        },
        .initial_regs = {},
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::R8, 0x12345678ULL}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // MOV - register -> register
    // =========================================================================

    tests.push_back({
        .description = "MOV RAX, RBX",
        .program = {
            0x48, 0x89, 0xD8
        },
        .initial_regs = {
            {Register::RBX, 0x12345678ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x12345678ULL},
            {Register::RBX, 0x12345678ULL}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "MOV RBX, RAX",
        .program = {
            0x48, 0x89, 0xC3
        },
        .initial_regs = {
            {Register::RAX, 0xCAFEBABEULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RBX, 0xCAFEBABEULL},
            {Register::RAX, 0xCAFEBABEULL}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // MOV - register -> memory
    // =========================================================================

    // RSI = 0x1000
    // RAX = 0xCAFEBABE
    //
    // 48 89 06
    //
    // MOV [RSI], RAX
    //
    // ModRM:
    //
    // 00 000 110
    // │  │   └── R/M = RSI
    // │  └────── REG = RAX
    // └───────── MOD = memory
    //
    tests.push_back({
        .description = "MOV [RSI], RAX",
        .program = {
            0x48, 0x89, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xCAFEBABEULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xCAFEBABEULL}
        },
        .expected_mem = {
            {0x1000, 0xBE},
            {0x1001, 0xBA},
            {0x1002, 0xFE},
            {0x1003, 0xCA}
        }
    });

    // =========================================================================
    // MOV - memory -> register
    // =========================================================================

    tests.push_back({
        .description = "MOV RBX, [RSI]",
        .program = {
            0x48, 0x8B, 0x1E
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xBE},
            {0x1001, 0xBA},
            {0x1002, 0xFE},
            {0x1003, 0xCA}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RBX, 0xCAFEBABEULL}
        },
        .expected_mem = {}
    });


    return tests;
}

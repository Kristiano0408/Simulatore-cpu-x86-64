// -----------------------------------------------------------------------------
// ADD and SUB: immediate, register and memory forms
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildAddSubTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // ADD - immediate
    // =========================================================================

    tests.push_back({
        .description = "ADD AL, imm8",
        .program = {
            0x04, 0x05
        },
        .initial_regs = {
            {Register::RAX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0F}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "ADD EAX, imm32",
        .program = {
            0x05,
            0x03, 0x00, 0x00, 0x00
        },
        .initial_regs = {
            {Register::RAX, 0x0F}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x12}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "ADD RAX, imm32",
        .program = {
            0x48, 0x05,
            0x10, 0x00, 0x00, 0x00
        },
        .initial_regs = {
            {Register::RAX, 0x12}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x22}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // ADD - register -> register
    // =========================================================================

    tests.push_back({
        .description = "ADD RAX, RBX",
        .program = {
            0x48, 0x01, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 5},
            {Register::RBX, 3}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 8},
            {Register::RBX, 3}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // SUB - immediate
    // =========================================================================

    tests.push_back({
        .description = "SUB AL, imm8",
        .program = {
            0x2C, 0x03
        },
        .initial_regs = {
            {Register::RAX, 0x08}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x05}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "SUB EAX, imm32",
        .program = {
            0x2D,
            0x01, 0x00, 0x00, 0x00
        },
        .initial_regs = {
            {Register::RAX, 0x05}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x04}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "SUB RAX, imm32",
        .program = {
            0x48, 0x2D,
            0x02, 0x00, 0x00, 0x00
        },
        .initial_regs = {
            {Register::RAX, 0x04}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x02}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // SUB - register -> register
    // =========================================================================

    tests.push_back({
        .description = "SUB RAX, RBX",
        .program = {
            0x48, 0x29, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 10},
            {Register::RBX, 3}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 7},
            {Register::RBX, 3}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // ADD - memory
    // =========================================================================

    // RSI = 0x2000
    //
    // ADD BYTE PTR [RSI], 1
    //
    // 80 /0 ib
    //
    // ModRM = 00 000 110 = 06
    //
    // 80 06 01
    //
    tests.push_back({
        .description = "ADD BYTE [RSI], imm8",
        .program = {
            0x80, 0x06, 0x01
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x05}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, 0x06}
        }
    });

    // =========================================================================
    // SUB - memory
    // =========================================================================

    // RSI = 0x3000
    //
    // SUB DWORD PTR [RSI], 2
    //
    // 83 /5 ib
    //
    // ModRM = 00 101 110 = 2E
    //
    // 83 2E 02
    //
    tests.push_back({
        .description = "SUB DWORD [RSI], imm8",
        .program = {
            0x83, 0x2E, 0x02
        },
        .initial_regs = {
            {Register::RSI, 0x3000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x3000, 0x10},
            {0x3001, 0x00},
            {0x3002, 0x00},
            {0x3003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x3000}
        },
        .expected_mem = {
            {0x3000, 0x0E},
            {0x3001, 0x00},
            {0x3002, 0x00},
            {0x3003, 0x00}
        }
    });


    return tests;
}

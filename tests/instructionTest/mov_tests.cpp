// -----------------------------------------------------------------------------
// MOV: immediate/reg/memory moves
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"



TEST_P(InstructionTest, Instruction_MOV_CorrectExecution)
{
    const auto& test = GetParam();
    // -------------------------------------------------------------------------
    // Registers
    // -------------------------------------------------------------------------

    for (const auto& [reg, expected] : test.expected_regs)
    {
        const uint64_t got =
            bus.getCPU().getRegisters().getReg(reg).raw();

        EXPECT_EQ(got,expected)<< "Register " << toStringRegister(reg) << " expected: " << std::hex << expected << ", got: " << std::hex << got;
    }

    // -------------------------------------------------------------------------
    // Memory
    // -------------------------------------------------------------------------

    for (const auto& [addr, expected] : test.expected_mem)
    {
        const uint8_t got =
            bus.getMemory().readTest(addr);

        EXPECT_EQ(got, expected) << "Memory at address " << std::hex << addr << " expected: " << std::hex << expected << ", got: " << std::hex << got;
    }

    // -------------------------------------------------------------------------
    // Flags
    // -------------------------------------------------------------------------

    for (const auto& [flag, expected] : test.expected_flags)
    {
        const bool got =
            bus.getCPU().getRegisters().getFlags().getFlag(flag);

        EXPECT_EQ(got, expected) << "Flag " << toStringFlag(flag) << " expected: " << expected << ", got: " << got;
    }

}







INSTANTIATE_TEST_SUITE_P(Instruction_MOV_CorrectExecution, InstructionTest, ::testing::ValuesIn(buildMovTests()),
                         [](const ::testing::TestParamInfo<TestCase>& paramInfo)
                         {
                             // The TestCase_<index> prefix guarantees uniqueness even
                             // when two descriptions collapse to the same sanitized name.
                             return "TestCase_" + std::to_string(paramInfo.index) + "_" +
                                    sanitizeTestName(paramInfo.param.description);
                         });

    

std::vector<TestCase> buildMovTests()
{
    std::vector<TestCase> tests{};

    // MOV does not modify the status flags: with the flags initialized to 0,
    // every flag must remain 0 after execution.
    const std::vector<std::pair<Flagbit, bool>> flagsUnchanged = {
        {Flagbit::CF, false},
        {Flagbit::PF, false},
        {Flagbit::AF, false},
        {Flagbit::ZF, false},
        {Flagbit::SF, false},
        {Flagbit::OF, false}
    };

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
        .expected_mem = {},
        .expected_flags = flagsUnchanged
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
        .expected_mem = {},
        .expected_flags = flagsUnchanged
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
        .expected_mem = {},
        .expected_flags = flagsUnchanged
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
        .expected_mem = {},
        .expected_flags = flagsUnchanged
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
        .expected_mem = {},
        .expected_flags = flagsUnchanged
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
        },
        .expected_flags = flagsUnchanged
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
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });


    return tests;
}

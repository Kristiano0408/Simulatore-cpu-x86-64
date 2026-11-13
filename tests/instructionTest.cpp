// -----------------------------------------------------------------------------
// Instruction test suite driver.
//
// Test cases live one file per instruction group under tests/instructionTest/;
// this file only aggregates them and runs them against the simulated CPU.
// -----------------------------------------------------------------------------

#include "instructionTest.hpp"

#include "../include/bus.hpp"
#include "../include/cpu.hpp"
#include "../include/registerFile.hpp"
#include "../include/helpers/debug.hpp"

#include <cctype>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using std::hex;

// -----------------------------------------------------------------------------
// Test-name sanitizer
//
// GoogleTest parameterized-test names must match [A-Za-z0-9_] and be unique
// inside an INSTANTIATE block. Descriptions like "MOV AL, imm8" or
// "LEA RAX, [RBX+RCX*8+0x10]" contain spaces, commas, brackets, etc., which
// abort the binary at name-generation time: every non-alphanumeric character
// is replaced with '_'.
// -----------------------------------------------------------------------------

static std::string sanitizeTestName(std::string name)
{
    for (char& c : name)
    {
        if (!std::isalnum(static_cast<unsigned char>(c)))
        {
            c = '_';
        }
    }

    return name;
}

// -----------------------------------------------------------------------------
// Build tests: concatenation of every instruction group, in the original order
// -----------------------------------------------------------------------------

std::vector<TestCase> buildTestCases()
{
    std::vector<TestCase> tests{};

    const auto append = [&tests](std::vector<TestCase> group)
    {
        tests.insert(tests.end(),
                     std::make_move_iterator(group.begin()),
                     std::make_move_iterator(group.end()));
    };

    /*append(buildMovTests());
    append(buildAddSubTests());
    append(buildCarryTests());
    append(buildLeaTests());
    append(buildUnaryTests());
    append(buildCmpTests());
    append(buildMulTests());
    append(buildDivTests());
    append(buildExtensionTests());
    append(buildStackTests());
    append(buildBooleanTests());
    append(buildTestTests());
    append(buildShiftTests());
    append(buildRotateTests());*/
    append(buildJmpTests());

    return tests;
}



void InstructionTest::SetUp()
{
    const auto& test = GetParam();

     bus.getMemory().setDataPartial(test.program, 0x0000);

    // -------------------------------------------------------------------------
    // Initialize registers
    // -------------------------------------------------------------------------

    for (const auto& [reg, value] : test.initial_regs)
    {
        bus.getCPU().getRegisters().getReg(reg).raw() = value;
    }

    for (const auto& [flag, value] : test.initial_flags)
    {
        bus.getCPU().getRegisters().getFlags().setFlag(flag, value);
    }

    // -------------------------------------------------------------------------
    // Initialize data memory
    // -------------------------------------------------------------------------

    for (const auto& [addr, value] : test.initial_mem)
    {
        bus.getMemory().writeTest(addr, value);
    }

    // -------------------------------------------------------------------------
    // Run CPU
    // -------------------------------------------------------------------------
    //
    // We intentionally give the CPU enough cycles for:
    //
    //   pipeline
    //   cache lookup
    //   L1 miss
    //   L2 miss
    //   L3 miss
    //   RAM access
    //   cache line fills
    //   eventual write-back
    //
    // This is a functional test, not a performance benchmark.
    //

    constexpr size_t MAX_CYCLES = 100;

    for (size_t c = 0; c < MAX_CYCLES; ++c)
    {
    
        DEBUG_LOG(debugLog("Cycle " + std::to_string(c)));
        bus.tick();
    }

    bus.getCPU().getCacheManager().flushAllCaches();

}




TEST_P(InstructionTest, InstructionCorrectExecution)
{
    const auto& test = GetParam();
    // -------------------------------------------------------------------------
    // Registers
    // -------------------------------------------------------------------------

    for (const auto& [reg, expected] : test.expected_regs)
    {
        const uint64_t got =
            bus.getCPU().getRegisters().getReg(reg).raw();

        EXPECT_EQ(got,expected)<< "Register " << toStringRegister(reg) << " expected: " << hex << expected << ", got: " << hex << got;
    }

    // -------------------------------------------------------------------------
    // Memory
    // -------------------------------------------------------------------------

    for (const auto& [addr, expected] : test.expected_mem)
    {
        const uint8_t got =
            bus.getMemory().readTest(addr);

        EXPECT_EQ(got, expected) << "Memory at address " << hex << addr << " expected: " << hex << expected << ", got: " << hex << got;
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


INSTANTIATE_TEST_SUITE_P(InstructionCorrectExecutionTests, InstructionTest, ::testing::ValuesIn(buildTestCases()),
                         [](const ::testing::TestParamInfo<TestCase>& paramInfo)
                         {
                             // The TestCase_<index> prefix guarantees uniqueness even
                             // when two descriptions collapse to the same sanitized name.
                             return "TestCase_" + std::to_string(paramInfo.index) + "_" +
                                    sanitizeTestName(paramInfo.param.description);
                         });
    



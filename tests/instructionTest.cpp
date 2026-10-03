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

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using std::hex;

// -----------------------------------------------------------------------------
// Register name helper
// -----------------------------------------------------------------------------

std::string regName(Register r)
{
    switch (r)
    {
        case Register::RAX: return "RAX";
        case Register::RBX: return "RBX";
        case Register::RCX: return "RCX";
        case Register::RDX: return "RDX";
        case Register::RSI: return "RSI";
        case Register::RDI: return "RDI";
        case Register::RSP: return "RSP";
        case Register::RBP: return "RBP";
        case Register::R8:  return "R8";
        case Register::R9:  return "R9";
        case Register::R10: return "R10";
        case Register::R11: return "R11";
        case Register::R12: return "R12";
        case Register::R13: return "R13";
        case Register::R14: return "R14";
        case Register::R15: return "R15";
        default: return "UNKNOWN";
    }
}

std::string flagName(Flagbit f)
{
    switch (f)
    {
        case Flagbit::CF: return "CF";
        case Flagbit::PF: return "PF";
        case Flagbit::AF: return "AF";
        case Flagbit::ZF: return "ZF";
        case Flagbit::SF: return "SF";
        case Flagbit::OF: return "OF";
        default: return "UNKNOWN";
    }
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

    append(buildMovTests());
    append(buildAddSubTests());
    append(buildCarryTests());
    append(buildLeaTests());
    append(buildUnaryTests());
    append(buildCmpTests());

    return tests;
}

// -----------------------------------------------------------------------------
// Run single test
// -----------------------------------------------------------------------------

void runTest(const TestCase& tc)
{
    Bus bus;

    // -------------------------------------------------------------------------
    // Initialize program memory
    // -------------------------------------------------------------------------

    bus.getMemory().setDataPartial(tc.program, 0x0000);

    // -------------------------------------------------------------------------
    // Initialize registers
    // -------------------------------------------------------------------------

    for (const auto& [reg, value] : tc.initial_regs)
    {
        bus.getCPU().getRegisters().getReg(reg).raw() = value;
    }

    for (const auto& [flag, value] : tc.initial_flags)
    {
        bus.getCPU().getRegisters().getFlags().setFlag(flag, value);
    }

    // -------------------------------------------------------------------------
    // Initialize data memory
    // -------------------------------------------------------------------------

    for (const auto& [addr, value] : tc.initial_mem)
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

    constexpr size_t MAX_CYCLES = 500;

    for (size_t c = 0; c < MAX_CYCLES; ++c)
    {
        DEBUG_LOG(debugLog("Cycle " + std::to_string(c)));
        bus.tick();
    }

    bus.getCPU().getCacheManager().flushAllCaches();

    // -------------------------------------------------------------------------
    // Verify
    // -------------------------------------------------------------------------

    bool success = true;

    // -------------------------------------------------------------------------
    // Registers
    // -------------------------------------------------------------------------

    for (const auto& [reg, expected] : tc.expected_regs)
    {
        const uint64_t got =
            bus.getCPU().getRegisters().getReg(reg).raw();

        if (got != expected)
        {
            success = false;

            std::cout
                << "  REG " << regName(reg)
                << " mismatch: expected 0x"
                << hex << expected
                << " got 0x"
                << got
                << '\n';
        }
    }

    // -------------------------------------------------------------------------
    // Memory
    // -------------------------------------------------------------------------

    for (const auto& [addr, expected] : tc.expected_mem)
    {
        const uint8_t got =
            bus.getMemory().readTest(addr);

        if (got != expected)
        {
            success = false;

            std::cout
                << "  MEM @"
                << hex << addr
                << " mismatch: expected 0x"
                << static_cast<unsigned>(expected)
                << " got 0x"
                << static_cast<unsigned>(got)
                << '\n';
        }
    }

    // -------------------------------------------------------------------------
    // Flags
    // -------------------------------------------------------------------------

    for (const auto& [flag, expected] : tc.expected_flags)
    {
        const bool got =
            bus.getCPU().getRegisters().getFlags().getFlag(flag);

        if (got != expected)
        {
            success = false;

            std::cout
                << "  FLAG " << flagName(flag)
                << " mismatch: expected "
                << (expected ? "1" : "0")
                << " got "
                << (got ? "1" : "0")
                << '\n';
        }
    }

    // -------------------------------------------------------------------------
    // Result
    // -------------------------------------------------------------------------

    std::cout
        << tc.description
        << " -> "
        << (success ? "TEST PASSED!" : "TEST FAILED!")
        << '\n';
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main()
{
    std::cout
        << "=== MOV / ADD / SUB / ADC / SBB / INC / DEC / NEG / CMP Opcode Test Suite ===\n\n";

    const auto tests = buildTestCases();

    for (const auto& test : tests)
    {
        runTest(test);
    }

    return 0;
}
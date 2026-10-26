#ifndef INSTRUCTION_TEST_HPP
#define INSTRUCTION_TEST_HPP

// -----------------------------------------------------------------------------
// Shared scaffolding for the instruction test suite.
//
// The suite is split by instruction group: every group lives in its own
// translation unit under tests/instructionTest/ and exposes a single
// buildXxxTests() function. instructionTest.cpp aggregates them and owns the
// runner (runTest) plus main().
// -----------------------------------------------------------------------------

#include "../include/registerFile.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------
// Test case
// -----------------------------------------------------------------------------

struct TestCase
{
    std::string description;

    // Program loaded at address 0
    std::vector<uint8_t> program;

    // CPU initial state
    std::vector<std::pair<Register, uint64_t>> initial_regs = {};

    std::vector<std::pair<Flagbit, bool>> initial_flags = {};

    // Memory initial state
    std::vector<std::pair<uint64_t, uint8_t>> initial_mem = {};

    // Expected final CPU state
    std::vector<std::pair<Register, uint64_t>> expected_regs = {};

    // Expected final memory state
    std::vector<std::pair<uint64_t, uint8_t>> expected_mem = {};

    // Expected final CPU flags
    std::vector<std::pair<Flagbit, bool>> expected_flags = {};
};

// -----------------------------------------------------------------------------
// Name helpers (defined in instructionTest.cpp, used by the runner)
// -----------------------------------------------------------------------------

std::string regName(Register r);
std::string flagName(Flagbit f);

// -----------------------------------------------------------------------------
// Test builders, one per instruction group
// -----------------------------------------------------------------------------

std::vector<TestCase> buildMovTests();
std::vector<TestCase> buildAddSubTests();
std::vector<TestCase> buildCarryTests();   // ADC / SBB + 128-bit chains
std::vector<TestCase> buildLeaTests();
std::vector<TestCase> buildUnaryTests();   // INC / DEC / NEG
std::vector<TestCase> buildCmpTests();
std::vector<TestCase> buildMulTests();     // MUL / IMUL (1, 2 and 3 operands)
std::vector<TestCase> buildDivTests();     // DIV / IDIV (single r/m operand, M_ACC)
std::vector<TestCase> buildExtensionTests(); // MOVZX / MOVSX / MOVSXD

#endif // INSTRUCTION_TEST_HPP

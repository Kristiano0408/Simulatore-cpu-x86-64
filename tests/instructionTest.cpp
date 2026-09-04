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

// -----------------------------------------------------------------------------
// Test case
// -----------------------------------------------------------------------------

struct TestCase
{
    std::string description;

    // Program loaded at address 0
    std::vector<uint8_t> program;

    // CPU initial state
    std::vector<std::pair<Register, uint64_t>> initial_regs;

    // Memory initial state
    std::vector<std::pair<uint64_t, uint8_t>> initial_mem;

    // Expected final CPU state
    std::vector<std::pair<Register, uint64_t>> expected_regs;

    // Expected final memory state
    std::vector<std::pair<uint64_t, uint8_t>> expected_mem;
};

// -----------------------------------------------------------------------------
// Build tests
// -----------------------------------------------------------------------------

std::vector<TestCase> buildTestCases()
{
    std::vector<TestCase> tests;

    // =========================================================================
    // MOV - immediate -> register
    // =========================================================================

    tests.push_back({
        .description = "MOV AL, imm8",
        .program = {
            0xB0, 0x11
        },
        .initial_regs = {},
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
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0A00},
            {Register::R8, 0x0A00}
        },
        .expected_mem = {}
    });
  

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

    bus.getMemory().setData(tc.program);

    // -------------------------------------------------------------------------
    // Initialize registers
    // -------------------------------------------------------------------------

    for (const auto& [reg, value] : tc.initial_regs)
    {
        bus.getCPU().getRegisters().getReg(reg).raw() = value;
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
        << "=== MOV / ADD / SUB Opcode Test Suite ===\n\n";

    const auto tests = buildTestCases();

    for (const auto& test : tests)
    {
        runTest(test);
    }

    return 0;
}
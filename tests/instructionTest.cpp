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
// Test case
// -----------------------------------------------------------------------------

struct TestCase
{
    std::string description;

    // Program loaded at address 0
    std::vector<uint8_t> program;

    // CPU initial state
    std::vector<std::pair<Register, uint64_t>> initial_regs;

    std::vector<std::pair<Flagbit, bool>> initial_flags;

    // Memory initial state
    std::vector<std::pair<uint64_t, uint8_t>> initial_mem;

    // Expected final CPU state
    std::vector<std::pair<Register, uint64_t>> expected_regs;

    // Expected final memory state
    std::vector<std::pair<uint64_t, uint8_t>> expected_mem;

    // Expected final CPU flags
    std::vector<std::pair<Flagbit, bool>> expected_flags;
};

// -----------------------------------------------------------------------------
// Build tests
// -----------------------------------------------------------------------------

std::vector<TestCase> buildTestCases()
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
        .initial_flags = {},
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
        .initial_flags = {},
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
        .initial_flags = {},
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
        .initial_flags = {},
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
        .initial_flags = {},
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
        .initial_flags = {},
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
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0A00},
            {Register::R8, 0x0A00}
        },
        .expected_mem = {}
    });

    // =========================================================================
    // INC - Single operand r/m instructions (Gruppo 4 e Gruppo 5)
    //   FE /0 = INC r/m8,  FE /1 = DEC r/m8
    //   FF /0 = INC r/m16/32/64,  FF /1 = DEC r/m16/32/64
    // =========================================================================

    tests.push_back({
        .description = "INC AL (register)",
        .program = {
            0xFE, 0xC0 // FE C0 -> INC r/m8, mod=11, reg=0(INC), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC RCX (register)",
        .program = {
            0xFF, 0xC1 // FF C1 -> INC r/m32/64, mod=11, reg=0(INC), r/m=RCX(1)
        },
        .initial_regs = {
            {Register::RCX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RCX, 0x0B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC AX (operand size override)",
        .program = {
            0x66, 0xFF, 0xC0 // 66 FF C0 -> INC r/m16 with operand-size prefix
        },
        .initial_regs = {
            {Register::RAX, 0x1234}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1235} // only lower 16 bits change: AX goes from 0x1234 to 0x1235
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC RAX (64-bit with REX.W)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xFF, 0xC0 // FF C0 -> INC r/m64, mod=11, reg=0(INC), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0A}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0B}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "DEC RAX (64-bit with REX.W)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xFF, 0xC8 // FF C8 -> DEC r/m64, mod=11, reg=1(DEC), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0B}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0A}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "DEC RCX (register)",
        .program = {
            0xFF, 0xC9 // FF C9 -> DEC r/m32/64, mod=11, reg=1(DEC), r/m=RCX(1)
        },
        .initial_regs = {
            {Register::RCX, 0x0B}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RCX, 0x0A}
        },
        .expected_mem = {}
    });

    tests.push_back({
        .description = "INC QWORD PTR [RSI] (memory)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xFF, 0x06 // FF 06 -> INC r/m64 at memory address in RSI
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, static_cast<uint8_t>(0xFF)}, // value = 0x00...00FF at [RSI] -> after INC: 0x00...0100
            {0x1001, 0x00},
            {0x1002, 0x00},
            {0x1003, 0x00},
            {0x1004, 0x00},
            {0x1005, 0x00},
            {0x1006, 0x00},
            {0x1007, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x1000} // RSI unchanged
        },
        .expected_mem = {
            {0x1000, static_cast<uint8_t>(0x00)},
            {0x1001, static_cast<uint8_t>(0x01)}, // 0xFF + 1 = 0x100 -> byte 1 = 0x01
            {0x1002, 0x00},
            {0x1003, 0x00},
            {0x1004, 0x00},
            {0x1005, 0x00},
            {0x1006, 0x00},
            {0x1007, 0x00}
        }
    });

    tests.push_back({
        .description = "DEC DWORD PTR [RDI] (memory)",
        .program = {
            0xFF, 0x0F // FF OF -> DEC r/m32 at memory address in RDI (no operand-size prefix needed for default)
        },
        .initial_regs = {
            {Register::RDI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, static_cast<uint8_t>(0x05)}, // value = 0x05 at [RDI] -> after DEC: 0x04
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_regs = {
            {Register::RDI, 0x2000} // RDI unchanged
        },
        .expected_mem = {
            {0x2000, static_cast<uint8_t>(0x04)},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        }
    });

    // =========================================================================
    // NEG - Two's complement negate (Gruppo 3, reg = 011b)
    //   F6 /3 = NEG r/m8
    //   F7 /3 = NEG r/m16/32/64
    // =========================================================================

    // -------------------------------------------------------------------------
    // NEG AL
    //
    // F6 D8
    //
    // ModRM:
    //   11 011 000 = D8
    //   MOD = 11 -> register-direct
    //   REG = 011 -> /3 (NEG)
    //   R/M = 000 -> RAX (AL)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG AL (positive value)",
        .program = {
            0xF6, 0xD8 // F6 D8 -> NEG r/m8, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x05}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFB} // -5 in two's complement (8-bit), upper bits untouched
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // dest != 0 -> borrow from a higher bit
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    tests.push_back({
        .description = "NEG AL (zero)",
        .program = {
            0xF6, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0x00}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // 0 - 0 needs no borrow
            {Flagbit::OF, false},
            {Flagbit::ZF, true},
            {Flagbit::SF, false}
        }
    });

    tests.push_back({
        .description = "NEG AL (most negative value -> overflow)",
        .program = {
            0xF6, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0x80} // -128
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x80} // -(-128) = 128 is not representable in 8 bits
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}, // overflow: operand is -2^(N-1)
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG AX (16-bit, operand-size prefix 0x66)
    //
    // 66 F7 D8
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG AX (operand size override)",
        .program = {
            0x66, // operand-size prefix -> 16-bit operand
            0xF7, 0xD8 // F7 D8 -> NEG r/m16, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0001}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFF} // only lower 16 bits change
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG EAX (32-bit default, no REX.W and no 0x66)
    //
    // F7 D8
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG EAX (32-bit default)",
        .program = {
            0xF7, 0xD8 // F7 D8 -> NEG r/m32, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0000002A} // 42
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFD6ULL} // -42 (32-bit write clears the upper half)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG RAX (64-bit, REX.W = 0x48)
    //
    // 48 F7 D8
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG RAX (64-bit with REX.W)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xF7, 0xD8 // F7 D8 -> NEG r/m64, mod=11, reg=3(NEG), r/m=RAX(0)
        },
        .initial_regs = {
            {Register::RAX, 0x0000000000000001ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL} // -1
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    tests.push_back({
        .description = "NEG RAX (64-bit overflow, 0x8000000000000000)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xF7, 0xD8
        },
        .initial_regs = {
            {Register::RAX, 0x8000000000000000ULL} // -2^63
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x8000000000000000ULL} // unchanged: 2^63 is not representable
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}, // overflow: operand is -2^(N-1)
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG BYTE PTR [RSI] (8-bit memory operand)
    //
    // F6 1E
    //
    // ModRM:
    //   00 011 110 = 1E
    //   MOD = 00 -> memory
    //   REG = 011 -> /3 (NEG)
    //   R/M = 110 -> RSI
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG BYTE PTR [RSI] (memory)",
        .program = {
            0xF6, 0x1E // F6 1E -> NEG r/m8 at memory address in RSI
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, static_cast<uint8_t>(0x0A)} // 10 -> after NEG: 0xF6 (-10)
        },
        .expected_regs = {
            {Register::RSI, 0x2000} // RSI unchanged
        },
        .expected_mem = {
            {0x2000, static_cast<uint8_t>(0xF6)}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // NEG QWORD PTR [RSI + RDI*8 + 0x10] (64-bit memory operand with SIB + disp8)
    //
    // 48 F7 5C E6 10
    //
    // REX.W = 48
    // Opcode = F7
    //
    // ModRM:
    //   MOD = 01 -> disp8
    //   REG = 011 -> /3 (NEG)
    //   R/M = 100 -> SIB follows
    //
    // 01 011 100 = 5C
    //
    // SIB:
    //   scale = 11 -> x8
    //   index = RDI = 111
    //   base  = RSI = 110
    //
    // 11 111 110 = FE
    //
    // disp8 = 10
    //
    // Address = 0x3000 + 0x2 * 8 + 0x10 = 0x3020
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "NEG QWORD PTR [RSI + RDI*8 + 0x10] (memory, SIB + disp8)",
        .program = {
            0x48, // REX.W for 64-bit operand size
            0xF7, 0x5C, 0xFE, 0x10 // F7 5C FE 10 -> NEG r/m64 at [RSI + RDI*8 + 0x10]
        },
        .initial_regs = {
            {Register::RSI, 0x3000},
            {Register::RDI, 0x2}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x3020, static_cast<uint8_t>(0x00)}, // value = 0x100 (little endian)
            {0x3021, static_cast<uint8_t>(0x01)},
            {0x3022, 0x00},
            {0x3023, 0x00},
            {0x3024, 0x00},
            {0x3025, 0x00},
            {0x3026, 0x00},
            {0x3027, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x3000}, // RSI unchanged
            {Register::RDI, 0x2}     // RDI unchanged
        },
        .expected_mem = {
            // -0x100 = 0xFFFFFFFFFFFFFF00 (little endian)
            {0x3020, static_cast<uint8_t>(0x00)},
            {0x3021, static_cast<uint8_t>(0xFF)},
            {0x3022, static_cast<uint8_t>(0xFF)},
            {0x3023, static_cast<uint8_t>(0xFF)},
            {0x3024, static_cast<uint8_t>(0xFF)},
            {0x3025, static_cast<uint8_t>(0xFF)},
            {0x3026, static_cast<uint8_t>(0xFF)},
            {0x3027, static_cast<uint8_t>(0xFF)}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // =========================================================================
    // CMP - confronta due operandi (dest - src) e aggiorna SOLO i flag:
    // registri e memoria non devono mai essere modificati
    // =========================================================================

    // -------------------------------------------------------------------------
    // CMP AL, imm8 (operandi uguali -> ZF = 1)
    //
    // 3C 20
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP AL, imm8 (equal -> ZF=1, AL unchanged)",
        .program = {
            0x3C, 0x20 // 3C ib -> CMP AL, 0x20
        },
        .initial_regs = {
            {Register::RAX, 0x20}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x20} // AL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // -------------------------------------------------------------------------
    // CMP AL, imm8 (dest < src -> CF = 1, SF = 1)
    //
    // 3C 20
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP AL, imm8 (dest < src -> CF=1, SF=1, AL unchanged)",
        .program = {
            0x3C, 0x20 // 3C ib -> CMP AL, 0x20
        },
        .initial_regs = {
            {Register::RAX, 0x10}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x10} // AL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (64-bit, REX.W + 39 /r)
    //
    // ModRM: 11 011 000 = D8
    //   MOD = 11 -> register
    //   REG = 011 -> RBX (source)
    //   R/M = 000 -> RAX (destination)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (64-bit, both operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x100},
            {Register::RBX, 0x50}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x100}, // RAX invariato
            {Register::RBX, 0x50}   // RBX invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (dest < src -> CF = 1, OF = 0)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (dest < src -> CF=1, SF=1, operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x50},
            {Register::RBX, 0x100}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x50},
            {Register::RBX, 0x100}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP DWORD PTR [RSI], imm8 (Gruppo 1, /7)
    //
    // 83 /7 ib
    //
    // ModRM: 00 111 110 = 3E
    //   MOD = 00 -> memory
    //   REG = 111 -> /7 (CMP)
    //   R/M = 110 -> RSI
    //
    // 83 3E 05
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP DWORD [RSI], imm8 (equal -> ZF=1, memory unchanged)",
        .program = {
            0x83, 0x3E, 0x05 // 83 /7 ib -> CMP DWORD PTR [RSI], 5
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x05},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x2000} // RSI invariato
        },
        .expected_mem = {
            // la memoria NON viene sovrascritta
            {0x2000, 0x05},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP DWORD PTR [RSI], imm8 (dest < src -> CF = 1)
    //
    // 83 3E 0A
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP DWORD [RSI], imm8 (dest < src -> CF=1, memory unchanged)",
        .program = {
            0x83, 0x3E, 0x0A // 83 /7 ib -> CMP DWORD PTR [RSI], 10
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x03},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, 0x03},
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP BYTE PTR [RSI], imm8 (8-bit, Gruppo 1 /7)
    //
    // 80 /7 ib
    //
    // ModRM: 00 111 110 = 3E
    //
    // 80 3E 02
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP BYTE [RSI], imm8 (dest > src -> CF=0, ZF=0, memory unchanged)",
        .program = {
            0x80, 0x3E, 0x02 // 80 /7 ib -> CMP BYTE PTR [RSI], 2
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, static_cast<uint8_t>(0x07)}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, static_cast<uint8_t>(0x07)} // invariato
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP R8, r/m8 (RM mode, 3A /r)
    //
    // ModRM: 11 000 001 = C1
    //   MOD = 11 -> register
    //   REG = 000 -> RAX (destination)
    //   R/M = 001 -> RCX (source)
    //
    // 3A C1 -> CMP AL, CL
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP AL, CL (RM 8-bit, dest < src -> CF=1, operands unchanged)",
        .program = {
            0x3A, 0xC1 // 3A /r -> CMP AL, CL
        },
        .initial_regs = {
            {Register::RAX, 0x05},
            {Register::RCX, 0x09}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x05},
            {Register::RCX, 0x09}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RDX, QWORD PTR [RSI] (RM mode, memoria come sorgente, 64-bit)
    //
    // REX.W = 48
    // Opcode = 3B
    // ModRM: 00 010 110 = 26
    //   MOD = 00 -> memory
    //   REG = 010 -> RDX (destination)
    //   R/M = 110 -> RSI (source)
    //
    // 48 3B 26
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RDX, QWORD [RSI] (mem source, dest > src -> CF=0, memory unchanged)",
        .program = {
            0x48, // REX.W
            0x3B, 0x26 // 3B /r -> CMP RDX, QWORD PTR [RSI]
        },
        .initial_regs = {
            {Register::RDX, 0x20},
            {Register::RSI, 0x4000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x4000, static_cast<uint8_t>(0x10)}, // value = 0x10
            {0x4001, 0x00},
            {0x4002, 0x00},
            {0x4003, 0x00},
            {0x4004, 0x00},
            {0x4005, 0x00},
            {0x4006, 0x00},
            {0x4007, 0x00}
        },
        .expected_regs = {
            {Register::RDX, 0x20}, // RDX invariato
            {Register::RSI, 0x4000}
        },
        .expected_mem = {
            {0x4000, static_cast<uint8_t>(0x10)}, // memoria invariata
            {0x4001, 0x00},
            {0x4002, 0x00},
            {0x4003, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP QWORD PTR [RSI + RDI*8], RDX (MR mode, SIB senza displacement, 64-bit)
    //
    // REX.W = 48
    // Opcode = 39
    //
    // ModRM: 00 010 100 = 24
    //   MOD = 00 -> memory
    //   REG = 010 -> RDX (source)
    //   R/M = 100 -> SIB follows
    //
    // SIB:
    //   scale = 11 -> x8
    //   index = RDI = 111
    //   base  = RSI = 110
    //   11 111 110 = FE
    //
    // Address = 0x3000 + 0x1 * 8 = 0x3008
    //
    // 48 39 24 FE
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP QWORD [RSI + RDI*8], RDX (SIB, dest < src -> CF=1, memory unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0x24, 0xFE // 39 /r + SIB -> CMP QWORD PTR [RSI + RDI*8], RDX
        },
        .initial_regs = {
            {Register::RSI, 0x3000},
            {Register::RDI, 0x1},
            {Register::RDX, 0x20}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x3008, static_cast<uint8_t>(0x10)}, // value = 0x10 (little endian)
            {0x3009, 0x00},
            {0x300A, 0x00},
            {0x300B, 0x00},
            {0x300C, 0x00},
            {0x300D, 0x00},
            {0x300E, 0x00},
            {0x300F, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x3000}, // RSI invariato
            {Register::RDI, 0x1},    // RDI invariato
            {Register::RDX, 0x20}    // RDX invariato
        },
        .expected_mem = {
            // [0x3008] NON viene sovrascritto
            {0x3008, static_cast<uint8_t>(0x10)},
            {0x3009, 0x00},
            {0x300A, 0x00},
            {0x300B, 0x00},
            {0x300C, 0x00},
            {0x300D, 0x00},
            {0x300E, 0x00},
            {0x300F, 0x00}
        },
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false},
            {Flagbit::PF, true}
        }
    });

    // -------------------------------------------------------------------------
    // CMP EAX, imm32 (3D id)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP EAX, imm32 (equal -> ZF=1, EAX unchanged)",
        .program = {
            0x3D,
            0x00, 0x01, 0x00, 0x00 // imm32 = 0x100
        },
        .initial_regs = {
            {Register::RAX, 0x100}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x100}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, imm32 (REX.W + 3D id, operanda a 64 bit)
    //
    // NOTA: il decoder del simulatore carica l'immediato zero-extended
    // (info.bit_extension non viene applicato), quindi l'immediato e' 0x20.
    // 0x10 - 0x20 = -0x10 -> borrow (CF=1), risultato 0xFFFFFFFFFFFFFFF0 (SF=1)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, imm32 (REX.W 64-bit, dest < src -> CF=1, SF=1, RAX unchanged)",
        .program = {
            0x48, // REX.W
            0x3D,
            0x20, 0x00, 0x00, 0x00 // imm32 = 0x20
        },
        .initial_regs = {
            {Register::RAX, 0x10}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x10}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (dest positivo, src negativo)
    //
    // RAX = 0x0000000000000001, RBX = 0xFFFFFFFFFFFFFFFF (-1)
    // unsigned: 1 < 0xFFFFFFFFFFFFFFFF -> CF = 1 (borrow)
    // signed:   1 - (-1) = 2 -> positivo, nessun overflow -> OF = 0, SF = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (positive - negative -> CF=1, OF=0, operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0x1},
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1},
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, true},
            {Flagbit::SF, false},
            {Flagbit::OF, false}
        }
    });

    // -------------------------------------------------------------------------
    // CMP RAX, RBX (dest negativo, src positivo)
    //
    // RAX = 0xFFFFFFFFFFFFFFFF (-1), RBX = 0x1
    // signed:   -1 - 1 = -2 -> 0xFFFFFFFFFFFFFFFE -> SF = 1, nessun overflow -> OF = 0
    // unsigned: 0xFFFFFFFFFFFFFFFF >= 1 -> nessun borrow -> CF = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "CMP RAX, RBX (negative - positive -> SF=1, OF=0, CF=0, operands unchanged)",
        .program = {
            0x48, // REX.W
            0x39, 0xD8 // 39 D8 -> CMP RAX, RBX
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL},
            {Register::RBX, 0x1}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL},
            {Register::RBX, 0x1}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::SF, true},
            {Flagbit::OF, false}
        }
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
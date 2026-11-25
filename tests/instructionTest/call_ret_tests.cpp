#include "../instructionTest.hpp"
#include "gtest/gtest.h"

using std::hex;




TEST_P(InstructionTest, Instruction_Call_Ret_CorrectExecution)
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





INSTANTIATE_TEST_SUITE_P(Instruction_Call_Ret_CorrectExecution, InstructionTest, ::testing::ValuesIn(buildCallRetTests()),
                         [](const ::testing::TestParamInfo<TestCase>& paramInfo)
                         {
                             // The TestCase_<index> prefix guarantees uniqueness even
                             // when two descriptions collapse to the same sanitized name.
                             return "TestCase_" + std::to_string(paramInfo.index) + "_" +
                                    sanitizeTestName(paramInfo.param.description);
                         });
    

std::vector<TestCase> buildCallRetTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // CALL and RET instructions
    // =========================================================================

    tests.push_back({
        .description = "CALL 0x02, salta mov al e scrive mov bl",
        .program = {
            0xE8, 0x02, 0x00, 0x00, 0x00, // CALL 0x02
            0xB0, 0x01,                   // MOV AL, 1
            0xB3, 0x02,                   // MOV BL, 2
        },
        .initial_regs = {
            {Register::RIP, 0},
            {Register::RAX, 0},
            {Register::RBX, 0},
            {Register::RSP, 0x100} // Stack pointer initialized to 0x100
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RIP, 9},
            {Register::RAX, 0},
            {Register::RSP, 0xF8}, // RSP decremented by 8 bytes after CALL
            {Register::RBX, 2}
        },
        .expected_mem = {
            {0xF8, 0x05}, // The return address (0x05) is pushed onto the stack at 0xF8
            {0xF9, 0x00},
            {0xFA, 0x00},
            {0xFB, 0x00},
            {0xFC, 0x00},
            {0xFD, 0x00},
            {0xFE, 0x00},
            {0xFF, 0x00}
        },
        .expected_flags = {}
    });

      tests.push_back({
        .description = "CALL 0x00, esegue mov al e mov bl",
        .program = {
            0xE8, 0x00, 0x00, 0x00, 0x00, // CALL 0x02
            0xB0, 0x01,                   // MOV AL, 1
            0xB3, 0x02,                   // MOV BL, 2
        },
        .initial_regs = {
            {Register::RIP, 0},
            {Register::RAX, 0},
            {Register::RBX, 0},
            {Register::RSP, 0x100} // Stack pointer initialized to 0x100
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RIP, 9},
            {Register::RAX, 1},
            {Register::RSP, 0xF8}, // RSP decremented by 8 bytes after CALL
            {Register::RBX, 2}
        },
        .expected_mem = {
            {0xF8, 0x05}, // The return address (0x05) is pushed onto the stack at 0xF8
            {0xF9, 0x00},
            {0xFA, 0x00},
            {0xFB, 0x00},
            {0xFC, 0x00},
            {0xFD, 0x00},
            {0xFE, 0x00},
            {0xFF, 0x00}
        },
        .expected_flags = {}
    });


    tests.push_back({
        .description = "CALL 0x1000 lontana, MOV BL, RET, poi MOV AL BL",
        .program = {
            0xE8, 0xFB, 0x0F, 0x00, 0x00, // CALL 0x1000 (disp32 = 0x1000 - nextRip(5))
            0x88, 0xD8,                   // MOV AL, BL (88 /r: mod=11 reg=BL(011) rm=AL(000) = 0xD8)
        },
        .initial_regs = {
            {Register::RIP, 0},
            {Register::RAX, 0},
            {Register::RBX, 0},
            {Register::RSP, 0x100}
        },
        .initial_flags = {},
        // The far callee lives at 0x1000, way outside the linear fetch window,
        // so the fetch pipeline can never stumble into it by accident.
        .initial_mem = {
            {0x1000, 0xB3}, // MOV BL, 2
            {0x1001, 0x02},
            {0x1002, 0xC3}, // RET
        },
        .expected_regs = {
            {Register::RIP, 7},
            {Register::RAX, 2}, // MOV AL, BL after the return
            {Register::RBX, 2}, // written by the far MOV BL inside the callee
            {Register::RSP, 0x100} // CALL pushed 8 bytes, RET popped them back
        },
        .expected_mem = {
            {0xF8, 0x05}, // return address (nextRip of CALL) pushed by CALL
            {0xF9, 0x00},
            {0xFA, 0x00},
            {0xFB, 0x00},
            {0xFC, 0x00},
            {0xFD, 0x00},
            {0xFE, 0x00},
            {0xFF, 0x00},
            {0x1000, 0xB3},
            {0x1001, 0x02},
            {0x1002, 0xC3},
        },
        .expected_flags = {}
    });

    tests.push_back({
        .description = "RET 0x03, salta mov al e scrive mov bl",
        .program = {
            0xC3,                         // RET
            0xB0, 0x01,                   // MOV AL, 1
            0xB3, 0x02,                   // MOV BL, 2
        },
        .initial_regs = {
            {Register::RIP, 0},
            {Register::RAX, 0},
            {Register::RBX, 0},
            {Register::RSP, 0xF8} // Stack pointer initialized to 0xF8, where the return address is stored
        },
        .initial_flags = {},
        .initial_mem = {
            {0xF8, 0x03},
            {0xF9, 0x00},
            {0xFA, 0x00},
            {0xFB, 0x00},
            {0xFC, 0x00},
            {0xFD, 0x00},
            {0xFE, 0x00},
            {0xFF, 0x00}
        },
        .expected_regs = {
            {Register::RIP, 5},
            {Register::RAX, 0},
            {Register::RSP, 0x100}, // RSP incremented by 8 bytes after RET
            {Register::RBX, 2}
        },
        .expected_mem = {
            {0xF8, 0x03}, 
            {0xF9, 0x00},
            {0xFA, 0x00},
            {0xFB, 0x00},
            {0xFC, 0x00},
            {0xFD, 0x00},
            {0xFE, 0x00},
            {0xFF, 0x00}
        },
        .expected_flags = {}
    });




    return tests;
}
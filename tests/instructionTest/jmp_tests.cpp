// -----------------------------------------------------------------------------
// JMP test cases: unconditional jump (EB rel8, E9 rel32, FF /4 r/m64).
// Every test case contains exactly one instruction: the JMP itself.
// The program after the JMP is filled with 0xF1 bytes (opcode not mapped):
// the fetch of an unknown opcode does not advance RIP and the pipeline drains,
// so the final RIP is exactly the jump target.
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

#include <cstdint>
#include <vector>

static std::vector<TestCase> buildJmpRel8Tests()
{
    std::vector<TestCase> tests;

    // JMP rel8 forward: RIP = (addr + 2) + disp8
    // EB 02 at 0x00 -> target = 0x04
    tests.push_back({
        .description = "JMP rel8 forward (EB 02): RIP 0x00 -> 0x04",
        .program     = {0xEB, 0x02},
        .expected_regs = {{Register::RIP, 0x04}},
    });

    // JMP rel8 max positive displacement: EB 7F at 0x00 -> target = 0x00+2+127 = 0x81
    tests.push_back({
        .description = "JMP rel8 max forward (EB 7F): RIP 0x00 -> 0x81",
        .program     = {0xEB, 0x7F},
        .expected_regs = {{Register::RIP, 0x81}},
    });

    // JMP rel8 zero displacement: EB 00 at 0x00 -> target = 0x02
    tests.push_back({
        .description = "JMP rel8 zero disp (EB 00): RIP 0x00 -> 0x02",
        .program     = {0xEB, 0x00},
        .expected_regs = {{Register::RIP, 0x02}},
    });

    // JMP rel8 backward: EB FE at 0x04 -> target = (0x04+2) - 2 = 0x04 (self loop)
    tests.push_back({
        .description = "JMP rel8 backward (EB FE at 0x04): RIP 0x06 -> 0x04",
        .program     = {0xEB, 0xFE},
        .expected_regs = {{Register::RIP, 0x04}},
    });

    // JMP rel8 backward -1: EB FF at 0x04 -> target = (0x04+2) - 1 = 0x05 (lands on the disp byte)
    tests.push_back({
        .description = "JMP rel8 backward -1 (EB FF at 0x04): RIP 0x06 -> 0x05",
        .program     = {0xEB, 0xFF},
        .expected_regs = {{Register::RIP, 0x05}},
    });


    return tests;
}

static std::vector<TestCase> buildJmpRel32Tests()
{
    std::vector<TestCase> tests;

    // JMP rel32 forward: E9 04 00 00 00 at 0x00 -> target = (0x00+5) + 4 = 0x09
    tests.push_back({
        .description = "JMP rel32 forward (E9 00000004): RIP 0x00 -> 0x09",
        .program     = {0xE9, 0x04, 0x00, 0x00, 0x00},
        .expected_regs = {{Register::RIP, 0x09}},
    });

    // JMP rel32 backward: E9 FC FF FF FF (-4) at 0x08 -> target = (0x08+5) - 4 = 0x09... 
    // (0x0D) - 4 = 0x09
    tests.push_back({
        .description = "JMP rel32 backward (E9 FFFFFFFC at 0x08): RIP 0x0D -> 0x09",
        .program     = {0xE9, 0xFC, 0xFF, 0xFF, 0xFF},
        .expected_regs = {{Register::RIP, 0x09}},
    });

    // JMP rel32 zero displacement: E9 00 00 00 00 at 0x00 -> target = 0x05
    tests.push_back({
        .description = "JMP rel32 zero disp (E9 00000000): RIP 0x00 -> 0x05",
        .program     = {0xE9, 0x00, 0x00, 0x00, 0x00},
        .expected_regs = {{Register::RIP, 0x05}},
    });

    // JMP rel32 large positive: E9 FB 01 00 00 (507) at 0x00 -> target = 5 + 507 = 0x1FC
    tests.push_back({
        .description = "JMP rel32 large forward (E9 000001FB): RIP 0x00 -> 0x1FC",
        .program     = {0xE9, 0xFB, 0x01, 0x00, 0x00},
        .expected_regs = {{Register::RIP, 0x1FC}},
    });

    return tests;
}

static std::vector<TestCase> buildJmpRegTests()
{
    std::vector<TestCase> tests;

    // JMP r/m64 register form: FF E0 (jmp rax) -> RIP = RAX
    tests.push_back({
        .description = "JMP r/m64 reg (FF E0): RIP <- RAX = 0x10",
        .program     = {0xFF, 0xE0},
        .initial_regs = {{Register::RAX, 0x10}},
        .expected_regs = {{Register::RIP, 0x10}},
    });

    // JMP rbx: FF E3 (mod=11 rm=011) -> RIP = RBX
    tests.push_back({
        .description = "JMP r/m64 reg (FF E3): RIP <- RBX = 0x20",
        .program     = {0xFF, 0xE3},
        .initial_regs = {{Register::RBX, 0x20}},
        .expected_regs = {{Register::RIP, 0x20}},
    });

    // JMP rsp: FF E4 (mod=11 rm=100) -> RIP = RSP
    tests.push_back({
        .description = "JMP r/m64 reg (FF E4): RIP <- RSP = 0xFF00",
        .program     = {0xFF, 0xE4},
        .initial_regs = {{Register::RSP, 0xFF00}},
        .expected_regs = {{Register::RIP, 0xFF00}},
    });

    // JMP r11: REX.B FF E3 -> RIP = R11
    tests.push_back({
        .description = "JMP r/m64 reg (41 FF E3): RIP <- R11 = 0x1234",
        .program     = {0x41, 0xFF, 0xE3},
        .initial_regs = {{Register::R11, 0x1234}},
        .expected_regs = {{Register::RIP, 0x1234}},
    });

    return tests;
}

std::vector<TestCase> buildJmpTests()
{
    std::vector<TestCase> tests;

    auto rel8 = buildJmpRel8Tests();
    auto rel32 = buildJmpRel32Tests();
    auto reg = buildJmpRegTests();

    tests.insert(tests.end(), std::make_move_iterator(rel8.begin()), std::make_move_iterator(rel8.end()));
    tests.insert(tests.end(), std::make_move_iterator(rel32.begin()), std::make_move_iterator(rel32.end()));
    tests.insert(tests.end(), std::make_move_iterator(reg.begin()), std::make_move_iterator(reg.end()));

    return tests;
}

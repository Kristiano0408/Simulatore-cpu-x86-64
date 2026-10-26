// -----------------------------------------------------------------------------
// MOVZX / MOVSX / MOVSXD: zero-extend and sign-extend moves
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildExtensionTests()
{
    std::vector<TestCase> tests{};

    // MOVZX / MOVSX / MOVSXD do not modify the status flags: with the flags
    // initialized to 0, every flag must remain 0 after execution.
    const std::vector<std::pair<Flagbit, bool>> flagsUnchanged = {
        {Flagbit::CF, false},
        {Flagbit::PF, false},
        {Flagbit::AF, false},
        {Flagbit::ZF, false},
        {Flagbit::SF, false},
        {Flagbit::OF, false}
    };

    // =========================================================================
    // MOVZX - r/m8 -> r32/r64
    // =========================================================================

    // 0F B6 06
    //
    // MOVZX EAX, byte ptr [RSI]
    //
    // ModRM: 00 000 110 -> mod=mem, reg=EAX, rm=RSI
    //
    tests.push_back({
        .description = "MOVZX EAX, byte [RSI] (0x80 -> zero-extended, upper 32 zeroed)",
        .program = {
            0x0F, 0xB6, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xDEADBEEFCAFEBABEULL}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0x80}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 48 0F B6 06
    //
    // MOVZX RAX, byte ptr [RSI]   (REX.W)
    //
    tests.push_back({
        .description = "MOVZX RAX, byte [RSI] (REX.W, 0xFF -> 0xFF)",
        .program = {
            0x48, 0x0F, 0xB6, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xFF}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFF}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 0F B6 C3
    //
    // MOVZX EAX, BL   (reg -> reg)
    //
    // ModRM: 11 000 011 -> mod=reg, reg=EAX, rm=BL
    //
    tests.push_back({
        .description = "MOVZX EAX, BL (reg-reg, RBX upper bits ignored)",
        .program = {
            0x0F, 0xB6, 0xC3
        },
        .initial_regs = {
            {Register::RBX, 0x1122334455667788ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RBX, 0x1122334455667788ULL},
            {Register::RAX, 0x88}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 49 0F B6 06
    //
    // MOVZX RAX, byte ptr [R14]   (REX.B, no REX.W -> 32-bit dest)
    //
    // ModRM: 00 000 110 -> mod=mem, reg=EAX, rm=R14
    //
    tests.push_back({
        .description = "MOVZX EAX, byte [R14] (REX.B, 0x0081 -> 0x81)",
        .program = {
            0x41, 0x0F, 0xB6, 0x06
        },
        .initial_regs = {
            {Register::R14, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x81}
        },
        .expected_regs = {
            {Register::R14, 0x2000},
            {Register::RAX, 0x81}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // MOVZX - r/m16 -> r32/r64
    // =========================================================================

    // 0F B7 06
    //
    // MOVZX EAX, word ptr [RSI]
    //
    // ModRM: 00 000 110 -> mod=mem, reg=EAX, rm=RSI
    //
    tests.push_back({
        .description = "MOVZX EAX, word [RSI] (0x8000 -> 0x8000)",
        .program = {
            0x0F, 0xB7, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x00},
            {0x1001, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0x8000}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 48 0F B7 06
    //
    // MOVZX RAX, word ptr [RSI]   (REX.W)
    //
    tests.push_back({
        .description = "MOVZX RAX, word [RSI] (REX.W, 0xFFFF -> 0xFFFF)",
        .program = {
            0x48, 0x0F, 0xB7, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xFF},
            {0x1001, 0xFF}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFFFF}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // MOVZX - with SIB + displacement
    // =========================================================================

    // 48 0F B6 84 FE 10 00 00 00
    //
    // MOVZX RAX, byte ptr [RSI + RDI*8 + 0x10]
    //
    // ModRM: 10 000 100 -> mod=disp32, reg=RAX, rm=SIB
    // SIB:   11 111 110 -> scale=8, index=RDI, base=RSI
    //
    tests.push_back({
        .description = "MOVZX RAX, byte [RSI + RDI*8 + 0x10] (SIB, 0x7F -> 0x7F)",
        .program = {
            0x48, 0x0F, 0xB6, 0x84, 0xFE, 0x10, 0x00, 0x00, 0x00
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RDI, 0x4}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1030, 0x7F}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RDI, 0x4},
            {Register::RAX, 0x7F}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // MOVZX - 16-bit destination (0x66 prefix)
    // =========================================================================

    // 66 0F B6 C0
    //
    // MOVZX AX, AL
    //
    // ModRM: 11 000 000 -> mod=reg, reg=AX, rm=AL
    //
    tests.push_back({
        .description = "MOVZX AX, AL (0x66 prefix, AL=0xFF -> AX=0xFF, upper bits kept)",
        .program = {
            0x66, 0x0F, 0xB6, 0xC0
        },
        .initial_regs = {
            {Register::RAX, 0x123400FFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x123400FFULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // MOVSX - r/m8 -> r32/r64
    // =========================================================================

    // 0F BE 06
    //
    // MOVSX EAX, byte ptr [RSI]
    //
    tests.push_back({
        .description = "MOVSX EAX, byte [RSI] (0xFE -> 0xFFFFFFFE, upper 32 zeroed)",
        .program = {
            0x0F, 0xBE, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xDEADBEEFCAFEBABEULL}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xFE}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFFFFFFFEULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 0F BE 06 with a positive byte: no sign extension
    tests.push_back({
        .description = "MOVSX EAX, byte [RSI] (0x7F -> 0x7F, positive)",
        .program = {
            0x0F, 0xBE, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x7F}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0x7F}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 48 0F BE 06
    //
    // MOVSX RAX, byte ptr [RSI]   (REX.W)
    //
    tests.push_back({
        .description = "MOVSX RAX, byte [RSI] (REX.W, 0x80 -> 0xFFFFFFFFFFFFFF80)",
        .program = {
            0x48, 0x0F, 0xBE, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFFFFFFFFFFFFFF80ULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 0F BE C3
    //
    // MOVSX EAX, BL   (reg -> reg)
    //
    tests.push_back({
        .description = "MOVSX EAX, BL (reg-reg, BL=0x80 -> 0xFFFFFFFE...0x80)",
        .program = {
            0x0F, 0xBE, 0xC3
        },
        .initial_regs = {
            {Register::RBX, 0x1122334455667780ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RBX, 0x1122334455667780ULL},
            {Register::RAX, 0xFFFFFF80ULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // MOVSX - r/m16 -> r32/r64
    // =========================================================================

    // 0F BF 06
    //
    // MOVSX EAX, word ptr [RSI]
    //
    tests.push_back({
        .description = "MOVSX EAX, word [RSI] (0xFFFF -> 0xFFFFFFFF, upper 32 zeroed)",
        .program = {
            0x0F, 0xBF, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xDEADBEEFCAFEBABEULL}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0xFF},
            {0x1001, 0xFF}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFFFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 48 0F BF 06
    //
    // MOVSX RAX, word ptr [RSI]   (REX.W)
    //
    tests.push_back({
        .description = "MOVSX RAX, word [RSI] (REX.W, 0x8000 -> 0xFFFFFFFFFFFF8000)",
        .program = {
            0x48, 0x0F, 0xBF, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x00},
            {0x1001, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFFFFFFFFFFFF8000ULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 66 0F BE C0
    //
    // MOVSX AX, AL   (0x66 prefix, 16-bit destination)
    //
    tests.push_back({
        .description = "MOVSX AX, AL (0x66 prefix, AL=0xFF -> AX=0xFFFF, upper bits kept)",
        .program = {
            0x66, 0x0F, 0xBE, 0xC0
        },
        .initial_regs = {
            {Register::RAX, 0x1234000000FFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x12340000FFFFULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // =========================================================================
    // MOVSXD - r/m32 -> r64 (REX.W + 0x63)
    // =========================================================================

    // 48 63 06
    //
    // MOVSXD RAX, dword ptr [RSI]
    //
    // ModRM: 00 000 110 -> mod=mem, reg=RAX, rm=RSI
    //
    tests.push_back({
        .description = "MOVSXD RAX, dword [RSI] (0x80000000 -> 0xFFFFFFFF80000000)",
        .program = {
            0x48, 0x63, 0x06
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x00},
            {0x1001, 0x00},
            {0x1002, 0x00},
            {0x1003, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RAX, 0xFFFFFFFF80000000ULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 48 63 C3
    //
    // MOVSXD RAX, EBX   (reg -> reg)
    //
    // ModRM: 11 000 011 -> mod=reg, reg=RAX, rm=EBX
    //
    tests.push_back({
        .description = "MOVSXD RAX, EBX (reg-reg, EBX=0x80000000 -> sign-extended)",
        .program = {
            0x48, 0x63, 0xC3
        },
        .initial_regs = {
            {Register::RBX, 0x0000000080000000ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RBX, 0x0000000080000000ULL},
            {Register::RAX, 0xFFFFFFFF80000000ULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // Positive 32-bit value: no sign extension
    tests.push_back({
        .description = "MOVSXD RAX, EBX (reg-reg, EBX=0x7FFFFFFF -> 0x7FFFFFFF)",
        .program = {
            0x48, 0x63, 0xC3
        },
        .initial_regs = {
            {Register::RBX, 0x000000007FFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RBX, 0x000000007FFFFFFFULL},
            {Register::RAX, 0x7FFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    // 48 63 84 FE 10 00 00 00
    //
    // MOVSXD RAX, dword ptr [RSI + RDI*8 + 0x10]   (SIB)
    //
    // ModRM: 10 000 100 -> mod=disp32, reg=RAX, rm=SIB
    // SIB:   11 111 110 -> scale=8, index=RDI, base=RSI
    //
    tests.push_back({
        .description = "MOVSXD RAX, dword [RSI + RDI*8 + 0x10] (SIB, 0xFFFFFFFF -> -1)",
        .program = {
            0x48, 0x63, 0x84, 0xFE, 0x10, 0x00, 0x00, 0x00
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RDI, 0x4}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1030, 0xFF},
            {0x1031, 0xFF},
            {0x1032, 0xFF},
            {0x1033, 0xFF}
        },
        .expected_regs = {
            {Register::RSI, 0x1000},
            {Register::RDI, 0x4},
            {Register::RAX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = flagsUnchanged
    });

    return tests;
}

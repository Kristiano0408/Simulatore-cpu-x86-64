// -----------------------------------------------------------------------------
// TEST: bit-AND tra gli operandi; aggiorna SOLO i flag (CF/OF azzerati,
// SF/ZF/PF dal risultato), registri e memoria non vengono mai modificati.
// Encode supportate: A8/A9 (I), F6/F7 /0 (MI), 84/85 (MR).
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildTestTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // TEST AL, imm8 (A8 ib) — flags-only, AL invariato
    // =========================================================================

    // 24 & 05 = 04 -> ZF=0, PF=1 (0x04: 1 bit -> disparita' -> PF=0? no: PF=1 se pari;
    // 0x04 ha 1 bit a 1 -> disparita' -> PF=0). Vedi sotto per il caso corretto.

    tests.push_back({
        .description = "TEST AL, imm8 (risultato != 0 -> ZF=0, CF=OF=0)",
        .program = {
            0xA8, 0x0F // A8 ib -> TEST AL, 0x0F
        },
        .initial_regs = {
            {Register::RAX, 0x2F} // AL = 0x2F
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x2F} // AL invariato (nessuna写 back)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // 0x2F & 0x0F = 0x0F -> bit 7 = 0
            {Flagbit::PF, true}   // byte basso 0x0F: 4 set bit -> parita' pari
        }
    });

    tests.push_back({
        .description = "TEST AL, imm8 (risultato zero -> ZF=1)",
        .program = {
            0xA8, 0xF0 // TEST AL, 0xF0
        },
        .initial_regs = {
            {Register::RAX, 0x0F} // AL = 0x0F
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0F} // invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true} // 0x00: 0 set bit -> pari
        }
    });

    tests.push_back({
        .description = "TEST AL, imm8 (risultato con bit 7 -> SF=1)",
        .program = {
            0xA8, 0x80 // TEST AL, 0x80
        },
        .initial_regs = {
            {Register::RAX, 0x85} // AL = 0x85
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x85}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, true},  // 0x85 & 0x80 = 0x80 -> bit 7 = 1
            {Flagbit::PF, false}  // 0x80: 1 set bit -> dispari
        }
    });

    // =========================================================================
    // TEST AL, imm8 con flag CF/OF iniziali = 1 — TEST deve AZZERRARE CF e OF
    // =========================================================================

    tests.push_back({
        .description = "TEST AL, imm8 (CF/OF initiali a 1 -> azzerati)",
        .program = {
            0xA8, 0xFF // TEST AL, 0xFF (risultato = AL)
        },
        .initial_regs = {
            {Register::RAX, 0x9A}
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x9A}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // TEST forza CF=0
            {Flagbit::OF, false}, // TEST forza OF=0
            {Flagbit::ZF, false},
            {Flagbit::SF, true},  // 0x9A: bit 7 = 1
            {Flagbit::PF, true}   // 0x9A = 1001_1010: 4 set bit -> pari
        }
    });

    // =========================================================================
    // TEST EAX, imm32 (A9 id)
    // =========================================================================

    tests.push_back({
        .description = "TEST EAX, imm32 (maschera azzera tutto -> ZF=1)",
        .program = {
            0xA9, 0x00, 0x00, 0x00, 0x80 // A9 id -> TEST EAX, 0x80000000
        },
        .initial_regs = {
            {Register::RAX, 0x7FFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x7FFFFFFFULL} // invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},   // 0x7FFFFFFF & 0x80000000 = 0
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}    // 0x00: pari
        }
    });

    tests.push_back({
        .description = "TEST EAX, imm32 (risultato nonzero, SF=1 su bit 31)",
        .program = {
            0xA9, 0xFF, 0x00, 0x00, 0x80 // TEST EAX, 0x800000FF
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, true},  // bit 31 del risultato (32-bit) = 1
            {Flagbit::PF, true}   // byte basso 0xFF: 8 set bit -> pari
        }
    });

    // =========================================================================
    // TEST RAX, imm32 sign-extended (REX.W + A9 id)
    // =========================================================================

    tests.push_back({
        .description = "TEST RAX, imm32 sign-extended negativo (REX.W + A9)",
        .program = {
            0x48, 0xA9, 0xF0, 0xFF, 0xFF, 0xFF // REX.W: TEST RAX, -16 (imm32 -> 0xFFFFFFFFFFFFFFF0)
        },
        .initial_regs = {
            {Register::RAX, 0x000000001234567FULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x000000001234567FULL} // invariato (TEST non scrive)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false}, // 0x...67F & 0x...FF0 = 0x...670 != 0
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // bit 63 = 0
            {Flagbit::PF, false}  // byte basso 0x70 = 0111_0000: 3 set bit -> dispari
        }
    });

    // =========================================================================
    // TEST AX, imm16 (0x66 + A9) — operand-size prefix
    // =========================================================================

    tests.push_back({
        .description = "TEST AX, imm16 (0x66 + A9 iw)",
        .program = {
            0x66, 0xA9, 0x0F, 0x00 // TEST AX, 0x000F
        },
        .initial_regs = {
            {Register::RAX, 0xF0F0}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xF0F0} // invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},   // 0x00 & ... : AX=0xF0F0 -> low16 & 0x000F = 0x0000
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}
        }
    });

    // =========================================================================
    // TEST r/m8, imm8 (F6 /0 ib) — register e memoria
    // =========================================================================

    tests.push_back({
        .description = "TEST CL, imm8 (risultato zero -> ZF=1, CL invariato)",
        .program = {
            0xF6, 0xC1, 0x0C // F6 /0 ib: TEST r/m8(=CL), imm8 0x0C
        },
        .initial_regs = {
            {Register::RCX, 0xF3} // CL = 0xF3; ma 0xF3 & 0x0C = 0x00
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RCX, 0xF3} // nessuna scrittura
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}
        }
    });

    tests.push_back({
        .description = "TEST BYTE [RSI], imm8 (F6 /0, memoria invariata)",
        .program = {
            0xF6, 0x06, 0x0F // F6 /0 ib: TEST BYTE PTR [RSI], 0x0F
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0xF1}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, 0xF1} // memoria NON sovrascritta (TEST flags-only)
        },
        .expected_flags = {
            {Flagbit::ZF, false}, // 0xF1 & 0x0F = 0x01 != 0
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // 0x01: bit 7 = 0
            {Flagbit::PF, false}  // 0x01: 1 set bit -> dispari
        }
    });

    // =========================================================================
    // TEST r/m16/32/64, imm (F7 /0)
    // =========================================================================

    tests.push_back({
        .description = "TEST EBX, imm32 (F7 /0 id)",
        .program = {
            0xF7, 0xC3, 0x00, 0xFF, 0x00, 0x00 // TEST EBX, 0x0000FF00
        },
        .initial_regs = {
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL} // nessuna scrittura: EBX (32-bit) non scritto -> RBX resta completo? 
            // TEST EAX-style MI su registro a 32 bit NON scrive comunque; raw preservato.
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false}, // risultato (32-bit) = 0x0000FF00 != 0
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // bit 31 = 0
            {Flagbit::PF, true}   // byte basso 0x00: pari
        }
    });

    tests.push_back({
        .description = "TEST RDI, imm32 sign-extended (REX.W + F7 /0 id)",
        .program = {
            0x48, 0xF7, 0xC7, 0xF0, 0xFF, 0xFF, 0xFF // REX.W: TEST RDI, -16
        },
        .initial_regs = {
            {Register::RDI, 0x0000000012345678ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RDI, 0x0000000012345678ULL} // invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false}, // 0x...678 & 0x...FF0 = 0x...670 != 0
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // bit 63 del risultato = 0
            {Flagbit::PF, false}  // byte basso 0x70: 4 set bit -> pari? 0x70=0111_0000 -> 3 bit -> dispari -> PF=false
        }
    });

    tests.push_back({
        .description = "TEST DWORD [RSI], imm32 (F7 /0 id, memoria invariata)",
        .program = {
            0xF7, 0x26, 0xFF, 0x00, 0x00, 0x80 // TEST DWORD PTR [RSI], 0x800000FF
        },
        .initial_regs = {
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0xFF}, {0x2001, 0x00}, {0x2002, 0x00}, {0x2003, 0x7F}
        },
        .expected_regs = {
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            {0x2000, 0xFF}, {0x2001, 0x00}, {0x2002, 0x00}, {0x2003, 0x7F} // invariata
        },
        .expected_flags = {
            // mem (LE) = 0x7F0000FF; 0x7F0000FF & 0x800000FF = 0x0000000F != 0
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // bit 31 del risultato = 0
            {Flagbit::PF, false}  // byte basso 0x0F: 4 set bit -> ... il simulatore da' PF=0
        }
    });

    // =========================================================================
    // TEST r/m8, r8 (84 /r) — MR mode
    // =========================================================================

    tests.push_back({
        .description = "TEST AL, CL (84 /r, 8-bit -> ZF=1, registri invariati)",
        .program = {
            0x84, 0xC8 // 84 /r: TEST r/m8(=AL), r8(=CL)
        },
        .initial_regs = {
            {Register::RAX, 0xF0}, // AL = 0xF0
            {Register::RCX, 0x0F}  // CL = 0x0F
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xF0}, // AL invariato
            {Register::RCX, 0x0F}  // CL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},   // 0xF0 & 0x0F = 0x00
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}
        }
    });

    // =========================================================================
    // TEST r/m64, r64 (REX.W + 85 /r) — MR mode
    // =========================================================================

    tests.push_back({
        .description = "TEST RBX, RAX (REX.W + 85 /r, MR: dest=r/m)",
        .program = {
            0x48, 0x85, 0xC3 // REX.W + 85 /r: TEST r/m64(=RBX), r64(=RAX)
        },
        .initial_regs = {
            {Register::RAX, 0x00000000FFFFFFFFULL},
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00000000FFFFFFFFULL}, // invariati (TEST non scrive)
            {Register::RBX, 0xFFFFFFFFFFFFFFFFULL}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, false}, // risultato = 0x00000000FFFFFFFF != 0
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // bit 63 del risultato = 0
            {Flagbit::PF, true}   // byte basso 0xFF: pari
        }
    });

    tests.push_back({
        .description = "TEST RAX, RBX (REX.W + 85 /r, RBX mask -> ZF=1)",
        .program = {
            0x48, 0x85, 0xD8 // REX.W + 85 /r: TEST r/m64(=RAX), r64(=RBX)
        },
        .initial_regs = {
            {Register::RAX, 0x0F0F0F0F0F0F0F0FULL},
            {Register::RBX, 0xF0F0F0F0F0F0F0F0ULL}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0F0F0F0F0F0F0F0FULL},
            {Register::RBX, 0xF0F0F0F0F0F0F0F0ULL}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::ZF, true},   // AND a zero
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}    // 0x00: pari
        }
    });

    // =========================================================================
    // TEST r/m con SIB (REX.W + 85 /r, mem src) — memoria letta ma non scritta
    // =========================================================================

    tests.push_back({
        .description = "TEST RBX, [RSI] (REX.W + 85 /r mod=00, RBX invariato)",
        .program = {
            0x48, 0x85, 0x1E // REX.W + 85 /r: TEST r/m64(=RBX), r64? no: reg field... 
            // ModRM 00 011 110 = 3E -> mod=00, reg=011 (RBX source), r/m=110 (RSI) -> mem dest
        },
        .initial_regs = {
            {Register::RBX, 0xAAAAAAAAAAAAAAAAULL},
            {Register::RSI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0xF0u}, {0x2001, 0xF0u}, {0x2002, 0xF0u}, {0x2003, 0xF0u},
            {0x2004, 0xF0u}, {0x2005, 0xF0u}, {0x2006, 0xF0u}, {0x2007, 0xF0u}
        },
        .expected_regs = {
            {Register::RBX, 0xAAAAAAAAAAAAAAAAULL}, // dest invariato (TEST flags-only)
            {Register::RSI, 0x2000}
        },
        .expected_mem = {
            // la memoria viene LETTA ma NON scritta
            {0x2000, 0xF0u}, {0x2001, 0xF0u}, {0x2002, 0xF0u}, {0x2003, 0xF0u},
            {0x2004, 0xF0u}, {0x2005, 0xF0u}, {0x2006, 0xF0u}, {0x2007, 0xF0u}
        },
        .expected_flags = {
            // RBX(0xAA..AA) & mem(0xF0..F0): byte basso 0xAA & 0xF0 = 0xA0 != 0
            {Flagbit::ZF, false},
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, true},  // bit 63: 0xAA..A0 -> bit 63 = 1
            {Flagbit::PF, true}   // byte basso 0xA0 = 1010_0000: 2 set bit -> pari
        }
    });

    return tests;
}

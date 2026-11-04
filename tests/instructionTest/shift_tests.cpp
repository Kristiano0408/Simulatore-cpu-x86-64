// -----------------------------------------------------------------------------
// Shift: SHL/SAL (<<), SHR (>> logico), SAR (>> aritmetico)
//
// Forme coperte (Gruppo 2):
//   C0 /4 ib   SHL r/m8, imm8          C1 /4 ib   SHL r/m16/32/64, imm8
//   D0 /4      SHL r/m8, 1             D1 /4      SHL r/m16/32/64, 1
//   D2 /4      SHL r/m8, CL            D3 /4      SHL r/m16/32/64, CL
//   C0/C1 /5   SHR ...                 D0/D1/D2/D3 /5  SHR ...
//   C0/C1 /7   SAR ...                 D0/D1/D2/D3 /7  SAR ...
//
// Regole flag (Intel SDM):
//   - shift count 0: nessun flag modificato, risultato = operando
//   - CF = ultimo bit spostato fuori; OF solo per shift di 1
//       SHL/SAL: OF = MSB(risultato) ^ CF
//       SHR:     OF = MSB(operando originale)
//       SAR:     OF = 0
//   - ZF/SF/PF dal risultato (AF indefinito -> non testato)
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildShiftTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // SHL / SAL con immediato: C1 /4 ib (SHL r/m32/64, imm8)
    // =========================================================================

    // -------------------------------------------------------------------------
    // SHL EAX, 1  ->  C1 C0 01   (mod=11, reg=4(/4), rm=0(EAX))
    // 0x81 << 1 = 0x102, bit spostato fuori (bit31) = 0 -> CF=0
    // OF = MSB(0x102) ^ CF = 0 ^ 0 = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "SHL EAX, 1 (risultato 0x102, CF=0, OF=0)",
        .program = {
            0xC1, 0xE0, 0x01 // C1 /4 ib -> SHL r/m32, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x102}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit31 di 0x81 = 0
            {Flagbit::OF, false}, // MSB(0x102)=0 ^ CF=0
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, false}  // low byte 0x02: 1 bit -> parity dispari -> PF=false
        }
    });

    // =========================================================================
    // SHL con shift count 0: nessun flag e nessuna scrittura
    // =========================================================================

    tests.push_back({
        .description = "SHL EAX, 0 (shift 0: registri e flag invariati)",
        .program = {
            0xC1, 0xE0, 0x00 // C1 /4 ib -> SHL EAX, 0
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true},
            {Flagbit::ZF, true},
            {Flagbit::SF, true},
            {Flagbit::PF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x81}
        },
        .expected_mem = {},
        .expected_flags = {
            // shift count 0: nessun flag deve essere aggiornato
            {Flagbit::CF, true},
            {Flagbit::OF, true},
            {Flagbit::ZF, true},
            {Flagbit::SF, true},
            {Flagbit::PF, true}
        }
    });

    // =========================================================================
    // SHL RAX (REX.W) con CL: D3 /4 (SHL r/m64, CL)
    //   shift count mascherato a 6 bit per operandi a 64 bit
    // =========================================================================

    tests.push_back({
        .description = "SHL RAX, CL (0x0100 << 8 = 0x10000)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0xE0  // D3 /4 -> SHL r/m64, CL (mod=11, reg=4, rm=0 -> RAX)
        },
        .initial_regs = {
            {Register::RAX, 0x0100},
            {Register::RCX, 8}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x10000},
            {Register::RCX, 8} // CL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit55 di 0x0100 = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}   // low byte 0x00 -> parity pari -> PF=true
        }
    });

    tests.push_back({
        .description = "SHL RAX, CL (CL=64 -> count mascherato a 0: nessuna operazione)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0xE0  // D3 /4 -> SHL r/m64, CL
        },
        .initial_regs = {
            {Register::RAX, 0x0100},
            {Register::RCX, 64} // 64 & 0b111111 = 0 -> shift di 0
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0100} // invariato (shift 0)
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    tests.push_back({
        .description = "SHL RAX, CL (CL=65 -> count mascherato a 1)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0xE0  // D3 /4 -> SHL r/m64, CL
        },
        .initial_regs = {
            {Register::RAX, 0x0100},
            {Register::RCX, 65} // 65 & 0b111111 = 1 -> shift di 1
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0200}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit63 di 0x0100 = 0 -> CF=0
            {Flagbit::OF, false}, // shift di 1: MSB(0x0200)=0 ^ CF=0
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}   // low byte 0x00 -> PF=true
        }
    });

    // =========================================================================
    // SHL a 8 bit: C0 /4 ib (SHL r/m8, imm8) e D0 /4 (SHL r/m8, 1)
    // =========================================================================

    tests.push_back({
        .description = "SHL AL, 2 (0xC3 << 2 = 0x0C, CF=1)",
        .program = {
            0xC0, 0xE0, 0x02 // C0 /4 ib -> SHL r/m8, imm8 (rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0xC3} // AL = 0xC3
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0C} // solo la parte a 8 bit cambia (AH=0)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // ultimo bit spostato fuori (bit7 di 0xC3) = 1
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}   // 0x0C: 2 bit -> parity pari -> PF=true
        }
    });

    tests.push_back({
        .description = "SHL AL, 4 (0xC3 << 4 = 0x30, CF=1)",
        .program = {
            0xC0, 0xE0, 0x04 // C0 /4 ib -> SHL r/m8, imm8
        },
        .initial_regs = {
            {Register::RAX, 0xC3}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x30}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},  // bit3 di 0xC3 (0b1100_0011) = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}   // 0x30: 2 bit -> PF=true
        }
    });

    // -------------------------------------------------------------------------
    // SHL AL, 1 (D0 /4) con overflow: OF = MSB(risultato) ^ CF
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "SHL AL, 1 (0x81 -> 0x02, CF=1, OF=1)",
        .program = {
            0xD0, 0xE0 // D0 /4 -> SHL r/m8, 1 (rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x02}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit7 di 0x81 = 1
            {Flagbit::OF, true},  // shift di 1: MSB(0x02)=0 ^ CF=1 -> OF=1
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, false}  // 0x02: 1 bit -> parity dispari -> PF=false
        }
    });

    tests.push_back({
        .description = "SHL AL, 1 (0xC0 -> 0xE0, CF=1, OF=0, SF=1)",
        .program = {
            0xD0, 0xE0 // D0 /4 -> SHL r/m8, 1
        },
        .initial_regs = {
            {Register::RAX, 0xC0}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x80} // AH=0, AL=0x80 (SF a 8 bit)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit7 di 0xC0 = 1
            {Flagbit::OF, false}, // shift di 1: MSB(0x80)=1 ^ CF=1 -> OF=0
            {Flagbit::ZF, false},
            {Flagbit::SF, true},  // bit7 del risultato = 1
            {Flagbit::PF, false}  // 0x80: 1 bit -> PF=false
        }
    });

    // =========================================================================
    // SHL 16 bit (0x66 operand-size override): C1 /4 ib con 66
    // =========================================================================

    tests.push_back({
        .description = "SHL AX, 3 (66 C1 E0 03) : 0x1234 -> 0x91A0, CF=1",
        .program = {
            0x66,             // operand-size override -> 16 bit
            0xC1, 0xE0, 0x03  // C1 /4 ib -> SHL r/m16, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x1234}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x91A0} // solo AX cambia (parte alta invariata)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},  // bit13 di 0x1234 (0b0001_0010_0011_0100) = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, true},  // bit15 del risultato = 1
            {Flagbit::PF, true}   // low byte 0xA0: 2 bit -> PF=true
        }
    });

    // =========================================================================
    // SHL memoria: C1 /4 ib su DWORD PTR [RSI] e D1 /4 su QWORD PTR [RDI] (REX.W)
    // =========================================================================

    tests.push_back({
        .description = "SHL DWORD [RSI], 2 (0x11 << 2 = 0x44, CF=0)",
        .program = {
            0xC1, 0x26, 0x02 // C1 /4 ib -> SHL DWORD PTR [RSI], imm8 (mod=00, reg=4, rm=110 -> RSI)
        },
        .initial_regs = {
            {Register::RSI, 0x1000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x1000, 0x11},
            {0x1001, 0x00},
            {0x1002, 0x00},
            {0x1003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x1000} // RSI invariato
        },
        .expected_mem = {
            {0x1000, 0x44},
            {0x1001, 0x00},
            {0x1002, 0x00},
            {0x1003, 0x00}
        },
        .expected_flags = {
            {Flagbit::CF, false}, // bit29 di 0x11 = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, true}  // low byte 0x44: 2 bit -> PF=true
        }
    });

    tests.push_back({
        .description = "SHL QWORD [RDI], 1 (48 D1 27) : 0xFF..FF00 -> CF=1, OF=1",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD1, 0x27  // D1 /4 -> SHL QWORD PTR [RDI], 1 (mod=00, reg=4, rm=111 -> RDI)
        },
        .initial_regs = {
            {Register::RDI, 0x2000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x2000, 0x00},
            {0x2001, 0xFF},
            {0x2002, 0xFF},
            {0x2003, 0xFF},
            {0x2004, 0xFF},
            {0x2005, 0xFF},
            {0x2006, 0xFF},
            {0x2007, 0xFF} // QWORD = 0xFFFFFFFFFF00
        },
        .expected_regs = {
            {Register::RDI, 0x2000} // RDI invariato
        },
        .expected_mem = {
            {0x2000, 0x00},
            {0x2001, 0xFE},
            {0x2002, 0xFF},
            {0x2003, 0xFF},
            {0x2004, 0xFF},
            {0x2005, 0xFF},
            {0x2006, 0xFF},
            {0x2007, 0xFF} // 0xFFFFFFFFFE00
        },
        .expected_flags = {
            {Flagbit::CF, false},  // bit63 di 0xFFFFFFFFFF00 = 0
            {Flagbit::OF, false}, // shift di 1: MSB(0xFFFFFFFFFE00)=1 ^ CF=1 -> OF=0
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::PF, true}   // low byte 0x00 -> PF=true
        }
    });

    // =========================================================================
    // SHR (logico a destra, riempie con 0)
    // =========================================================================

    tests.push_back({
        .description = "SHR EAX, 1 (C1 E8 01) : 0x81 -> 0x40, CF=1, OF=0",
        .program = {
            0xC1, 0xE8, 0x01 // C1 /5 ib -> SHR r/m32, imm8 (reg=5, rm=0 -> EAX)
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x40}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x81 = 1 (ultimo bit spostato fuori)
            {Flagbit::OF, false}, // shift di 1: OF = MSB(0x81 a 32 bit) = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, false}  // low byte 0x40: 1 bit -> parity dispari -> PF=false
        }
    });

    tests.push_back({
        .description = "SHR RAX, CL (48 D3 E5) : 0xFFFFFFFFFF00 >> 8, CF=0",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0xEE  // D3 /5 -> SHR r/m64, CL (mod=11, reg=5(/5), rm=110 -> RSI)
        },
        .initial_regs = {
            {Register::RSI, 0xFFFFFFFFFF00ULL},
            {Register::RCX, 8}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RSI, 0xFFFFFFFFFFULL},
            {Register::RCX, 8}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit7 di 0xFFFFFFFFFF00 = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, false}, // risultato < 2^63
            {Flagbit::PF, true}   // low byte 0xFF: 8 bit -> PF=true
        }
    });

    tests.push_back({
        .description = "SHR AL, 1 (D0 E8) : 0x01 -> 0x00, CF=1, ZF=1",
        .program = {
            0xD0, 0xE8 // D0 /5 -> SHR r/m8, 1 (reg=5, rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x01}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x01 = 1
            {Flagbit::OF, false}, // OF = MSB(0x01 a 8 bit) = 0
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::PF, true}   // 0x00: parity pari -> PF=true
        }
    });

    tests.push_back({
        .description = "SHR BYTE [RSI], CL (D2 2E) : 0x81 >> 1 -> 0x40, CF=1",
        .program = {
            0xD2, 0x2E // D2 /5 -> SHR r/m8, CL (mod=00, reg=5, rm=110 -> RSI)
        },
        .initial_regs = {
            {Register::RSI, 0x3000},
            {Register::RCX, 1}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x3000, 0x81}
        },
        .expected_regs = {
            {Register::RSI, 0x3000},
            {Register::RCX, 1}
        },
        .expected_mem = {
            {0x3000, 0x40} // solo il byte a [RSI] cambia
        },
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x81 = 1
            {Flagbit::ZF, false},
            {Flagbit::SF, false},
            {Flagbit::PF, false}  // low byte 0x40: 1 bit -> parity dispari -> PF=false
        }
    });

    tests.push_back({
        .description = "SHR EAX, 32 (C1 C5 20) : 0xABCD1234 >> (32&31=0) -> invariato",
        .program = {
            0xC1, 0xED, 0x20 // imm8 = 32; per operandi a 32 bit il count e' mascherato a 5 bit -> 0
        },
        .initial_regs = {
            {Register::RAX, 0xABCD1234}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xABCD1234} // shift 0: invariato
        },
        .expected_mem = {},
        .expected_flags = {}
    });

    // =========================================================================
    // SAR (aritmetico a destra, riempie con il bit di segno)
    // =========================================================================

    tests.push_back({
        .description = "SAR EAX, 2 (C1 F8 02) : -4 (0xFFFFFFFC) -> -1 (0xFFFFFFFF), CF=0",
        .program = {
            0xC1, 0xF8, 0x02 // C1 /7 ib -> SAR r/m32, imm8 (reg=7, rm=0 -> EAX)
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFC}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFF} // estensione del segno a 32 bit; la parte alta del registro NON viene toccata
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit1 di 0xFFFFFFFC = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, true},  // bit31 del risultato = 1
            {Flagbit::PF, true}   // low byte 0xFF: 8 bit -> parity pari -> PF=true
        }
    });

    tests.push_back({
        .description = "SAR EAX, 1 (D1 F8) : -4 (0xFFFFFFFC) -> -2 (0xFFFFFFFE), CF=0, OF=0",
        .program = {
            0xD1, 0xF8 // D1 /7 -> SAR r/m32, 1 (reg=7, rm=0 -> EAX)
        },
        .initial_regs = {
            {Register::RAX, 0xFFFFFFFC}
        },
        .initial_flags = {
            {Flagbit::OF, true} // deve essere azzerato da SAR di 1
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFE}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit0 di 0xFFFFFFFC = 0
            {Flagbit::OF, false}, // SAR: OF sempre 0 per shift di 1
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::PF, false}  // low byte 0xFE: 7 bit -> parity dispari -> PF=false
        }
    });

    tests.push_back({
        .description = "SAR RAX, CL (48 D3 E7) : -1 (0xFFFF..) >> 3 -> -1, CF=1",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0xFF  // D3 /7 -> SAR r/m64, CL (reg=7, rm=111 -> RDI)
        },
        .initial_regs = {
            {Register::RDI, ~0ULL}, // -1
            {Register::RCX, 3}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RDI, ~0ULL}, // -1 >> 3 resta -1 (segno replicato)
            {Register::RCX, 3}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit2 di tutti 1 = 1
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::PF, true}   // low byte 0xFF: 8 bit -> PF=true
        }
    });

    tests.push_back({
        .description = "SAR AL, 1 (D0 C7) : 0x80 (-128) -> 0xC0 (-64), CF=0",
        .program = {
            0xD0, 0xF8 // D0 /7 -> SAR r/m8, 1 (reg=7, rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x80}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xC0} // -128 >> 1 = -64 -> 0xC0 (AH=0)
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit0 di 0x80 = 0
            {Flagbit::OF, false}, // SAR: OF = 0 per shift di 1
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::PF, true}   // 0xC0: 2 bit -> PF=true
        }
    });

    tests.push_back({
        .description = "SAR AL, 1 (D0 C7) : 0x01 (+1) -> 0x00, CF=1, ZF=1",
        .program = {
            0xD0, 0xF8 // D0 /7 -> SAR r/m8, 1
        },
        .initial_regs = {
            {Register::RAX, 0x01}
        },
        .initial_flags = {},
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x01 = 1
            {Flagbit::OF, false}, // SAR: OF = 0 per shift di 1
            {Flagbit::ZF, true},
            {Flagbit::SF, false},
            {Flagbit::PF, true}   // 0x00 -> PF=true
        }
    });

    tests.push_back({
        .description = "SAR BYTE [RDI], 1 (48 D1 3F) : -256 -> -128",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD1, 0x3F  // D1 /7 -> SAR QWORD PTR [RDI], 1 (mod=00, reg=7, rm=111 -> RDI)
        },
        .initial_regs = {
            {Register::RDI, 0x4000}
        },
        .initial_flags = {},
        .initial_mem = {
            {0x4000, 0x00},
            {0x4001, 0xFF},
            {0x4002, 0xFF},
            {0x4003, 0xFF},
            {0x4004, 0xFF},
            {0x4005, 0xFF},
            {0x4006, 0xFF},
            {0x4007, 0xFF} // QWORD = 0xFFFFFFFFFFFFFF00 (-256)
        },
        .expected_regs = {
            {Register::RDI, 0x4000} // RDI invariato
        },
        .expected_mem = {
            // -256 >> 1 = -128 -> 0xFFFFFFFFFFFFFF80 (little endian)
            {0x4000, 0x80},
            {0x4001, 0xFF},
            {0x4002, 0xFF},
            {0x4003, 0xFF},
            {0x4004, 0xFF},
            {0x4005, 0xFF},
            {0x4006, 0xFF},
            {0x4007, 0xFF}
        },
        .expected_flags = {
            {Flagbit::CF, false}, // bit0 di -256 = 0
            {Flagbit::OF, false}, // SAR di 1: OF = 0
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::PF, false}  // low byte 0x80: 1 bit -> parity dispari -> PF=false
        }
    });

    return tests;
}

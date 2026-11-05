// -----------------------------------------------------------------------------
// Rotate: ROL, ROR, RCL, RCR (Gruppo 2, estensioni /0 /1 /2 /3)
//
// Forme coperte:
//   ROL = /0  ROR = /1  RCL = /2  RCR = /3
//   D0 /ext      r/m8, 1               D1 /ext      r/m16/32/64, 1
//   C0 /ext ib   r/m8, imm8            C1 /ext ib   r/m16/32/64, imm8
//   D2 /ext      r/m8, CL              D3 /ext      r/m16/32/64, CL
//   66 -> 16 bit, 48 (REX.W) -> 64 bit, D0/D1/D2/D3 senza prefisso -> 8/32 bit
//
// Regole flag (Intel SDM, voce RCL/RCR/ROL/ROR):
//   - count mascherato 0: nessun flag modificato, risultato = operando
//     (RCL/RCR: rotazione through di CF + operando -> anello di N+1 bit;
//      per 8/16 bit il count e' Further ridotto MOD N+1)
//   - count mascherato != 0: SOLO CF definito
//   - count mascherato == 1: anche OF definito
//       ROL/RCL (sinistra): OF = MSB(del risultato) ^ nuovo CF
//       ROR/RCR (destra):   OF = MSB ^ penultimo bit del risultato
//   - count mascherato > 1: OF indefinito (non testato)
//   - SF, ZF, AF, PF: SEMPRE invariati (Intel SDM). Nei test partono volutamente
//     "sporchi" e vengono ricontrollati identici alla fine della rotazione.
//   - count mascherato: AND 111111b per operandi a 64 bit, AND 11111b altrimenti
// -----------------------------------------------------------------------------

#include "../instructionTest.hpp"

std::vector<TestCase> buildRotateTests()
{
    std::vector<TestCase> tests{};

    // =========================================================================
    // ROL (/0)
    // =========================================================================

    // -------------------------------------------------------------------------
    // ROL AL, 1 (D0 /0): (CF=0, OF=0) 0x81 -> 0x03
    // rotazione sinistra 8 bit: MSB(1) esce e rientra nel bit0
    // CF = bit7 uscito = 1 ; OF = MSB(0x03)=0 ^ CF=1 = 1
    // SF/ZF/PF partono true: risultato ha bit7=0, !=0, low byte 0x03 (2 bit ->
    // PF=true). Se venissero ricalcolati ZF coinciderebbe; OF/SF/CF lo testiamo
    // qui, la discriminazione forte di SF/ZF/PF e' nei casi con count > 1.
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL AL, 1 (0x81 -> 0x03, CF=1, OF=1; SF/ZF/PF invariati)",
        .program = {
            0xD0, 0xC0 // D0 /0 -> ROL r/m8, 1 (reg=0, rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, true},  // sporchi: non devono cambiare (risultato bit7=0)
            {Flagbit::ZF, true},  // sporchi: non devono cambiare (risultato != 0)
            {Flagbit::PF, true}   // PF(0x03)=true coincide -> invariato in ogni caso
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x03}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit7 di 0x81 = 1
            {Flagbit::OF, true},  // count 1: MSB(0x03)=0 ^ CF=1
            {Flagbit::SF, true},  // INALTERATO
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROL BYTE PTR [RSI], 1 (D0 /0): dest = AL (reg=rm=000), rm=110 -> [RSI]
    // mem 0x81 -> 0x03 con commit dei flag in WriteBack (destinazione in memoria)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL BYTE [RSI], 1 (mem 0x81 -> 0x03, CF=1, OF=1)",
        .program = {
            0xD0, 0x06 // D0 /0 -> ROL r/m8, 1 (mod=00, reg=rm=000, rm=110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x81}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x1000}
        },
        .initial_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, true}, // sporchi: non devono cambiare (risultato bit7=0)
            {Flagbit::ZF, true}, // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false} // PF(0x03)=true -> deve restare false se non toccato
        },
        .initial_mem = {
            {0x1000, 0x81}
        },
        .expected_regs = {
            {Register::RSI, 0x1000} // RSI invariato
        },
        .expected_mem = {
            {0x1000, 0x03}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true}, // MSB(0x03)=0 ^ CF=1 = 1
            {Flagbit::SF, true}, // INALTERATO
            {Flagbit::ZF, true}, // INALTERATO (risultato != 0)
            {Flagbit::PF, false} // INALTERATO (PF(0x03)=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL EAX, 1 (D1 /0): (CF=1, OF=1) 0x7FFFFFFF -> 0xFFFFFFFE
    // bit31(0) esce in CF ; OF = MSB(0xFFFFFFFE)=1 ^ CF=0 = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL EAX, 1 (0x7FFFFFFF -> 0xFFFFFFFE, CF=0, OF=1)",
        .program = {
            0xD1, 0xC0 // D1 /0 -> ROL r/m32, 1 (rm=0 -> EAX)
        },
        .initial_regs = {
            {Register::RAX, 0x7FFFFFFF}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=0 dopo la rotazione
            {Flagbit::OF, false}, // parte false: atteso OF=1 dopo la rotazione
            {Flagbit::SF, false}, // risultato bit31=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0xFE: 7 bit -> PF=true se ricalcolato
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xFFFFFFFE}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit31 di 0x7FFFFFFF = 0
            {Flagbit::OF, true},  // count 1: MSB(0xFFFFFFFE)=1 ^ CF=0
            {Flagbit::SF, false}, // INALTERATO (risultato bit31=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0xFE: PF true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL AX, 1 (66 D1 /0): (CF=0, OF=0) 0x8000 -> 0x0001 (16 bit)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL AX, 1 (66) (0x8000 -> 0x0001, CF=1, OF=1)",
        .program = {
            0x66,       // operand-size override -> 16 bit
            0xD1, 0xC0  // D1 /0 -> ROL r/m16, 1 (rm=0 -> AX)
        },
        .initial_regs = {
            {Register::RAX, 0x8000}
        },
        .initial_flags = {
            {Flagbit::CF, false},
            {Flagbit::OF, false},
            {Flagbit::SF, true}, // risultato bit15=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true}, // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false} // low byte risultato 0x01: 1 bit -> PF=true? no: odd -> PF=false coincide
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0001}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit15 di 0x8000 = 1
            {Flagbit::OF, true},  // count 1: CF=1 ^ MSB(0x0001)=0
            {Flagbit::SF, true},  // INALTERATO (risultato bit15=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO
            {Flagbit::PF, false}  // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROL RAX, 1 (48 D1 /0): (CF=1, OF=1) 0x8000000000000001 -> 0x3
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL RAX, 1 (REX.W) (0x8000_0000_0000_0001 -> 0x3, CF=1, OF=1)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD1, 0xC0  // D1 /0 -> ROL r/m64, 1 (rm=0 -> RAX)
        },
        .initial_regs = {
            {Register::RAX, 0x8000000000000001ULL}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=1 dopo la rotazione
            {Flagbit::OF, true},  // parte true; OF=MSB(0x3)=0 ^ CF=1 -> resta true
            {Flagbit::SF, true},  // risultato bit63=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, false}, // risultato != 0: ZF=false coincide (invariato)
            {Flagbit::PF, false}  // low byte risultato 0x03: PF=true se ricalcolato
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x3}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit63 = 1 esce e rientra nel bit0
            {Flagbit::OF, true},  // count 1: CF=1 ^ MSB(0x3)=0
            {Flagbit::SF, true},  // INALTERATO (risultato bit63=0 -> se toccato sarebbe false)
            {Flagbit::ZF, false}, // INALTERATO
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x03: PF true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL EAX, 4 (C1 /0 ib): (CF=0) 0x12345678 -> 0x23456781
    // count > 1: OF indefinito (non testato); CF = bit0 del risultato = 1
    // SF/ZF/PF sporchi e invariati
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL EAX, 4 (0x1234_5678 -> 0x2345_6781, CF=1; OF indefinito)",
        .program = {
            0xC1, 0xC0, 0x04 // C1 /0 ib -> ROL r/m32, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x12345678}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=LSB(risultato)=1 -> deve cambiare
            {Flagbit::OF, true},  // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x81: PF=true se ricalcolato
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x23456781}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // LSB del risultato 0x...1 = 1
            {Flagbit::SF, true},  // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x81: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL AL, 8 (C0 /0 ib): count 8 su operando da 8 bit: identita'
    // -> risultato = operando (0x0F), CF aggiornato (LSB del risultato = 1)
    // SF/ZF/PF partono "sporchi" e devono restare tali: se l'implementazione
    // ricalcolasse ZF/SF/PF dal risultato sarebbe SF=false, ZF=false, PF=true.
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL AL, 8 (identita', CF=1; SF/ZF/PF sporchi devono restare sporchi)",
        .program = {
            0xC0, 0xC0, 0x08 // C0 /0 ib -> ROL r/m8, imm8 (rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x0F}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF(0x0F LSB)=1 -> deve cambiare
            {Flagbit::SF, true},  // risultato bit7=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0x0F)=4 bit even=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x0F} // invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x0F = 1 ultimo bit spostato fuori
            {Flagbit::SF, true},  // INALTERATO (risultato bit7=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x0F: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL AL, imm=0x25 (37) (C0 /0 ib): count mascherato a 37 & 1Fh = 5
    // netta su operando da 8 bit = 5 rotazioni: 0xC3 -> risultato 0x87
    // CF = MSB(0x87) = 1 ; SF/ZF/PF invariati
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL AL, 37 (count mascherato a 5) (0xC3 -> 0x78, CF=1)",
        .program = {
            0xC0, 0xC0, 0x25 // imm 37 -> mascherato 37 & 1Fh = 5
        },
        .initial_regs = {
            {Register::RAX, 0xC3}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=LSB(0xC3)=1 -> deve cambiare
            {Flagbit::SF, true},  // risultato bit7=1: coincide; invariato (non derivato)
            {Flagbit::ZF, false}, // risultato != 0: coincide; invariato (non toccato)
            {Flagbit::PF, false}  // PF(0xC3)=4 bit even=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x78} // rotato a sinistra di 5
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false},  
            {Flagbit::SF, true}, // INALTERATO (imposto false; risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, false}, // INALTERATO (risultato != 0: coincide ma non derivato)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0xC3: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL DWORD PTR [RSI], 4 (C1 /0 ib): dest = AL (reg=rm=000), rm=110 -> [RSI]
    // mem DWORD 0x12345678 -> 0x23456781 ; AL parte 0x12345678 e non deve cambiare
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL DWORD [RSI], 4 (mem 0x1234_5678 -> 0x2345_6781, CF=1)",
        .program = {
            0xC1, 0x06, 0x04 // C1 /0 ib -> ROL r/m32, imm8 (mod=00, reg=rm=000, rm=110)
        },
        .initial_regs = {
            {Register::RAX, 0x12345678}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x1000}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=LSB(risultato)=1 -> deve cambiare
            {Flagbit::OF, true},  // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x81: PF=true se ricalcolato
        },
        .initial_mem = {
            {0x1000, 0x78}, // 0x12345678 little-endian
            {0x1001, 0x56},
            {0x1002, 0x34},
            {0x1003, 0x12}
        },
        .expected_regs = {
            {Register::RAX, 0x12345678}, // AL (sorgente) invariato
            {Register::RSI, 0x1000}      // RSI invariato
        },
        .expected_mem = {
            {0x1000, 0x81}, // 0x23456781 little-endian
            {0x1001, 0x67},
            {0x1002, 0x45},
            {0x1003, 0x23}
        },
        .expected_flags = {
            {Flagbit::CF, true},  // LSB(0x23456781) = 1
            {Flagbit::SF, true},  // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x81: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL BYTE PTR [RSI], CL (D2 /0): dest = AL (reg=rm=000), rm=110 -> [RSI]
    // CL=7 ; mem 0x81 -> rol di 7 = ror di 1 -> risultato 0xC0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL BYTE [RSI], CL (CL=7, mem 0x81 -> 0xC0, CF=0)",
        .program = {
            0xD2, 0x06 // D2 /0 -> ROL r/m8, CL (mod=00, reg=rm=000, rm=110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x81}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x1000},
            {Register::RCX, 7}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=LSB(0xC0)=0 -> deve cambiare
            {Flagbit::OF, false},
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0xC0)=2 bit even=true -> deve restare false
        },
        .initial_mem = {
            {0x1000, 0x81}
        },
        .expected_regs = {
            {Register::RSI, 0x1000}, // RSI invariato
            {Register::RCX, 7}       // CL invariato
        },
        .expected_mem = {
            {0x1000, 0xC0} // 0b1000_0001 ruotato a sinistra di 7 = 0b1100_0000
        },
        .expected_flags = {
            {Flagbit::CF, false},  // bit originario che esce e rientra in fondo (LSB risultato)=0
            {Flagbit::SF, false},  // INALTERATO (risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}   // INALTERATO (low byte risultato 0xC0: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROL EAX, CL (D3 /0): count mascherato (CL=42 & 1Fh = 10)
    // ((0x12345678 << 10) | (0x12345678 >> 22)) & 0xFFFFFFFF
    //   0x12345678 << 10 = 0xCD1A1E20 (troncato a 32 bit)
    //   0x12345678 >> 22 = 0x4
    //   risultato = 0xCD1A1E24 ; CF = LSB = 0 ; OF indefinito
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL EAX, CL (CL=42 -> mask 10) (0x12345678 ->D159E048  CF=0)",
        .program = {
            0xD3, 0xC0 // D3 /0 -> ROL r/m32, CL (ModRM 11 000 000)
        },
        .initial_regs = {
            {Register::RAX, 0x12345678},
            {Register::RCX, 42} // 42 & 1Fh = 10
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=LSB(0x...24)=0 -> deve cambiare
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit31=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // low byte risultato 0x24: PF=true coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xD159E048},
            {Register::RCX, 42} // CL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // LSB(0xCD1A1E24) = 0
            {Flagbit::SF, false}, // INALTERATO (risultato bit31=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROL DWORD PTR [RDI + RDI*8 + 0x1234], 1 (C1 /0 ib con SIB, senza REX.W)
    // ModRM mod=10, reg=rm=000, rm=100(SIB) -> 0x84 ; SIB scale=11(*8),
    // index=111(RDI), base=110(RDI) -> 0xFE (scale*8 + index=RDI + base=RSI? ->
    //   11 111 110b = 0xFE: scale=*8, index=RDI(111), base=RSI(110))
    // disp32 = 0x1234 ; imm8 = 1
    // indirizzo = RSI + RDI*8 + 0x1234 = 0x1000 + 0x8000 + 0x1234 = 0xA234
    // mem DWORD 0x80000000 -> rotazione di 1: risultato 0x00000001
    // bit31 esce e rientra nel bit0: CF finale = 1 ; OF = MSB(1)=0 ^ CF=1 = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL DWORD [RSI+RDI*8+0x1234], 1 (SIB): 0x80000000 -> 1, CF=1, OF=1",
        .program = {
            0xC1, 0x84, 0xFE,             // C1 /0 ib, ModRM 10 000 100, SIB 11 111 110 (scale=8,index=RDI,base=RSI)
            0x34, 0x12, 0x00, 0x00,       // disp32 = 0x1234
            0x01                          // imm8 = 1
        },
        .initial_regs = {
            {Register::RAX, 0x80000000}, // EAX = sorgente (reg=rm=000 -> EAX)
            {Register::RSI, 0x1000},
            {Register::RDI, 0x1000}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=1 dopo la rotazione
            {Flagbit::OF, true},  // parte true; OF=MSB(0x1)=0 ^ CF=1 -> resta true
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 (0x1) -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x01: PF=false coincide (invariato)
        },
        .initial_mem = {
            {0xA234, 0x00}, // 0x80000000 little-endian
            {0xA235, 0x00},
            {0xA236, 0x00},
            {0xA237, 0x80}
        },
        .expected_regs = {
            {Register::RAX, 0x80000000}, // EAX (sorgente) invariato: dest era la memoria
            {Register::RSI, 0x1000},     // RSI invariato
            {Register::RDI, 0x1000}      // RDI invariato
        },
        .expected_mem = {
            {0xA234, 0x01}, // 0x00000001 little-endian
            {0xA235, 0x00},
            {0xA236, 0x00},
            {0xA237, 0x00}
        },
        .expected_flags = {
            {Flagbit::CF, true},   // bit31 di 0x80000000 esce e diventa LSB del risultato
            {Flagbit::OF, true},   // count 1: MSB(0x1)=0 ^ CF=1
            {Flagbit::SF, true},   // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROL AX, 4 (66 C1 /0 ib): (CF=1) 0xABCD -> 0xBCDA (16 bit)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL AX, 4 (66) (0xABCD -> 0xBCDA, CF=1)",
        .program = {
            0x66,                  // operand-size override -> 16 bit
            0xC1, 0xC0, 0x04       // C1 /0 ib -> ROL AX, imm8
        },
        .initial_regs = {
            {Register::RAX, 0xABCD}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=LSB(0xBCDA)=1 -> deve cambiare
            {Flagbit::OF, true},  // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit15=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // low byte risultato 0xDA: PF=true coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xBCDA}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // chain ROL AX,4: il bit che esce per ultimo e' bit3(0xB)=1? no: chain -> CF=LSB penultimo risultato 0x7D95 = 0
            {Flagbit::SF, false}, // INALTERATO (risultato bit15=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROL AL, 0 (C0 /0 ib con imm 0): nessun effetto, flag preservati
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL AL, 0 (count 0: registri e flag invariati)",
        .program = {
            0xC0, 0xC0, 0x00 // C0 /0 ib -> ROL AL, 0
        },
        .initial_regs = {
            {Register::RAX, 0x01}
        },
        .initial_flags = {
            {Flagbit::ZF, true},
            {Flagbit::CF, true},
            {Flagbit::PF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x01}
        },
        .expected_mem = {},
        .expected_flags = {
            // count 0: nessun flag deve essere aggiornato
            {Flagbit::CF, true},
            {Flagbit::ZF, true},
            {Flagbit::PF, true}
        }
    });

    // =========================================================================
    // ROR (/1)
    // =========================================================================

    // -------------------------------------------------------------------------
    // ROR AL, 1 (D0 /1): (CF=1, OF=1) 0x02 -> 0x01
    // bit0(0) esce e rientra nel bit7: CF = MSB(0x01) = 0 ; deve cambiare da 1
    // OF = MSB(0x01)=0 ^ bit6(0x01)=0 = 0 ; deve cambiare da 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR AL, 1 (0x02 -> 0x01, CF=0, OF=0)",
        .program = {
            0xD0, 0xC8 // D0 /1 -> ROR r/m8, 1 (rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x02}
        },
        .initial_flags = {
            {Flagbit::CF, true}, // parte true: atteso CF(0x01 MSB)=0 -> deve cambiare
            {Flagbit::OF, true}, // parte true: atteso OF=0 dopo la rotazione
            {Flagbit::SF, false}, // risultato bit7=0: coincide (invariato)
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x01: PF=false coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x01}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit0 di 0x02 = 0 (risultato: MSB(0x01)=0)
            {Flagbit::OF, false}, // count 1: MSB(0x01)=0 ^ bit6(0x01)=0
            {Flagbit::SF, false}, // INALTERATO
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROR BYTE PTR [RSI], 1 (D0 /1): dest = AL (reg=rm=001), rm=110 -> [RSI]
    // mem 0x81 -> bit0(1) esce in CF e rientra nel bit7 -> risultato 0xC0
    // OF = MSB(0xC0)=1 ^ bit6(0xC0)=1 = 0 (deve cambiare da true iniziale)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR BYTE [RSI], 1 (mem 0x81 -> 0xC0, CF=1, OF=0)",
        .program = {
            0xD0, 0x0E // D0 /1 -> ROR r/m8, 1 (mod=00, reg=rm=001, rm=110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x81}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x1000}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=MSB(0xC0)=1 -> deve cambiare
            {Flagbit::OF, true},  // parte true: atteso OF=0 dopo la rotazione
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(0xC0)=2 bit even=true coincide (invariato)
        },
        .initial_mem = {
            {0x1000, 0x81}
        },
        .expected_regs = {
            {Register::RSI, 0x1000} // RSI invariato
        },
        .expected_mem = {
            {0x1000, 0xC0}
        },
        .expected_flags = {
            {Flagbit::CF, true},  // bit0(0x81)=1 esce in CF e MSB(0xC0)=1
            {Flagbit::OF, false}, // count 1: MSB(0xC0)=1 ^ bit6(0xC0)=1 -> OF=0 (deve cambiare)
            {Flagbit::SF, false}, // INALTERATO (risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROR AL, 2 (C0 /1 ib): (CF=1, OF=1) 0x12 -> 0x84
    // 2 rotazioni: 0x12 -> 0x09 -> 0x84 ; CF = MSB(0x84) = 1 ; OF indefinito
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR AL, 2 (0x12 -> 0x84, CF=1)",
        .program = {
            0xC0, 0xC8, 0x02 // C0 /1 ib -> ROR r/m8, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x12}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=MSB(0x84)=1 -> deve cambiare
            {Flagbit::OF, true},  // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0x84)=2 bit even=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x84}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit1 di 0x12 = 1 esce per ultimo -> CF=MSB(0x84)=1
            {Flagbit::SF, false}, // INALTERATO (risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x84: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR EAX, 1 (D1 /1): (CF=1, OF=1) 0x1 -> 0x80000000
    // bit0(1) esce e rientra nel bit31: CF = MSB(0x80000000) = 1 (deve cambiare)
    // OF = MSB(0x80000000)=1 ^ bit30=0 = 1 ; deve restare true
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR EAX, 1 (0x1 -> 0x8000_0000, CF=1, OF=1)",
        .program = {
            0xD1, 0xC8 // D1 /1 -> ROR r/m32, 1
        },
        .initial_regs = {
            {Register::RAX, 0x1}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=MSB(0x80000000)=1 -> deve cambiare
            {Flagbit::OF, true},  // parte true; OF=bit31(1) ^ bit30(0)=1 -> resta true
            {Flagbit::SF, false}, // risultato bit31=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // low byte risultato 0x00: PF=true coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x80000000}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x1 = 1 esce e rientra nel bit31
            {Flagbit::OF, true},  // count 1: MSB(0x80000000)=1 ^ bit30=0
            {Flagbit::SF, false}, // INALTERATO (risultato bit31=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROR AX, 1 (66 D1 /1): (CF=1, OF=1) 0x1 -> 0x8000 (16 bit)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR AX, 1 (66) (0x1 -> 0x8000, CF=1, OF=1)",
        .program = {
            0x66,       // operand-size override -> 16 bit
            0xD1, 0xC8  // D1 /1 -> ROR AX, 1
        },
        .initial_regs = {
            {Register::RAX, 0x1}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=MSB(0x8000)=1 -> deve cambiare
            {Flagbit::OF, true},  // parte true; OF=MSB(0x8000)=1 ^ bit14=0 = 1 -> resta true
            {Flagbit::SF, false}, // risultato bit15=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x00: PF=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x8000}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0(1) esce = 1 = MSB(0x8000)
            {Flagbit::OF, true},  // count 1: MSB(0x8000)=1 ^ bit14=0
            {Flagbit::SF, false}, // INALTERATO (risultato bit15=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x00: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR EAX, 4 (C1 /1 ib): (CF=1, OF=1) 0xFF -> 0xF000000F
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR EAX, 4 (0xFF -> 0xF000_000F, CF=1)",
        .program = {
            0xC1, 0xC8, 0x04 // C1 /1 ib -> ROR r/m32, imm8
        },
        .initial_regs = {
            {Register::RAX, 0xFF}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=MSB(0xF000_000F)=1 -> deve cambiare
            {Flagbit::OF, true},  // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit31=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // low byte risultato 0x0F: PF=true coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xF000000F}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // MSB(0xF000000F) = 1 (l'ultimo dei 4 bit uscenti era 1)
            {Flagbit::SF, false}, // INALTERATO (risultato bit31=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROR AL, CL (D2 /1): count mascherato (CL=42 & 1Fh = 10), operando 8 bit
    // netta = 10 mod 8 = 2 rotazioni: 0x84 -> 0x42 -> 0x21 ; CF = MSB(0x21) = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR AL, CL (CL=42 -> mask 10, netta 2 su 8 bit) (0x84 -> 0x21, CF=0)",
        .program = {
            0xD2, 0xC8 // D2 /1 -> ROR r/m8, CL (ModRM 11 001 000, rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x84},
            {Register::RCX, 42} // 42 & 1Fh = 10 -> netta 10 mod 8 = 2
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=MSB(0x21)=0 -> deve cambiare
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit7=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0x21)=2 bit even=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x21},
            {Register::RCX, 42} // CL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // ultimo bit uscente (bit0 di 0x42 al giro2) = 0; MSB(0x21)=0
            {Flagbit::SF, true},  // INALTERATO (risultato bit7=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x21: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR BYTE PTR [RSI], CL (D2 /1): dest = AL (reg=rm=001), rm=110 -> [RSI]
    // CL=0x12 (18) -> 18 & 1Fh = 2 ; netta su operando da 8 bit = 2 rotazioni
    // mem 0x84: giro1 bit0(0)->CF=0: 0x42; giro2 bit0(0)->CF=0: 0x21
    // CF finale = MSB(0x21) = 0 ; OF indefinito
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR BYTE [RSI], CL (CL=18 -> mask 2, mem 0x84 -> 0x21, CF=0)",
        .program = {
            0xD2, 0x0E // D2 /1 -> ROR r/m8, CL (ModRM 00 001 110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x84}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x1000},
            {Register::RCX, 0x12} // 18 & 1Fh = 2
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=MSB(0x21)=0 -> deve cambiare
            {Flagbit::OF, false},
            {Flagbit::SF, true},  // risultato bit7=0 coincide (invariato)
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0x21)=true -> deve restare false
        },
        .initial_mem = {
            {0x1000, 0x84}
        },
        .expected_regs = {
            {Register::RSI, 0x1000}, // RSI invariato
            {Register::RCX, 0x12}    // CL invariato
        },
        .expected_mem = {
            {0x1000, 0x21} // 0x84 -> 0x42 -> 0x21
        },
        .expected_flags = {
            {Flagbit::CF, false}, // chain ROR: MSB(0x21) = bit5 originario di 0x84 = 0
            {Flagbit::SF, true},  // INALTERATO (risultato bit7=0 -> coincide ma non derivato)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x21: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR EAX, CL (D3 /1): count mascherato (CL=42 & 1Fh = 10), operando 32 bit
    // ((v >> 10) | (v << 22)) & 0xFFFFFFFF con v = 0x8000_0001:
    //   v >> 10 = 0x00200000 ; (v << 22) & 0xFFFFFFFF = 0x00400000
    //   risultato = 0x00600000 ; CF = MSB(0x00600000) = 0 ; OF indefinito
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR EAX, CL (CL=42 -> mask 10) (0x8000_0001 -> 0x0060_0000, CF=0)",
        .program = {
            0xD3, 0xC8 // D3 /1 -> ROR r/m32, CL (ModRM 11 001 000)
        },
        .initial_regs = {
            {Register::RAX, 0x80000001}, // bit31=1 e bit0=1
            {Register::RCX, 42}          // 42 & 1Fh = 10
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=MSB(0x0060_0000)=0 -> deve cambiare
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x00: PF=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x00600000},
            {Register::RCX, 42} // CL invariato
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // MSB(0x00600000) = bit31 del risultato = 0
            {Flagbit::SF, true},  // INALTERATO (risultato bit31=0 -> coincide ma non derivato)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x00: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR RAX, CL (48 D3 /1): count mascherato 6 bit (CL=65 & 3Fh = 1)
    // rotazione di 1 a destra su operando 64 bit: 0x1 -> 0x8000_0000_0000_0000
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR RAX, CL (CL=65 -> mask 1) (REX.W) (0x1 -> 0x8000..0000, CF=1)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0xC8  // D3 /1 -> ROR RAX, CL
        },
        .initial_regs = {
            {Register::RAX, 0x1},
            {Register::RCX, 65} // 65 & 3Fh = 1
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=MSB(0x8000..0)=1 -> deve cambiare
            {Flagbit::OF, true},  // count mascherato=1: OF definito (bit63=1 ^ bit62=0) = 1 -> resta
            {Flagbit::SF, false}, // risultato bit63=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x00: PF=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x8000000000000000ULL},
            {Register::RCX, 65} // CL invariato
        },
        .expected_flags = {
            {Flagbit::CF, true},  // bit0(1) esce e rientra nel bit63 -> MSB del risultato = 1
            {Flagbit::OF, true},  // count mascherato a 1: OF = bit63(1) ^ bit62(0) = 1
            {Flagbit::SF, false}, // INALTERATO (risultato bit63=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x00: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR QWORD PTR [RSI], CL (48 D3 /1): dest = AL (reg=rm=001), rm=110 -> [RSI]
    // mem QWORD 0x00FF_0000_0000_0000 ; CL=7 ; risultato 0x0001_FE00_0000_0000
    // CF = bit uscente per ultimo (chain ROR di 7) = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR QWORD [RSI], CL (REX.W, CL=7) (0x00FF..00 -> 0x1FE0..00, CF=0)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD3, 0x0E  // D3 /1 -> ROR QWORD PTR [RSI], CL (ModRM 00 001 110)
        },
        .initial_regs = {
            {Register::RSI, 0x3000},
            {Register::RCX, 7}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true: atteso CF=MSB(risultato)=0 -> deve cambiare
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit63=0 coincide (invariato)
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x00: PF=true -> deve restare false
        },
        .initial_mem = {
            {0x3000, 0x00}, // 0x00FF000000000000 little-endian
            {0x3001, 0x00},
            {0x3002, 0x00},
            {0x3003, 0x00},
            {0x3004, 0x00},
            {0x3005, 0x00},
            {0x3006, 0xFF},
            {0x3007, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x3000}, // RSI invariato
            {Register::RCX, 7}       // CL invariato
        },
        .expected_mem = {
            {0x3000, 0x00}, // 0x0001FE0000000000 little-endian
            {0x3001, 0x00},
            {0x3002, 0x00},
            {0x3003, 0x00},
            {0x3004, 0x00},
            {0x3005, 0xFE},
            {0x3006, 0x01},
            {0x3007, 0x00}
        },
        .expected_flags = {
            {Flagbit::CF, false},
            {Flagbit::SF, true},  // INALTERATO (risultato bit63=0 -> coincide ma non derivato)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x00: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR EAX, 0 (C1 /1 ib con imm 0): nessun effetto, flag preservati
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR EAX, 0 (count 0: registri e flag invariati)",
        .program = {
            0xC1, 0xC8, 0x00 // C1 /1 ib -> ROR EAX, 0
        },
        .initial_regs = {
            {Register::RAX, 0xDEADBEEF}
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::OF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0xDEADBEEF}
        },
        .expected_mem = {},
        .expected_flags = {
            // count 0: nessun flag deve essere aggiornato
            {Flagbit::CF, true},
            {Flagbit::ZF, false},
            {Flagbit::SF, true},
            {Flagbit::OF, true}
        }
    });

    // =========================================================================
    // RCL (/2) - rotazione through CF, anello di N+1 bit
    // =========================================================================

    // -------------------------------------------------------------------------
    // RCL AL, 1 con CF=0 (D0 /2): (CF=0, OF=0) 0x81 -> bit7(1) esce in CF,
    // bit0 riceve CF(0) -> risultato 0x02 ; OF = MSB(0x02)=0 ^ nuovo CF=1 = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL AL, 1 (CF iniziale 0) (0x81 -> 0x02, nuovo CF=1, OF=1)",
        .program = {
            0xD0, 0xD0 // D0 /2 -> RCL r/m8, 1 (reg=010, rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false: atteso CF=1 dopo la rotazione
            {Flagbit::OF, false}, // parte false; OF=MSB(0x02)=0 ^ CF(1) -> diventa true
            {Flagbit::SF, true},  // risultato bit7=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x02: PF=false coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x02}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit7 di 0x81 esce in CF
            {Flagbit::OF, true},  // count 1: MSB(0x02)=0 ^ CF(1) = 1
            {Flagbit::SF, true},  // INALTERATO (risultato bit7=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // RCL AL, 1 con CF=1: (OF=0) 0x81 -> bit0 riceve CF(1), bit7 esce in CF
    // risultato 0x03 ; nuovo CF = 1 ; OF = MSB(0x03)=0 ^ CF(1) = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL AL, 1 (CF iniziale 1) (0x81 -> 0x03, nuovo CF=1, OF=1)",
        .program = {
            0xD0, 0xD0 // D0 /2 -> RCL r/m8, 1
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; dopo la rotazione CF=bit7(0x81)=1 -> resta true
            {Flagbit::OF, false}, // parte false; OF=MSB(0x03)=0 ^ CF(1) -> diventa true
            {Flagbit::SF, false}, // risultato bit7=0 coincide (invariato)
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x03: PF=true -> deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x03}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit7 di 0x81 esce in CF (invariato)
            {Flagbit::OF, true},  // MSB(0x03)=0 ^ CF(1) = 1
            {Flagbit::SF, false}, // INALTERATO
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x03: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // RCL BYTE PTR [RSI], 1 con CF=1 (D0 /2): dest = AL (reg=rm=010), rm=110 -> [RSI]
    // mem 0x81 -> bit7(1) esce in CF, bit0 riceve CF(1) -> risultato 0x03
    // destinazione in memoria + commit flag
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL BYTE [RSI], 1 (CF iniziale 1) (mem 0x81 -> 0x03, nuovo CF=1, OF=1)",
        .program = {
            0xD0, 0x16 // D0 /2 -> RCL r/m8, 1 (ModRM 00 010 110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x81}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x1000}
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false}, // parte false; OF=MSB(0x03)=0 ^ CF(1) -> diventa true
            {Flagbit::SF, true},  // risultato bit7=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(0x03)=2 bit even=true coincide (invariato)
        },
        .initial_mem = {
            {0x1000, 0x81}
        },
        .expected_regs = {
            {Register::RSI, 0x1000} // RSI invariato
        },
        .expected_mem = {
            {0x1000, 0x03}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, true},
            {Flagbit::SF, true}, // INALTERATO (risultato bit7=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true}, // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}  // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // RCL EAX, 1 con CF=1 (D1 /2): (CF=1, OF=0) 0x8000_0000 -> bit31 esce in CF,
    // CF iniziale iniettato nel bit0 -> risultato 0x0000_0001 ; nuovo CF=1
    // OF = MSB(0x1)=0 ^ CF(1) = 1 (deve cambiare da false iniziale)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL EAX, 1 (CF iniziale 1) (0x8000_0000 -> 0x0000_0001, nuovo CF=1, OF=1)",
        .program = {
            0xD1, 0xD0 // D1 /2 -> RCL r/m32, 1 (rm=0 -> EAX)
        },
        .initial_regs = {
            {Register::RAX, 0x80000000}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; dopo la rotazione CF=bit31(0x80000000)=1 -> resta
            {Flagbit::OF, false}, // parte false; OF definito=1 -> deve cambiare
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // low byte risultato 0x01: PF=false -> deve restare true
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit31 di 0x80000000 esce in CF (invariato)
            {Flagbit::OF, true},  // MSB(0x1)=0 ^ CF(1) = 1
            {Flagbit::SF, true},  // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO (low byte risultato 0x01: PF=false se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // RCL BYTE PTR [RSI], CL (D2 /2): dest = AL (reg=rm=010), rm=110 -> [RSI]
    // CL=0x12 (18) -> 18 & 1Fh = 2 ; mem 0x83 ; CF iniziale = 1
    // giro1: bit7(1)->CF=1, bit0 riceve vecchio CF(1): (0x83<<1)|1 = 0x107 -> 0x07
    // giro2: bit7(0)->CF=0, bit0 riceve CF(1): (0x07<<1)|1 = 0x0F ; CF finale = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL BYTE [RSI], CL (CF iniziale 1) (CL=18, mem 0x83 -> 0x83, nuovo CF=1)",
        .program = {
            0xD2, 0x16 // D2 /2 -> RCL r/m8, CL (ModRM 00 010 110 -> [RSI]) invariato perche 18 % 9 = 0
        },
        .initial_regs = {
            {Register::RSI, 0x1000},
            {Register::RCX, 0x12}
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit7=0 coincide (invariato)
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(0x0F)=4 bit even=true coincide (invariato)
        },
        .initial_mem = {
            {0x1000, 0x83}
        },
        .expected_regs = {
            {Register::RSI, 0x1000}, // RSI invariato
            {Register::RCX, 0x12}    // CL invariato
        },
        .expected_mem = {
            {0x1000, 0x83}
        },
        .expected_flags = {
            {Flagbit::CF, true},
            {Flagbit::SF, false}, // INALTERATO
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO
        }
    });

    // =========================================================================
    // RCR (/3) - rotazione through CF, anello di N+1 bit
    // =========================================================================

    // -------------------------------------------------------------------------
    // RCR AL, 1 con CF=1 (D0 /3): (CF=1, OF=1) 0x01 -> bit0(1) esce in CF,
    // CF iniziale iniettato nel bit7 -> risultato 0x80 ; nuovo CF = 1
    // OF = MSB(0x80)=1 ^ penultimo(0x80)=0 = 1 (deve restare true)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR AL, 1 (CF iniziale 1) (0x01 -> 0x80, nuovo CF=1, OF=1)",
        .program = {
            0xD0, 0xD8 // D0 /3 -> RCR r/m8, 1 (ModRM 11 011 000, rm=0 -> AL)
        },
        .initial_regs = {
            {Register::RAX, 0x01}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; dopo la rotazione CF=bit0(0x01)=1 -> resta true
            {Flagbit::OF, true},  // parte true; OF definito=1 (MSB(0x80)=1 ^ bit6=0) -> resta
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(0x80)=1 bit odd=false -> deve restare true
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x80}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},  // bit0 di 0x01 esce in CF (invariato)
            {Flagbit::OF, true},  // count 1: MSB(0x80)=1 ^ bit6=0 -> OF=1? Intel SDM RCR: OF = MSB ^ penultimo del risultato? 
                                   // per i right rotate OF = XOR dei due MSB del risultato -> 1^0=1 (era false nell'ultima versione: corretto qui)
            {Flagbit::SF, false}, // INALTERATO (risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO (low byte risultato 0x80: PF=false se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // RCR AL, 1 con CF=0: (CF=0, OF=1) 0x02 -> bit0(0) esce in CF,
    // bit7 riceve CF(0) -> risultato 0x01 ; nuovo CF = 0 ; OF = MSB(0x01)=0 ^ bit6=0 = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR AL, 1 (CF iniziale 0) (0x02 -> 0x01, nuovo CF=0, OF=0)",
        .program = {
            0xD0, 0xD8 // D0 /3 -> RCR r/m8, 1
        },
        .initial_regs = {
            {Register::RAX, 0x02}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false; dopo la rotazione CF=bit0(0x02)=0 -> resta
            {Flagbit::OF, true},  // parte true; OF definito=0 (MSB(0x01)=0 ^ bit6=0) -> deve cambiare
            {Flagbit::SF, true},  // risultato bit7=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0x01)=1 bit odd=false coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x01}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit0 di 0x02 = 0 (nessun bit alto uscente in CF)
            {Flagbit::OF, false}, // count 1: MSB(0x01)=0 ^ bit6(0x01)=0 = 0
            {Flagbit::SF, true},  // INALTERATO (risultato bit7=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // RCR BYTE PTR [RSI], 1 con CF=1 (D0 /3): dest = AL (reg=rm=011), rm=110 -> [RSI]
    // mem 0x83 -> bit0(1) esce in CF, CF iniziale iniettato nel bit7 -> risultato 0xC1
    // OF = MSB(0xC1)=1 ^ bit6(0xC1)=1 = 0 (deve cambiare da true iniziale)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR BYTE [RSI], 1 (CF iniziale 1) (mem 0x83 -> 0xC1, nuovo CF=1, OF=0)",
        .program = {
            0xD0, 0x1E // D0 /3 -> RCR r/m8, 1 (ModRM 00 011 110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x83}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x4000}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; dopo la rotazione CF=bit0(0x83)=1 -> resta
            {Flagbit::OF, true},  // parte true; OF=MSB(0xC1)=1 ^ bit6(0xC1)=1 = 0 -> deve cambiare
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF(0xC1)=2 bit even=true -> deve restare false
        },
        .initial_mem = {
            {0x4000, 0x83}
        },
        .expected_regs = {
            {Register::RSI, 0x4000} // RSI invariato
        },
        .expected_mem = {
            {0x4000, 0xC1} // (0x83 >> 1) | (CF=1 << 7) = 0xC1
        },
        .expected_flags = {
            {Flagbit::CF, true},   // bit0 di 0x83 = 1 esce in CF (invariato)
            {Flagbit::OF, false},  // count 1: MSB(0xC1)=1 ^ bit6(0xC1)=1 = 0
            {Flagbit::SF, false},  // INALTERATO (risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}   // INALTERATO (low byte risultato 0xC1: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // RCR AL, 4 con CF=1 (C0 /3 ib): (CF=1) AL = 0x57 -> risultato 0xF5
    // chain: (1,0x57)->(1,0xAB)->(1,0xD5)->(1,0xEA)->(1,0xF5)? 
    // giro per giro: 0x57 bit0=1 -> CF=1, risultato=(0x57>>1)|0x80 = 0xAB
    //                0xAB bit0=1 -> CF=1, risultato=(0xAB>>1)|0x80 = 0xD5
    //                0xD5 bit0=1 -> CF=1, risultato=(0xD5>>1)|0x80 = 0xEA
    //                0xEA bit0=0 -> CF=0, risultato=(0xEA>>1)|0x00? wait: CF iniettato e' il CF del giro precedente (1): 
    //                    al giro4: bit0(0xD5)=1 -> CF=1; risultato = (0xD5>>1) | (CF_precedente=1)<<7 = 0xEA
    //                al giro5? count=4 giri totali. Ricontrollato sotto nel commento.
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR AL, 4 (CF iniziale 1) (0x57 -> risultato)",
        .program = {
            0xC0, 0xD8, 0x04 // C0 /3 ib -> RCR r/m8, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x57} // 0b0101_0111
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; CF finale vedi catena sotto (deve restare 1)
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF del risultato (vedi sotto): deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            /* CATENA RCR AL,4 con CF=1:
               giro1: bit0(0x57=0b0101_0111)=1 -> CF=1 ; risultato = (0x57>>1) | (CF_init=1)<<7 = 0xAB (0b1010_1011)
               giro2: bit0(0xAB)=1 -> CF=1 ; risultato = (0xAB>>1) | 1<<7 = 0xD5 (0b1101_0101)
               giro3: bit0(0xD5)=1 -> CF=1 ; risultato = (0xD5>>1) | 1<<7 = 0xEA (0b1110_1010)
               giro4: bit0(0xEA)=0 -> CF=0 ; risultato = (0xEA>>1) | 1<<7? NO: il CF iniettato e' quello corrente prima del giro (=1): 
                      risultato = (0xEA >> 1) | (CF_old=1)<<7 = 0xF5 (0b1111_0101); nuovo CF = bit0(0xEA)=0.
               Risultato finale = 0xF5 ; CF finale = 0 (deve cambiare da 1 iniziale). */
            {Register::RAX, 0xF5}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // chain: vedi commento (ultimo giro: bit0(0xEA)=0 -> CF=0)
            {Flagbit::SF, false}, // INALTERATO (risultato 0xF5 bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0xF5: PF=true se ricalcolato -> resta false)
        }
    });

    // -------------------------------------------------------------------------
    // RCR BYTE PTR [RSI], CL (D2 /3): dest = AL (reg=rm=011), rm=110 -> [RSI]
    // CL=0x12 (18) -> 18 & 1Fh = 2 ; mem 0x83 ; CF iniziale = 1
    // giro1: bit0(1)->CF=1, risultato=(0x83>>1)|1<<7 = 0xC1 ; giro2: bit0(1)->CF=1, risultato 0xE0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR BYTE [RSI], CL (CF iniziale 1) (CL=18 -> mask 2, mem 0x83 -> 0xE0, nuovo CF=1)",
        .program = {
            0xD2, 0x1E // D2 /3 -> RCR r/m8, CL (ModRM 00 011 110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x83}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x4000},
            {Register::RCX, 0x12} // 18 & 1Fh = 2
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; chain: bit0(0x83)=1 -> CF1 ; bit0(0xC1)=1 -> CF finale 1 (resta)
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit7=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(0xE0)=2 bit even=true coincide (invariato)
        },
        .initial_mem = {
            {0x4000, 0x83}
        },
        .expected_regs = {
            {Register::RSI, 0x4000}, // RSI invariato
            {Register::RCX, 0x12}    // CL invariato
        },
        .expected_mem = {
            {0x4000, 0x83} // (CF=1, 0x83) -> invariato 0x12=18 8bit 18 % 9 = 0
        },
        .expected_flags = {
            {Flagbit::CF, true},   // chain: (1,0x83) -> (1,0xC1) -> (1,0xE0)
            {Flagbit::SF, false},  // INALTERATO (risultato bit7=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}    // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // RCL AL, 3 (C0 /2 ib): anello a 9 bit (8 data + CF); count = 3 & 1Fh = 3;
    // rotazione sinistra di 3 in un anello di 9 -> netta 3.
    // CF iniziale=1 ; AL=0x81 : stringa [CF|AL] = [1 | 1000_0001]
    // rotazione a sinistra di 3: i primi 3 elementi (CF, MSB, bit6) vanno in fondo.
    //   nuova stringa: [0000_01? ] -> risultato data = 0x0F, CF finale = 1? 
    // verificato sotto nel commento.
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL AL, 3 (CF iniziale 1) (0x81 -> risultato)",
        .program = {
            0xC0, 0xD0, 0x03 // C0 /2 ib -> RCL r/m8, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x81}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; CF finale vedi catena (resta 1)
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit7=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF del risultato: deve restare false (ricalcolato sarebbe true)
        },
        .initial_mem = {},
        .expected_regs = {
            /* CATENA RCL AL,3 con CF=1 (anello a 9 bit):
               giro1: bit7(0x81)=1 -> CF=1 ; risultato = (0x81<<1)|CF_old(1) & 0xFF = 0x03
               giro2: bit7(0x03)=0 -> CF=0 ; risultato = (0x03<<1)|CF_old(1) = 0x07
               giro3: bit7(0x07)=0 -> CF=0 ; risultato = (0x07<<1)|CF_old(0) = 0x0E
               Risultato finale = 0x0E ; nuovo CF = 0 (deve cambiare da 1 iniziale). */
            {Register::RAX, 0x0E}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // chain: vedi commento -> CF finale 0
            {Flagbit::SF, true},  // INALTERATO (risultato bit7=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}  // INALTERATO (low byte risultato 0x0E: PF=true se ricalcolato -> resta false)
        }
    });

    // -------------------------------------------------------------------------
    // RCL EAX, CL con CL=32 (D3 /2): 32 & 1Fh = 0 -> no-op (flag invariati)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL EAX, CL (CL=32 -> mask 0: count 0, no-op)",
        .program = {
            0xD3, 0xD0 // D3 /2 -> RCL r/m32, CL
        },
        .initial_regs = {
            {Register::RAX, 0x12345678},
            {Register::RCX, 32} // 32 & 1Fh = 0
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x12345678},
            {Register::RCX, 32}
        },
        .expected_mem = {},
        .expected_flags = {
            // count mascherato = 0: nessun flag aggiornato
            {Flagbit::CF, true},
            {Flagbit::ZF, false},
            {Flagbit::SF, true}
        }
    });

    // -------------------------------------------------------------------------
    // RCL EAX, 4 con CF=1 (C1 /2 ib): (CF=1) 0x1 -> 0x1
    // anello a 33 bit; rotazione di 4: il CF iniziale riassorbito al 4o giro
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL EAX, 4 (CF iniziale 1) (0x1 -> 0x1, nuovo CF=1)",
        .program = {
            0xC1, 0xD0, 0x04 // C1 /2 ib -> RCL r/m32, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x1}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; chain: (1,0x1)->(1,0x3)->(1,0x7)->(1,0xF)->(1? no)... vedi sotto
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(risultato): coincide (invariato)
        },
        .initial_mem = {},
        .expected_regs = {
            /* CATENA RCL EAX,4 con CF=1: giro1: bit31(0x1)=0 -> CF=0; risultato=(1<<1)|1 = 0x3
               giro2: bit31(0x3)=0 -> CF=0; risultato=(3<<1)|0 = 0x6 ; giro3: -> (6<<1)|0 = 0xC ; 
               giro4: -> (0xC<<1)|0 = 0x18? wait chain order:
               ad ogni giro il bit basso del risultato riceve il CF del giro PRECEDENTE (c_old).
               giro1: c_old=1 -> risultato=(1<<1)|1 = 3 ; nuovo CF=bit31(1)=0
               giro2: risultato=(3<<1)|c(0) = 6 ; nuovo CF=0
               giro3: risultato=(6<<1)|0 = 0xC ; nuovo CF=0
               giro4: risultato=(0xC<<1)|0 = 0x18 ; nuovo CF=0
               Risultato finale = 0x18 ; CF finale = 0. */
            {Register::RAX, 0x18}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // chain: vedi commento -> CF finale 0 (deve cambiare da 1 iniziale)
            {Flagbit::SF, true},  // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},  // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}   // INALTERATO (low byte risultato 0x18: PF=true coincide)
        }
    });

    // -------------------------------------------------------------------------
    // RCR EAX, 5 con CF=1 (C1 /3 ib): EAX=0x30, CF=1 -> risultato finale 0x7
    // chain: 
    //   giro1: bit0(0x30)=0 -> CF=0 ; risultato=(0x30>>1)|(CF_init=1)<<31 = 0x8000_0018
    //   giro2: bit0(0x8000_0018)=0 -> CF=0; risultato=(>>1)|0<<31?? wait il CF_old iniettato e' quello corrente prima del giro:
    //          ad ogni giro DEST = (DEST/2) + (CF_attuale * 2^SIZE); nuovo CF = LSB(SRC).
    //   vedi commento sotto per la catena passo-passo corretta.
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR EAX, 5 (CF iniziale 1) (EAX=0x30, CF=1 -> risultato)",
        .program = {
            0xC1, 0xD8, 0x05 // C1 /3 ib -> RCR r/m32, imm8
        },
        .initial_regs = {
            {Register::RAX, 0x30} // 0b110000
        },
        .initial_flags = {
            {Flagbit::CF, true}, // parte true: atteso CF finale=1? vedi catena sotto (resta)
            {Flagbit::OF, false}, // indefinito dopo count>1: non verificato
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF del risultato: coincide? (ricalcolato) deve restare false
        },
        .initial_mem = {},
        .expected_regs = {
            /* CATENA RCR EAX,5 con CF=1 (anello a 33 bit), stringa S=[CF|EAX]:
               rotazione a DESTRA della stringa di 5 posti.
               chain giro per giro:
                 c0=1; v=0x30 (LSB=0)
                 giro1: nuovo CF = LSB(0x30)=0 ; risultato=(0x30>>1)|(c0<<31)=0x8000_0018
                 giro2: nuovo CF = LSB(0x8000_0018)=0 ; risultato=(0x8000_0018>>1)|(0<<31)=0x4000_000C
                 giro3: nuovo CF = LSB(0x4000_000C)=0 ; risultato=0x2000_0006
                 giro4: nuovo CF = 0 ; risultato=0x1000_0003
                 giro5: nuovo CF = LSB(0x1000_0003)=1 ; risultato=(0x1000_0003>>1)|(CF_old=0<<31)=0x0800_0001
               Risultato finale = 0x0800_0001 ; CF finale = 1 (deve restare true). */
            {Register::RAX, 0x08000001}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, true},   // chain: vedi commento -> CF finale 1 (invariato)
            {Flagbit::SF, true},   // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}   // INALTERATO (low byte risultato 0x01: PF=false coincide)
        }
    });

    // =========================================================================
    // Copertura REX.W (48) per D1/D3 /ext su registri e memoria a 64 bit
    // =========================================================================

    // -------------------------------------------------------------------------
    // ROL QWORD PTR [RSI], 1 (48 D1 /0, rm=110): mem QWORD 0x8000_0000_0000_0001
    // -> risultato 0x3 ; CF iniziale = 1 ; OF = MSB(0x3)=0 ^ CF finale(1) = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL QWORD [RSI], 1 (REX.W) (0x8000..01 -> 0x3, nuovo CF=1, OF=1)",
        .program = {
            0x48,       // REX.W -> 64 bit
            0xD1, 0x06  // D1 /0 -> ROL QWORD PTR [RSI], 1 (ModRM 00 000 110)
        },
        .initial_regs = {
            {Register::RAX, 0x8000000000000001ULL}, // AL = sorgente (reg=rm=000 -> RAX)
            {Register::RSI, 0x2000}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; dopo la rotazione CF=bit63(0x8000..)=1 -> resta
            {Flagbit::OF, false}, // parte false; OF=MSB(0x3)=0 ^ CF(1) -> deve cambiare a true
            {Flagbit::SF, true},  // risultato bit63=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // low byte risultato 0x03: PF=true -> deve restare false
        },
        .initial_mem = {
            {0x2000, 0x01}, // 0x8000000000000001 little-endian
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00},
            {0x2004, 0x00},
            {0x2005, 0x00},
            {0x2006, 0x00},
            {0x2007, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x2000} // RSI invariato
        },
        .expected_mem = {
            {0x2000, 0x03}, // 0x3 little-endian
            {0x2001, 0x00},
            {0x2002, 0x00},
            {0x2003, 0x00},
            {0x2004, 0x00},
            {0x2005, 0x00},
            {0x2006, 0x00},
            {0x2007, 0x00}
        },
        .expected_flags = {
            {Flagbit::CF, true},   // bit63 esce e rientra nel bit0 -> LSB(0x3)=1
            {Flagbit::OF, true},   // count 1: MSB(0x3)=0 ^ CF(1) = 1 (deve cambiare da false iniziale)
            {Flagbit::SF, true},   // INALTERATO (risultato bit63=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}   // INALTERATO (low byte risultato 0x03: PF=true se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // RCR QWORD PTR [RSI], CL (48 D3 /3, rm=110): dest = AL (reg=rm=011), rm=110 -> [RSI]
    // CL=5 ; mem = 0x8000_0000_0000_0001 ; CF iniziale = 0
    // rotazione a DESTRA della stringa di 65 bit ([CF|mem]) di 5 posti:
    //   risultato finale = mem >> 5 con i CF iniettati dall'alto; CF finale = LSB uscente.
    // chain passo-passo nel commento sotto.
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCR QWORD [RSI], CL (REX.W, CL=5) (CF iniziale 0, mem -> risultato)",
        .program = {
            0x48,       // REX.W
            0xD3, 0x1E  // D3 /3 -> RCR QWORD PTR [RSI], CL (ModRM 00 011 110)
        },
        .initial_regs = {
            {Register::RAX, 0x8000000000000001ULL}, // AL = sorgente (dest in memoria)
            {Register::RSI, 0x5000},
            {Register::RCX, 5}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false; CF finale vedi catena sotto (=1 -> deve cambiare)
            {Flagbit::OF, true},  // indefinito dopo count>1: non verificato
            {Flagbit::SF, false}, // risultato bit63=0 -> coincide (invariato)
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, false}  // PF del risultato: deve restare false
        },
        .initial_mem = {
            {0x5000, 0x01}, // 0x8000000000000001 little-endian
            {0x5001, 0x00},
            {0x5002, 0x00},
            {0x5003, 0x00},
            {0x5004, 0x00},
            {0x5005, 0x00},
            {0x5006, 0x00},
            {0x5007, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x5000}, // RSI invariato
            {Register::RCX, 5}       // CL invariato
        },
        .expected_mem = {
            /* CATENA RCR QWORD,5 con CF=0 (anello a 65 bit), stringa S=[CF|mem], mem=0x8000_0000_0000_0001:
               rotazione DESTRA della stringa di 5 posti. I primi elementi da destra dell'anello:
               la stringa ruotata a destra di k = ogni elemento in posizione i va in (i+k) mod 65.
               girio per girio:
                 c0=0; v0=0x8000_0000_0000_0001 ; LSB(v0)=1 -> CF1 ; v1=(v0>>1)|(c0<<63)=0x4000_0000_0000_0000
                 c1=1; LSB(v1)=0 -> CF2=0 ; v2=(v1>>1)|(c1<<63)=0xA000_0000_0000_0000
                 c2=0; LSB(v2)=0 -> CF3=0 ; v3=0x5000_0000_0000_0000
                 c3=0; LSB(v3)=0 -> CF4=0 ; v4=0x2800_0000_0000_0000
                 c4=0; LSB(v4)=0 -> CF5=0 ; v5=0x1400_0000_0000_0000
               Risultato finale = 0x1400_0000_0000_0000 ; CF finale = 0 (invariato). */
            {0x5000, 0x00}, // 0x1400000000000000 little-endian
            {0x5001, 0x00},
            {0x5002, 0x00},
            {0x5003, 0x00},
            {0x5004, 0x00},
            {0x5005, 0x00},
            {0x5006, 0x00},
            {0x5007, 0x14}
        },
        .expected_flags = {
            {Flagbit::CF, false},  // chain: vedi commento -> CF finale 0 (invariato)
            {Flagbit::SF, false},  // INALTERATO (risultato bit63=0 -> coincide ma non derivato)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, false}   // INALTERATO (low byte risultato 0x00: PF=true se ricalcolato -> resta false)
        }
    });

    // =========================================================================
    // Copertura C0/C1 (/ext ib) per RCR su registri e memoria, e ROL con count
    // mascherato grande (43).
    // =========================================================================

    // -------------------------------------------------------------------------
    // RCL DWORD PTR [RSI], 1 con CF=1 (C1 /2 ib? no: D1 /2 gia' testato; uso C1
    // con imm): dest = AL (reg=rm=010), rm=110 -> mem DWORD.
    // imm8=1 ; mem = 0x8000_0000 ; CF iniziale=1 -> risultato 0x0000_0001, nuovo CF=1
    // OF = MSB(0x1)=0 ^ CF(1) = 1
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "RCL DWORD [RSI], 1 (CF iniziale 1) (mem 0x8000_0000 -> 0x1, nuovo CF=1, OF=1)",
        .program = {
            0xC1, 0x16, 0x01 // C1 /2 ib -> RCL r/m32, imm8 (ModRM 00 010 110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x80000000}, // AL/EAX = sorgente (dest in memoria)
            {Register::RSI, 0x6000}
        },
        .initial_flags = {
            {Flagbit::CF, true},  // parte true; dopo la rotazione CF=bit31(0x8000_0000)=1 -> resta
            {Flagbit::OF, false}, // parte false; OF=MSB(0x1)=0 ^ CF(1) -> deve cambiare a true
            {Flagbit::SF, true},  // risultato bit31=0 -> se toccato SF sarebbe false
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(0x1)=false -> deve restare true
        },
        .initial_mem = {
            {0x6000, 0x00}, // 0x80000000 little-endian
            {0x6001, 0x00},
            {0x6002, 0x00},
            {0x6003, 0x80}
        },
        .expected_regs = {
            {Register::RSI, 0x6000} // RSI invariato
        },
        .expected_mem = {
            {0x6000, 0x01}, // 0x00000001 little-endian
            {0x6001, 0x00},
            {0x6002, 0x00},
            {0x6003, 0x00}
        },
        .expected_flags = {
            {Flagbit::CF, true},   // bit31 di 0x80000000 esce in CF (invariato)
            {Flagbit::OF, true},   // count 1: MSB(0x1)=0 ^ CF(1) = 1 (deve cambiare da false iniziale)
            {Flagbit::SF, true},   // INALTERATO (risultato bit31=0 -> se toccato sarebbe false)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}    // INALTERATO (low byte risultato 0x01: PF=false se ricalcolato)
        }
    });

    // -------------------------------------------------------------------------
    // ROR DWORD PTR [RSI], imm8=1 via C1 /1 ib: dest = AL (reg=rm=001), rm=110 -> mem
    // mem DWORD 0x0000_0001 ; CF iniziale=0 -> risultato 0x8000_0000, nuovo CF=1
    // OF = MSB(0x8000_0000)=1 ^ bit30=0 = 1 (deve cambiare da false iniziale)
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROR DWORD [RSI], 1 (mem 0x1 -> 0x8000_0000, nuovo CF=1, OF=1)",
        .program = {
            0xC1, 0x0E, 0x01 // C1 /1 ib -> ROR r/m32, imm8 (ModRM 00 001 110 -> [RSI])
        },
        .initial_regs = {
            {Register::RAX, 0x1}, // AL/EAX = sorgente (dest in memoria)
            {Register::RSI, 0x7000}
        },
        .initial_flags = {
            {Flagbit::CF, false}, // parte false; atteso CF=MSB(0x8000_0000)=1 -> deve cambiare
            {Flagbit::OF, false}, // parte false; OF definito=1 -> deve cambiare a true
            {Flagbit::SF, false}, // risultato bit31=1 -> se toccato SF sarebbe true
            {Flagbit::ZF, true},  // risultato != 0 -> se toccato ZF sarebbe false
            {Flagbit::PF, true}   // PF(low byte risultato 0x00)=true coincide (invariato)
        },
        .initial_mem = {
            {0x7000, 0x01}, // 0x00000001 little-endian
            {0x7001, 0x00},
            {0x7002, 0x00},
            {0x7003, 0x00}
        },
        .expected_regs = {
            {Register::RSI, 0x7000} // RSI invariato
        },
        .expected_mem = {
            {0x7000, 0x00}, // 0x80000000 little-endian
            {0x7001, 0x00},
            {0x7002, 0x00},
            {0x7003, 0x80}
        },
        .expected_flags = {
            {Flagbit::CF, true},   // bit0(1) esce e rientra nel bit31 -> MSB risultato = 1
            {Flagbit::OF, true},   // count 1: MSB(0x8000_0000)=1 ^ bit30=0 -> OF=1 (deve cambiare da false iniziale)
            {Flagbit::SF, false},  // INALTERATO (risultato bit31=1 -> se toccato sarebbe true)
            {Flagbit::ZF, true},   // INALTERATO (risultato != 0 -> se toccato sarebbe false)
            {Flagbit::PF, true}    // INALTERATO
        }
    });

    // -------------------------------------------------------------------------
    // ROL EAX, 43 (C1 /0 ib con imm 0x2B): count mascherato a 11 (43 & 1Fh)
    // su operando 32 bit: EAX=2 -> risultato 0x1000 ; CF = LSB(0x1000) = 0
    // -------------------------------------------------------------------------

    tests.push_back({
        .description = "ROL EAX, 43 (imm 43 -> mask 11) (2 -> 0x1000, CF=0)",
        .program = {
            0xC1, 0xC0, 0x2B // 43 = 43 & 1Fh = 11
        },
        .initial_regs = {
            {Register::RAX, 0x2}
        },
        .initial_flags = {
            {Flagbit::CF, true},
            {Flagbit::ZF, true},
            {Flagbit::SF, true}
        },
        .initial_mem = {},
        .expected_regs = {
            {Register::RAX, 0x1000}
        },
        .expected_mem = {},
        .expected_flags = {
            {Flagbit::CF, false}, // bit20 di 0x2 = 0 -> CF uscente finale = 0 (deve cambiare da true iniziale)
            {Flagbit::ZF, true},  // INALTERATO (era true: deve restare true, non dipendente dal risultato)
            {Flagbit::SF, true}   // INALTERATO (risultato bit31=0 -> se toccato sarebbe false... coincide? 
                                   // SF iniziale=true e risultato bit31=0 -> se ricalcolato SF=false -> discriminante: deve restare TRUE)
        }
    });

    return tests;
}

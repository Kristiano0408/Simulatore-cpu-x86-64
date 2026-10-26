#include "opcode_map.hpp"

//REMINDER: the legth of the instruction(for 16/32/64 bit mode)  is standardized to 32bit , the others sizes are modified with prefixes

//REMINDER: the sign extension is set  by the flag in the instruction details struct, with the rex there isa alway
//std::unordered_map<uint32_t, InstructionDetails> opcodeMap = 
ankerl::unordered_dense::map<uint32_t, InstructionDetails> opcodeMap =
{

    // Istruzioni MOV

    // Istruzioni MOV tra registro e immediato
    {0xB0, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV AL, imm8"}},
    {0xB1, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV CL, imm8"}},
    {0xB2, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV DL, imm8"}},
    {0xB3, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV BL, imm8"}},
    {0xB4, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV SPL, imm8"}},
    {0xB5, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV BPL, imm8"}},
    {0xB6, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV SIL, imm8"}},
    {0xB7, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "MOV DIL, imm8"}},

    {0xB8, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV AX/EAX/RAX, imm16/imm32/imm64"}},
    {0xB9, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV CX/ECX/RCX, imm16/imm32/imm64"}},
    {0xBA, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV DX/EDX/RDX, imm16/imm32/imm64"}},
    {0xBB, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV BX/EBX/RBX, imm16/imm32/imm64"}},
    {0xBC, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV SP/ESP/RSP, imm16/imm32/imm64"}},
    {0xBD, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV BP/EBP/RBP, imm16/imm32/imm64"}},
    {0xBE, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV SI/ESI/RSI, imm16/imm32/imm64"}},
    {0xBF, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, false, true, "MOV DI/EDI/RDI, imm16/imm32/imm64"}},

    // Istruzioni MOV tra registro e registro/memoria
    {0x88, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "MOV r/m8, r8"}},
    {0x89, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "MOV r/m16/32/64, r16/32/64"}},
    {0x8A, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "MOV r8, r/m8"}},
    {0x8B, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "MOV r16/32/64, r/m16/32/64"}},

    //istuzioni move con offset
    {0xC6, {3, 1, 2, 2, 1, 8, 8, 0, 0, true, false, true, "MOV r/m8, imm8"}},
    {0xC7, {6, 1, 5, 2, 4, 32, 32, 2, 1, true, false, true, "MOV r/m16/32/64, imm16/32/64"}},

    //itruzioni tra rax e memoria
    {0xA0, {5, 1, 4, 1, 4, 8, 8, 0, 0, false, true, false, "MOV AL, moffs8"}},
    {0xA1, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, true, false, "MOV EAX/RAX, moffs32/64"}},
    {0xA2, {5, 1, 4, 1, 4, 8, 8, 0, 0, false, true, false, "MOV moffs8, AL"}},
    {0xA3, {5, 1, 4, 1, 4, 32, 32, 0, 0, false, true, false, "MOV moffs32/64, EAX/RAX"}},


    // Istruzioni ADD

    {0x04, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "ADD AL, imm8"}},
    {0x05, {5, 1, 4, 1, 4, 32, 32, 2, 1, false, false, true, "ADD EAX/RAX, imm16/imm32/imm64"}},
    {0x8000, {3, 1, 2, 2, 1, 8, 8, 0, 0, true, false, true, "ADD r/m8, imm8"}},
    {0x8100, {6, 1, 5, 2, 4, 32, 32, 2, 1, true, false, true, "ADD r/m16/32/64, imm16/32/64"}},
    {0x8300, {3, 1, 2, 2, 1, 8, 32, 2, 0, true, false, true, "ADD r/m16/32/64, imm8"}},

    {0x00, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "ADD r/m8, r8"}},
    {0x01, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "ADD r/m16/32/64, r16/32/64"}},
    {0x02, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "ADD r8 , r/m8"}},
    {0x03, {2, 1, 1, 2, 1 ,32 ,32 ,0 ,0 ,true ,false ,false , "ADD r16/32/64 , r/m16/32/64"}},


    // Istruzioni SUB
    {0x2C, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "SUB AL, imm8"}},
    {0x2D, {5, 1, 4, 1, 4, 32, 32, 2, 1, false, false, true, "SUB  AX/EAX/RAX, imm16/imm32/imm64"}},
    {0x8005, {3, 1, 2, 2, 1, 8, 8, 0, 0, true, false, true, "SUB r/m8, imm8"}},
    {0x8105, {6, 1, 5, 2, 4, 32, 32, 2, 1, true, false, true, "SUB r/m16/32/64, imm16/32/64"}},
    {0x8305, {3, 1, 2, 2, 1, 8, 8, 2, 0, true, false, true, "SUB r/m16/32/64, imm8"}},

    {0x28, {2, 1, 1, 2, 1 , 8 , 8 , 0 , 0 ,true ,false ,false , "SUB r/m8 , r8"}},
    {0x29, {2, 1, 1, 2, 1 ,32 ,32 ,0 ,0 ,true ,false ,false , "SUB r/m16/32/64 , r16/32/64"}},
    {0x2A, {2, 1, 1, 2, 1 , 8 , 8 , 0 , 0 ,true ,false ,false , "SUB r8 , r/m8"}},
    {0x2B, {2, 1, 1, 2, 1 ,32 ,32 ,0 ,0 ,true ,false ,false , "SUB r16/32/64 , r/m16/32/64"}},

    // Istruzioni CMP (nessuna scrittura del risultato, aggiorna solo i flag)
    {0x3C, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "CMP AL, imm8"}},
    {0x3D, {5, 1, 4, 1, 4, 32, 32, 2, 1, false, false, true, "CMP AX/EAX/RAX, imm16/imm32/imm64"}},
    {0x8007, {3, 1, 2, 2, 1, 8, 8, 0, 0, true, false, true, "CMP r/m8, imm8"}},
    {0x8107, {6, 1, 5, 2, 4, 32, 32, 2, 1, true, false, true, "CMP r/m16/32/64, imm16/32/64"}},
    {0x8307, {3, 1, 2, 2, 1, 8, 32, 2, 0, true, false, true, "CMP r/m16/32/64, imm8"}},

    {0x38, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "CMP r/m8, r8"}},
    {0x39, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "CMP r/m16/32/64, r16/32/64"}},
    {0x3A, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "CMP r8, r/m8"}},
    {0x3B, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "CMP r16/32/64, r/m16/32/64"}},

    // Istruzioni ADC
    {0x14, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "ADC AL, imm8"}},
    {0x15, {5, 1, 4, 1, 4, 32, 32, 2, 1, false, false, true, "ADC EAX/RAX, imm16/imm32/imm64"}},
    {0x8002, {3, 1, 2, 2, 1, 8, 8, 0, 0, true, false, true, "ADC r/m8, imm8"}},
    {0x8102, {6, 1, 5, 2, 4, 32, 32, 2, 1, true, false, true, "ADC r/m16/32/64, imm16/32/64"}},
    {0x8302, {3, 1, 2, 2, 1, 8, 32, 2, 0, true, false, true, "ADC r/m16/32/64, imm8"}},
    {0x10, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "ADC r/m8, r8"}},
    {0x11, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "ADC r/m16/32/64, r16/32/64"}},
    {0x12, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "ADC r8, r/m8"}},
    {0x13, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "ADC r16/32/64, r/m16/32/64"}},

    // Istruzioni SBB
    {0x1C, {2, 1, 1, 1, 1, 8, 8, 0, 0, false, false, true, "SBB AL, imm8"}},
    {0x1D, {5, 1, 4, 1, 4, 32, 32, 2, 1, false, false, true, "SBB AX/EAX/RAX, imm16/imm32/imm64"}},
    {0x8003, {3, 1, 2, 2, 1, 8, 8, 0, 0, true, false, true, "SBB r/m8, imm8"}},
    {0x8103, {6, 1, 5, 2, 4, 32, 32, 2, 1, true, false, true, "SBB r/m16/32/64, imm16/32/64"}},
    {0x8303, {3, 1, 2, 2, 1, 8, 8, 2, 0, true, false, true, "SBB r/m16/32/64, imm8"}},
    {0x18, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "SBB r/m8, r8"}},
    {0x19, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "SBB r/m16/32/64, r16/32/64"}},
    {0x1A, {2, 1, 1, 2, 1, 8, 8, 0, 0, true, false, false, "SBB r8, r/m8"}},
    {0x1B, {2, 1, 1, 2, 1, 32, 32, 0, 0, true, false, false, "SBB r16/32/64, r/m16/32/64"}},

    // Istruzioni INC / DEC (Gruppo 4 per 8-bit, Gruppo 5 per 16/32/64-bit)
    {0xFE00, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "INC r/m8"}},
    {0xFE01, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "DEC r/m8"}},
    {0xFF00, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "INC r/m16/32/64"}},
    {0xFF01, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "DEC r/m16/32/64"}},

    // Istruzioni NEG (Gruppo 3, reg = 011b)
    {0xF603, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "NEG r/m8"}},
    {0xF703, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "NEG r/m16/32/64"}},

    // Istruzioni LEA
    {0x8D, {3, 1, 2, 2, 1, 32, 32, 2, 0, true, false, false, "LEA r32, [r/m]"}},

    
    // MUL (1 operando, Gruppo 3, reg = 4)
    {0xF604, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "MUL r/m8"}},
    {0xF704, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "MUL r/m16/32/64"}},

    // IMUL (1 operando, Gruppo 3, reg = 5)
    {0xF605, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "IMUL r/m8"}},
    {0xF705, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "IMUL r/m16/32/64"}},

    // IMUL (2 operandi: IMUL reg, r/m)
    {0x0FAF, {3, 2, 1, 2, 4, 32, 32, 0, 0, true, false, false, "IMUL r16/32/64, r/m16/32/64"}},

    // IMUL (3 operandi: IMUL reg, r/m, imm)
    {0x6B, {3, 1, 2, 3, 1, 8, 32, 2, 0, true, false, true, "IMUL r16/32/64, r/m16/32/64, imm8"}},
    {0x69, {6, 1, 5, 3, 4, 32, 32, 2, 1, true, false, true, "IMUL r16/32/64, r/m16/32/64, imm16/32"}},

    // DIV (Gruppo 3, reg = 6)
    {0xF606, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "DIV r/m8"}},
    {0xF706, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "DIV r/m16/32/64"}},

    // IDIV (Gruppo 3, reg = 7)
    {0xF607, {2, 1, 1, 1, 1, 8, 8, 0, 0, true, false, false, "IDIV r/m8"}},
    {0xF707, {2, 1, 1, 1, 4, 32, 32, 0, 0, true, false, false, "IDIV r/m16/32/64"}},
    
    // MOVZX r16/32/64, r/m8   (0F B6)
    {0x0FB6, {3, 2, 1, 2, 1, 8, 32, 1, 0, true, false, false, "MOVZX r16/32/64, r/m8"}},
    // MOVZX r32/64, r/m16     (0F B7)
    {0x0FB7, {3, 2, 1, 2, 2, 16, 32, 1, 0, true, false, false, "MOVZX r32/64, r/m16"}},
    // MOVSX r16/32/64, r/m8   (0F BE)
    {0x0FBE, {3, 2, 1, 2, 1, 8, 32, 2, 0, true, false, false, "MOVSX r16/32/64, r/m8"}},
    // MOVSX r32/64, r/m16     (0F BF)
    {0x0FBF, {3, 2, 1, 2, 2, 16, 32, 2, 0, true, false, false, "MOVSX r32/64, r/m16"}},
    // MOVSXD r64, r/m32       (REX.W + 63)
    {0x63,   {2, 1, 1, 2, 4, 32, 64, 2, 1, true, false, false, "MOVSXD r64, r/m32"}},







    /*
    
    // Istruzioni di controllo del flusso
    {0xEB, {2, 1, 1, "JMP rel8"}},
    {0xE9, {5, 1, 4, "JMP rel32"}},
    {0xE8, {5, 1, 4, "CALL rel32"}},
    {0xC3, {1, 1, 0, "RET"}},

    // Istruzioni di stack
    {0x50, {1, 1, 0, "PUSH r32"}},
    {0x58, {1, 1, 0, "POP r32"}},
    {0xFF, {2, 1, 1, "PUSH r/m32"}},
    {0x8F, {2, 1, 1, "POP r/m32"}},

    // Istruzioni logiche
    {0x24, {2, 1, 1, "AND AL, imm8"}},
    {0x25, {5, 1, 4, "AND EAX, imm32"}},
    {0x80, {3, 1, 2, "AND r/m8, imm8"}},
    {0x81, {6, 1, 5, "AND r/m32, imm32"}},

    // Istruzioni di moltiplicazione
    {0x0FAF, {3, 2, 1, "IMUL r32, r/m32"}},
    {0x0F38F1, {4, 3, 1, "CRC32 r32, r/m8"}},*/
};
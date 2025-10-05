#include <vector>
#include <unordered_map>
#include <cstdint>
#include "opcode_map.hpp"

//REMINDER: the legth of the instruction(for 16/32/64 bit mode)  is standardized to 32bit , the others sizes are modified with prefixes

//REMINDER: the sign extension is set  by the flag in the instruction details struct, with the rex there isa alway
std::unordered_map<uint32_t, InstructionDetails> opcodeMap = {

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


    // Istruzioni AND
    







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
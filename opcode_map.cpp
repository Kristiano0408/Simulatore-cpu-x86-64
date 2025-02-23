#include <vector>
#include <unordered_map>
#include <cstdint>

// Struct per memorizzare i dettagli dell'istruzione
struct InstructionDetails {
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t additionalBytes; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    size_t numOperands; // Numero di operandi
    const char* description; // Descrizione dell'istruzione (opzionale)
};

std::unordered_map<uint32_t, InstructionDetails> opcodeMap = {
    // Istruzioni MOV
    {0xB8, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xB9, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xBA, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xBB, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xBC, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xBD, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xBE, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},
    {0xBF, {5, 1, 4, 1, "MOV r16/r32/r64, imm16/imm32/imm64"}},

    /*
    // Istruzioni ADD
    {0x04, {2, 1, 1, "ADD AL, imm8"}},
    {0x05, {5, 1, 4, "ADD EAX, imm32"}},
    {0x80, {3, 1, 2, "ADD r/m8, imm8"}},
    {0x81, {6, 1, 5, "ADD r/m32, imm32"}},

    // Istruzioni SUB
    {0x2C, {2, 1, 1, "SUB AL, imm8"}},
    {0x2D, {5, 1, 4, "SUB EAX, imm32"}},
    {0x80, {3, 1, 2, "SUB r/m8, imm8"}},
    {0x81, {6, 1, 5, "SUB r/m32, imm32"}},

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
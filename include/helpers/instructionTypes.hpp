#ifndef INSTRUCTION_TYPES_HPP
#define INSTRUCTION_TYPES_HPP

#include "types.hpp"
#include <cstdint>

struct r_m {
    uint8_t r_m;
    uint8_t mod;
    uint8_t reg;
    std::byte byte_r_m;
};

struct SIB {
    uint8_t base;
    uint8_t index;
    uint8_t scale;
    std::byte byte_sib;
};

//enum for the type of instruction
enum class TypeofInstruction : uint8_t
{
    MOV  = 0, //move instruction
    ADD = 1, //add instruction
    SUB = 2, //sub instruction
    LEA = 3, //load effective address
    ADC = 4, // Add with carry
    SBB = 5, // Subtract with borrow
    INC = 6, //increment instruction
    DEC = 7, //decrement instruction
    NEG = 8, //two's complement negate
    CMP = 9, //compare two operands (flags only, result discarded)
    MUL = 10, //multiply instruction
    IMUL = 11, //signed multiply instruction
    DIV = 12, //divide instruction
    IDIV = 13, //signed divide instruction
    MOVZX = 14, //move with zero extension
    MOVSX = 15, //move with sign extension
    MOVSXD = 16, //move with sign extension to doubleword
    COUNT = 17

   
};

//enum for the addressing mode of the instruction
enum class AddressingMode : uint8_t
{
    I = 0,  //move immediate to accumulator
    OI = 1, //move immediate to register
    MI = 2, //move immediate to memory/register
    MR = 3, //move register to R/M
    RM = 4, //move R/M to register
    FD = 5, //move from offset to Rax
    TD = 6, //move from Rax to offset
    M = 7,  //single operand r/m (used for unary instructions like INC/DEC)
    M_ACC = 8, //single operand r/m with accumulator for IMUL,MUL, DIV, IDIV (this doest existin real intel docs, it is separated from M only for the sake of clarity and for make easier the implementation of the operands)
               //it doesnt have his decode func, it differs from fetchoperands to the end 
    RMI = 9, //move r/m  to reg with immediate (used for IMUL with 3 operands)
    COUNT = 10

};

enum class InstructionExecutionMode : uint8_t
{
    ALU,
    DATA_TRANSFER,
    CONTROL_FLOW,
    SYSTEM,

};

enum class InstructionDataTransferMode : uint8_t
{
    REGISTER_TO_REGISTER,
    REGISTER_TO_MEMORY,
    MEMORY_TO_REGISTER,
    IMMEDIATE_TO_REGISTER,
    IMMEDIATE_TO_ACCUMULATOR,
    IMMEDIATE_TO_MEMORY,
    MEMORY_TO_MEMORY,
    
};

//struct for the instruction type and relative addressing mode
struct InstructionType_and_addMode
{
    TypeofInstruction type;
    AddressingMode mode;
    InstructionExecutionMode executionMode;
};

struct temporaryValues {
        uint64_t srcValue{}; //value of the source operand
        uint64_t destValue{}; //value of the destination operand
        uint64_t destHighValue{}; //value of the destination operand for high part (used for MUL/IMUL)(RDX/EDX/DX)
        uint64_t auxValue{}; //auxiliary value for the operation (used for IMUL 3 operands)
        uint64_t resultValue{}; //result of the operation
        uint64_t resultHighValue{}; //result of the operation for high part (used for MUL/IMUL)(RDX/EDX/DX)
        uint8_t nOperands{}; //number of bits of the value/operand
        bool isSrcValueReady = false; //flag to indicate if the source value is ready
        bool isDestValueReady = false; //flag to indicate if the destination value is ready
        bool isAuxValueReady = false; //flag to indicate if the auxiliary value is ready
        bool carryIn = false; // Carry/Borrow in ingresso per ADC/SBB
        bool CF; //Carry Flag
        bool ZF; //Zero Flag
        bool SF; //Sign Flag
        bool OF; //Overflow Flag
        bool PF; //Parity Flag
        bool AF; //Auxiliary Carry Flag
        uint64_t flagsMask{}; // Mask for the flags that are affected by the operation

        void setFlagMask(uint8_t flagPos, bool value) {
            if (value) {
                flagsMask|= (1ULL << static_cast<uint8_t>(flagPos));
            } else {
                flagsMask &= ~(1ULL << static_cast<uint8_t>(flagPos));
            }
        }

        bool isFlagAffected(uint8_t flagPos) const {
            return (flagsMask & (1ULL << static_cast<uint8_t>(flagPos))) != 0;
        }
};

struct InstructionCore
{
    uint64_t InstructionId; //unique id for the instruction
    uint64_t value;
    uint64_t displacement;
    uint32_t opcode;
    uint32_t SIBdisplacement;
    std::array<uint8_t, 4> prefix;
    uint8_t rexprefix;
    uint8_t numPrefixes;
    uint8_t srcNbit;
    uint8_t destNbit;
    uint8_t auxNbit;
    uint8_t immediateNbit;
    uint8_t numOperands;
    r_m rm;
    SIB sib;
    TypeofInstruction type; //type of the instruction (arithmetic, logical, control flow, etc.)
    AddressingMode addressingMode; //addressing mode of the instruction
    InstructionExecutionMode executionMode; //execution mode of the instruction (ALU, data transfer, control flow, etc.)


};

struct InstructionFlags
{
    //for avoiding waisting space, we can use bit fields for the flags
    uint8_t rex : 1;
    uint8_t hasImmediate : 1;
    uint8_t hasDisplacement : 1;
    uint8_t hasModRM : 1;
    uint8_t hasSIB : 1;
    uint8_t regToReg : 1;
    uint8_t regToMem : 1;
    uint8_t memToReg : 1;
    uint8_t waitingSrcOperand : 1;
    uint8_t waitingDestOperand : 1;
    InstructionDataTransferMode dataTransferMode;
};

struct InstructionInfo {
    uint64_t instructionId{}; // Unique ID for the instruction
    size_t totalLength{};  // Lunghezza totale dell'istruzione
    size_t opcodeLength{}; // Lunghezza dell'opcode (1, 2 o 3 byte)
    std::array<uint8_t, 4> prefix{}; // Array dei prefissi (max 4)
    uint32_t opcode{};     // Opcode completo (1, 2 o 3 byte)
    uint8_t rexprefix{};   // Prefisso REX
    uint8_t additionalBytes{}; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    uint8_t numOperands{}; // Numero di operandi
    uint8_t operandLength{}; // Lunghezza degli operandi
    uint8_t src_operand_length{}; // Lunghezza dell'operando sorgente
    uint8_t dest_operand_length{}; // Lunghezza dell'operando destinazione
    uint8_t bit_extension{}; // Estensione in bit (0 se non esteso, 1 per 0 extension, 2 per sign extension)
    uint8_t prefixCount{};  // Numero di prefissi
   
    MaxCPUInstructionLength instruction; // Rappresentazione dell'istruzione in formato testuale
    const char* description{}; // Descrizione dell'istruzione (opzionale)
    bool rex_w_sensitive{}; // Indica se l'istruzione è sensibile al prefisso REX.W (se 0 sorgente e destinazione sono 64 bit, se 1 sign extension a 64 bit per sorgente)
    bool rex{};            // Flag REX
    bool hasModRM{};            // Presenza del byte ModR/M
    bool hasSIB{};              // Presenza del byte SIBs
    bool hasDisplacement{};     // Presenza di un displacement
    bool hasImmediate{};        // Presenza di un valore immediato

};





#endif // INSTRUCTION_TYPES_HPP
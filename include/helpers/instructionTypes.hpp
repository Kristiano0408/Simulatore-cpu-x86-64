#ifndef INSTRUCTION_TYPES_HPP
#define INSTRUCTION_TYPES_HPP


struct r_m {
    uint8_t r_m : 3;
    uint8_t mod : 2;
    uint8_t reg : 3;
    uint8_t byte_r_m;
};

struct SIB {
    uint8_t base : 3;
    uint8_t index : 3;
    uint8_t scale : 2;
    uint8_t byte_sib;
};

//enum for the type of instruction
enum class TypeofInstruction : uint8_t
{
    MOV, //move instruction
    ADD, //add instruction
    SUB, //sub instruction

   
};

//enum for the addressing mode of the instruction
enum class AddressingMode : uint8_t
{
    I,  //move immediate to accumulator
    OI, //move immediate to register
    MI, //move immediate to memory/register
    MR, //move register to R/M
    RM, //move R/M to register
    FD, //move from offset to Rax
    TD, //move from Rax to offset

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
        uint64_t srcValue; //value of the source operand
        uint64_t destValue; //value of the destination operand
        uint64_t resultValue; //result of the operation
        bool CF; //Carry Flag
        bool ZF; //Zero Flag
        bool SF; //Sign Flag
        bool OF; //Overflow Flag
        bool PF; //Parity Flag
        bool AF; //Auxiliary Carry Flag
};

struct InstructionCore
{
    uint64_t InstructionId; //unique id for the instruction
    uint64_t value;
    uint64_t displacement;
    uint32_t opcode;
    uint32_t SIBdisplacement;
    std::array<uint8_t, 4> prefix;
    int8_t rexprefix;
    uint8_t numPrefixes;
    uint8_t nbit; //number of bits of the value/operand
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
    uint64_t instructionId; // Unique ID for the instruction
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    std::array<uint8_t, 4> prefix; // Array dei prefissi (max 4)
    uint32_t opcode;     // Opcode completo (1, 2 o 3 byte)
    uint16_t rexprefix;   // Prefisso REX
    uint8_t additionalBytes; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    uint8_t numOperands; // Numero di operandi
    uint8_t operandLength; // Lunghezza degli operandi
    uint8_t src_operand_length; // Lunghezza dell'operando sorgente
    uint8_t dest_operand_length; // Lunghezza dell'operando destinazione
    uint8_t bit_extension; // Estensione in bit (0 se non esteso, 1 per 0 extension, 2 per sign extension)
   
    std::vector<uint8_t> instruction;
    const char* description; // Descrizione dell'istruzione (opzionale)
    bool rex_w_sensitive; // Indica se l'istruzione è sensibile al prefisso REX.W (se 0 sorgente e destinazione sono 64 bit, se 1 sign extension a 64 bit per sorgente)
    bool rex;            // Flag REX
    bool hasModRM;            // Presenza del byte ModR/M
    bool hasSIB;              // Presenza del byte SIBs
    bool hasDisplacement;     // Presenza di un displacement
    bool hasImmediate;        // Presenza di un valore immediato

    // Costruttore di default
    InstructionInfo() : instructionId(0), totalLength(0), opcodeLength(0), prefixCount(0), prefix{0},
                        opcode(0),rexprefix(0),additionalBytes(0), numOperands(0), operandLength(0),
                        src_operand_length(0), dest_operand_length(0), bit_extension(0), instruction(), description(""), rex_w_sensitive(false),
                        rex(false),hasModRM(false), hasSIB(false), hasDisplacement(false), hasImmediate(false)
                         {}

};





#endif // INSTRUCTION_TYPES_HPP
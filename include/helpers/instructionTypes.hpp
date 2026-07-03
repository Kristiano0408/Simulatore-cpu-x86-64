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
enum class TypeofInstruction
{
    MOV, //move instruction
    ADD, //add instruction
    SUB, //sub instruction

   
};

//enum for the addressing mode of the instruction
enum class AddressingMode
{
    I,  //move immediate to accumulator
    OI, //move immediate to register
    MI, //move immediate to memory/register
    MR, //move register to R/M
    RM, //move R/M to register
    FD, //move from offset to Rax
    TD, //move from Rax to offset

};

//struct for the instruction type and relative addressing mode
struct InstructionType_and_addMode
{
    TypeofInstruction type;
    AddressingMode mode;
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


struct InstructionInfo {
    uint64_t instructionId; // Unique ID for the instruction
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    uint8_t prefix[4] {0};   // Prefissi
    uint16_t rexprefix;   // Prefisso REX
    uint32_t opcode;     // Opcode completo (1, 2 o 3 byte)
    size_t additionalBytes; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    size_t numOperands; // Numero di operandi
    size_t operandLength; // Lunghezza degli operandi
    size_t src_operand_length; // Lunghezza dell'operando sorgente
    size_t dest_operand_length; // Lunghezza dell'operando destinazione
    size_t bit_extension; // Estensione in bit (0 se non esteso, 1 per 0 extension, 2 per sign extension)
    bool rex_w_sensitive; // Indica se l'istruzione è sensibile al prefisso REX.W (se 0 sorgente e destinazione sono 64 bit, se 1 sign extension a 64 bit per sorgente)
    bool rex;            // Flag REX
    bool hasModRM;            // Presenza del byte ModR/M
    bool hasSIB;              // Presenza del byte SIB
    bool hasDisplacement;     // Presenza di un displacement
    bool hasImmediate;        // Presenza di un valore immediato
    std::vector<uint8_t> instruction;
    const char* description; // Descrizione dell'istruzione (opzionale)

    // Costruttore di default
    InstructionInfo() : instructionId(0), totalLength(0), opcodeLength(0), prefixCount(0), 
                        rexprefix(0), opcode(0), additionalBytes(0), numOperands(0), operandLength(0),
                        src_operand_length(0), dest_operand_length(0), bit_extension(0), rex_w_sensitive(false),
                        rex(false),hasModRM(false), hasSIB(false), hasDisplacement(false), hasImmediate(false),
                        instruction(), description("") {}

};





#endif // INSTRUCTION_TYPES_HPP
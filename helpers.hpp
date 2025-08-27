#ifndef HELPERS_HPP
#define HELPERS_HPP
#include<string>
#include <vector> 
#include <cstddef>

//farward declaration of the enum class for registers
enum class Register;

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

//////////////////////////////////////////////////////////////////

enum class ComponentType {
    CACHE,
    CACHE_L1,
    CACHE_L2,
    CACHE_L3,
    RAM,
    OPERAND,
    ALU,
    FPU,
    UNKNOWN
};

enum class ErrorType {
    NONE,
    INVALID_ADDRESS,
    INVALID_SIZE,
    OUT_OF_BOUNDS,
    WRITE_FAIL,
    READ_FAIL,
    UNKNOWN
};

enum class EventType {
    NONE,
    CACHE_HIT,
    CACHE_MISS,
    RAM_ACCESS,
    ERROR,
    CACHE_READ_ERROR,
    CACHE_WRITE_ERROR,
    RAM_READ_ERROR,
    RAM_WRITE_ERROR
};

struct Error_Event_Info {
    ComponentType source = ComponentType::UNKNOWN; // Source of the error or event
    EventType event = EventType::NONE;
    ErrorType error = ErrorType::NONE;
    std::string message =""; // Error message optional for debugging
};

template<typename T>
struct Result {
    T data;
    bool success;
    Error_Event_Info errorInfo; // Error information if any
};

//specialization for void
template<>
struct Result<void> {
    bool success;
    Error_Event_Info errorInfo; // Error information if any
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isPrefix(unsigned char byte); // Dichiarazione della funzione

bool isMoveInstruction(uint32_t opcode);

bool isMoveInstructionIO(uint32_t opcode);

bool isMoveInstructionIO_8bit(uint32_t opcode);

bool isMoveInstructionIO_mem(uint32_t opcode);

bool isMoveInstructionOffset(uint32_t opcode);

bool isMoveInstructionOffsetRAX_mem(uint32_t opcode);

bool isMoveInstructionOffsetMem_RAx(uint32_t opcode);

bool isMoveInstructionR_M(uint32_t opcode);

bool isMoveR_M_reg_mem(uint32_t opcode);

bool isMoveR_M_mem_reg(uint32_t opcode);


Register decodeRegisterReg(uint8_t reg, uint8_t rexprefix);

Register decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB);

Register decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB);

Register decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB);

template<typename T>
T castTo(uint64_t value) {

    static_assert(std::is_integral<T>::value, "T must be an integral type");
    return static_cast<T>(value);
}


template<typename T>
Result<void> From_T_toVoid (const Result<T>& r)
{
    Result<void> result_void;
    result_void.success = r.success;
    result_void.errorInfo = r.errorInfo;
    return result_void;
}



// Utility function to compute log2 of power-of-two sizes
inline unsigned ilog2(uint64_t x)
{
    return (x == 1) ? 0 : 1 + ilog2(x >> 1);
}

constexpr unsigned ilog2_constexpr(uint64_t x)
{
    return (x == 1) ? 0 : 1 + ilog2_constexpr(x >> 1);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct InstructionInfo {
    size_t totalLength;  // Lunghezza totale dell'istruzione
    size_t opcodeLength; // Lunghezza dell'opcode (1, 2 o 3 byte)
    size_t prefixCount;  // Numero di prefissi
    uint8_t prefix[4] {0};   // Prefissi
    bool rex;            // Flag REX
    uint16_t rexprefix;   // Prefisso REX
    uint32_t opcode;     // Opcode completo (1, 2 o 3 byte)
    size_t additionalBytes; // Byte aggiuntivi (ModR/M, SIB, displacement, immediate)
    size_t numOperands; // Numero di operandi
    size_t operandLength; // Lunghezza degli operandi
    bool hasModRM;            // Presenza del byte ModR/M
    bool hasSIB;              // Presenza del byte SIB
    bool hasDisplacement;     // Presenza di un displacement
    bool hasImmediate;        // Presenza di un valore immediato
    std::vector<uint8_t> instruction;
    const char* description; // Descrizione dell'istruzione (opzionale)

};


/////////////////////////////////////////////////////////////////////////////////////////

enum class CPUState {
    FETCH,
    DECODE,
    OPERAND_FETCH,
    EXECUTE,
    COMPLETE
};

#endif // HELPERS_HPP
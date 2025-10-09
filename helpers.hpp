#ifndef HELPERS_HPP
#define HELPERS_HPP
#include<string>
#include <vector> 
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <concepts>
#include <type_traits>
#include <algorithm>
#include <cstring>

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

    // operator= solo per std::array
    template<size_t N>
    Result<std::array<uint8_t, N>>& operator=(const Result<std::array<uint8_t, N>>& other) {
    std::memcpy(data.data(), other.data.data(), N);
    success = other.success;
    errorInfo = other.errorInfo;
    return *this;}
};

//specialization for void
template<>
struct Result<void> {
    bool success;
    Error_Event_Info errorInfo; // Error information if any
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t N, size_t M>
void copyPartial(Result<std::array<uint8_t, N>>& dest, const Result<std::array<uint8_t, M>>& src) {
    size_t bytesToCopy = std::min(N, M);
    std::memcpy(dest.data.data(), src.data.data(), bytesToCopy);
    dest.success = src.success;
    dest.errorInfo = src.errorInfo;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const ErrorType& type);

std::ostream& operator<<(std::ostream& os, const EventType& type);

std::ostream& operator<<(std::ostream& os, const ComponentType& type);

template <typename T>
std::ostream& operator<<(std::ostream& os, const Result<T>& result)
{
    if constexpr (std::is_same_v<T, void>) {
        return os << "success: " << result.success << "\n"
                  << "Error info:" << result.errorInfo.source << " "
                  << result.errorInfo.event << " "
                  << result.errorInfo.error << std::endl;
    }
    else {
        return os << "Data: " << result.data << "\n"
                  << "success: " << result.success << "\n"
                  << "Error info:" << result.errorInfo.source << " "
                  << result.errorInfo.event << " "
                  << result.errorInfo.error << std::endl;
    }
}

template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const Result<std::array<T, N>>& result)
{
    os << "Data: ";
    for (const auto& byte : result.data) {
        os << std::hex << static_cast<int>(byte) << " ";
    }
    return os << "\n"
              << "success: " << result.success << "\n"
              << "Error info:" << result.errorInfo.source << " "
              << result.errorInfo.event << " "
              << result.errorInfo.error << std::endl;
}



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
     if (x == 0)
        std::cerr << "Error: log2(0) is undefined." << std::endl;

    unsigned result = 0;
    while (x > 1) {
        x >>= 1;   // sposta a destra di 1 bit
        result++;  // conta quanti shift fai
    }
    return result;
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
    size_t src_operand_length; // Lunghezza dell'operando sorgente
    size_t dest_operand_length; // Lunghezza dell'operando destinazione
    size_t bit_extension; // Estensione in bit (0 se non esteso, 1 per 0 extension, 2 per sign extension)
    bool rex_w_sensitive; // Indica se l'istruzione è sensibile al prefisso REX.W (se 0 sorgente e destinazione sono 64 bit, se 1 sign extension a 64 bit per sorgente)
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
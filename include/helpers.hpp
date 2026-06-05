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
#include <variant>
#include <sstream>
#include <iomanip>
#include <array>
#include <functional>

//defined constants
constexpr unsigned CACHE_LINE_SIZE = 64; // Size of a cache line in bytes


using anydata = std::variant<std::monostate, uint8_t, uint16_t, uint32_t, uint64_t, std::array<uint8_t, CACHE_LINE_SIZE>,  std::array<uint8_t,2*CACHE_LINE_SIZE>, std::array<uint8_t, 16>>;

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
    UNKNOWN,
    WAITING_MEMORY
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

bool isPrefix(uint8_t byte);

Register decodeRegisterReg(uint8_t reg, uint8_t rexprefix);

Register decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB);

Register decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB);

Register decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    uint64_t instructionId; // Unique ID for the instruction
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

    // Costruttore di default
    InstructionInfo() : instructionId(0), totalLength(0), opcodeLength(0), prefixCount(0), rex(false),
                        rexprefix(0), opcode(0), additionalBytes(0), numOperands(0), operandLength(0),
                        src_operand_length(0), dest_operand_length(0), bit_extension(0), rex_w_sensitive(false),
                        hasModRM(false), hasSIB(false), hasDisplacement(false), hasImmediate(false),
                        instruction(), description("") {}

};


/////////////////////////////////////////////////////////////////////////////////////////
//enum for the type of instruction


enum class typeofInstruction
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
    typeofInstruction type;
    AddressingMode mode;
};



////////////////////////////////////////////////////////////////////////////////////////////

enum class StageStatus {
    READY,
    MEMORY_DONE,
    STALLED,
    WAITING_MEMORY,
    WAITING_SRC_OPERAND,
    WAITING_DEST_OPERAND,
    FLUSHED,
    EMPTY,
    
    

    
};

enum class StageType {
    FETCH,
    DECODE,
    OPERAND_FETCH,
    EXECUTE,
    MEMORY,
    WRITE_BACK
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Request type enumeration 
enum class RequestType
{
    READ,
    WRITE,
    FILL, // For filling cache lines during a miss
    PREFETCH, // For prefetching cache lines
    NONE
};

////////////////////////////////////////////////////////////////////////////7

//RequestState enum class RequestState

enum class RequestState
{
    WAITING_LATENCY,
    READY_TO_PROCESS,
    IDLE,
    PROCESSING,
    HIT_READY,
    MISS_PENDING,
    DONE
};

enum class TypeofData
{
    UINT_8T = 1,
    UINT_16T = 2,
    UINT_32T = 4,
    UINT_64T = 8,
    ARRAY_16B = 16,
    ARRAY_64B = 64,
    ARRAY_128B = 128,
    UNKNOWN = 0
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct temporaryValues {
        anydata srcValue; //value of the source operand
        anydata destValue; //value of the destination operand
        anydata resultValue; //result of the operation
        bool CF; //Carry Flag
        bool ZF; //Zero Flag
        bool SF; //Sign Flag
        bool OF; //Overflow Flag
        bool PF; //Parity Flag
        bool AF; //Auxiliary Carry Flag
};

////////////////////////////////////////////////////////////////////////////////////////////////////


//request structure(the cache manager will use it to manage read and write requests from the cpu)
struct CacheRequest
{
    RequestType type = RequestType::NONE; // Type of request (READ or WRITE)
    TypeofData dataType = TypeofData::UNKNOWN; // Type of data for the request
    uint64_t address = 0; // Memory address
    std::array<uint8_t, 16> data{}; // Data for write requests (up to 15 bytes, maximun size for an instruction with prefixes and opcode, 16 bytes to align)
    bool completed = false; // Indicates if the request has been completed
    int requestID = 0; // Unique ID for the request
    std::function<void()> callback; // Callback function to be called when the request is completed

    CacheRequest(): type(RequestType::NONE), dataType(TypeofData::UNKNOWN), address(0), data{}, completed(false), requestID(0), callback(nullptr) {}

    CacheRequest(RequestType type, TypeofData dataType, uint64_t address, const std::array<uint8_t, 16>& data, bool completed, uint64_t requestID, std::function<void()> callback)
        : type(type), dataType(dataType), address(address), data(data), completed(completed), requestID(requestID), callback(callback) {}
};




//for response structure we will use the Result<T> structure in a cpu map


//////////////////////////////////////////////////////////////////////////////////////////////

//debuging functions
inline void debugLog([[maybe_unused]] const std::string& message) {

    #ifdef DEBUG
    std::cout << "[DEBUG] " << message << std::endl;    
    #endif
}

template<typename T>
std::string to_string_hex(const T& value) {
    if constexpr (std::is_integral_v<T>) { // Solo tipi interi
        std::ostringstream oss;
        oss << "0x" 
            << std::hex << std::setw(sizeof(T)*2) << std::setfill('0') 
            << +value; // +value promuove i tipi piccoli
        return oss.str();
    }
    else if constexpr (std::is_same_v<T, std::variant<std::monostate, uint8_t, uint16_t, uint32_t, uint64_t, std::array<uint8_t, 64>, std::array<uint8_t, 16>, std::array<uint8_t, 128>>>) {
        return std::visit([](auto&& arg) {
            using U = std::decay_t<decltype(arg)>;
            if constexpr (std::is_integral_v<U>) {
                std::ostringstream oss;
                oss << "0x" << std::hex << +arg;
                return oss.str();
            } else {
                return std::string("[array/monostate]");
            }
        }, value);
    }
    else 
    {
        return std::to_string(value); // fallback per altri tipi
    }
}

#endif // HELPERS_HPP
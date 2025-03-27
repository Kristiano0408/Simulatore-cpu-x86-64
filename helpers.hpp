#ifndef HELPERS_HPP
#define HELPERS_HPP
#include<cstdint> 
#include<string>


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


const std::string decodeRegisterReg(uint8_t reg, uint8_t rexprefix);

const std::string decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB);

const std::string decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB);

const std::string decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB);

template<typename T>
T castTo(uint64_t value) {

    static_assert(std::is_integral<T>::value, "T must be an integral type");
    return static_cast<T>(value);
}


#endif // HELPERS_HPP
#ifndef HELPERS_HPP
#define HELPERS_HPP
#include<cstdint> 
#include<string>


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


std::string decodeRegisterReg(uint8_t reg, uint8_t rexprefix);

std::string decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB);

std::string decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB);

std::string decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB);


#endif // HELPERS_HPP
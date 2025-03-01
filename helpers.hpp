#ifndef HELPERS_HPP
#define HELPERS_HPP
#include<cstdint> 


bool isPrefix(unsigned char byte); // Dichiarazione della funzione

bool isMoveInstruction(uint32_t opcode);

bool isMoveInstructionIO(uint32_t opcode);

bool isMoveInstructionR_M(uint32_t opcode);

bool isMoveR_M_reg_mem(uint32_t opcode);

bool isMoveR_M_mem_reg(uint32_t opcode);

#endif // HELPERS_HPP
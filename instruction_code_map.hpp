#ifndef INSTRUCTION_CODE_MAP_HPP
#define INSTRUCTION_CODE_MAP_HPP

#include <cstdint>
#include <unordered_map>

enum class MOVType
{
    MOV_MR,  //move register to R/M
    MOV_RM,  //move R/M to register
    MOV_MI,  //move immediate to memory/register
    MOV_OI,  //move immediate to reg
    MOV_FD,  //move from offset to Rax
    MOV_TD,  //move from Rax to offset

};


extern std::unordered_map<uint32_t, MOVType> move_instruction;


#endif // INSTRUCTION_CODE_MAP_HPP
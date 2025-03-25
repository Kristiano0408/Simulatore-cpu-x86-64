#include <unordered_map>
#include <cstdint>
#include "instruction_code_map.hpp"


std::unordered_map<uint32_t, MOVType> move_instruction =
{
    {0xB0, MOVType::MOV_OI},
    {0xB1, MOVType::MOV_OI},
    {0xB2, MOVType::MOV_OI},
    {0xB3, MOVType::MOV_OI},
    {0xB4, MOVType::MOV_OI},
    {0xB5, MOVType::MOV_OI},
    {0xB6, MOVType::MOV_OI},
    {0xB7, MOVType::MOV_OI},
    {0xB8, MOVType::MOV_OI},
    {0xB9, MOVType::MOV_OI},
    {0xBA, MOVType::MOV_OI},
    {0xBB, MOVType::MOV_OI},
    {0xBC, MOVType::MOV_OI},
    {0xBD, MOVType::MOV_OI},
    {0xBE, MOVType::MOV_OI},
    {0xBF, MOVType::MOV_OI},
    {0x88, MOVType::MOV_MR},
    {0x89, MOVType::MOV_MR},
    {0x8A, MOVType::MOV_RM},
    {0x8B, MOVType::MOV_RM},
    {0xC6, MOVType::MOV_MI},
    {0xC7, MOVType::MOV_MI},
    {0xA0, MOVType::MOV_FD},
    {0xA1, MOVType::MOV_FD},
    {0xA2, MOVType::MOV_TD},
    {0xA3, MOVType::MOV_TD}

};
#ifndef PIPELINE_TYPES_HPP
#define PIPELINE_TYPES_HPP

#include <cstdint>

enum class StageStatus : uint8_t 
{
    READY,
    MEMORY_DONE,
    STALLED,
    WAITING_MEMORY,
    WAITING_SRC_OPERAND,
    WAITING_DEST_OPERAND,
    FLUSHED,
    EMPTY,
    WAITING_GUI_BUFFER1,
    WAITING_GUI_EXECUTION
    
    

    
};

enum class StageType : uint8_t 
{
    FETCH = 0,
    DECODE = 1,
    OPERAND_FETCH = 2,
    EXECUTE = 3,
    MEMORY = 4,
    WRITE_BACK = 5,
    COUNT = 6
};


#endif // PIPELINE_TYPES_HPP
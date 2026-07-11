#ifndef PIPELINE_TYPES_HPP
#define PIPELINE_TYPES_HPP

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
    FETCH,
    DECODE,
    OPERAND_FETCH,
    EXECUTE,
    MEMORY,
    WRITE_BACK
};


#endif // PIPELINE_TYPES_HPP
#ifndef OPERAND_ENGINE_HPP
#define OPERAND_ENGINE_HPP

#include "operands.hpp"
#include "memoryInterface.hpp"
#include "device.hpp"

class Instruction;
class CacheManager;
class CPU;

class OperandEngine : public FaultDevice
{
    public:
        OperandEngine(CacheManager& cm, CPU& c) : cacheManager(cm), cpu(c) {};

        OperandResult readOperand(Instruction* instruction, Operand* operand, void* context, void(*callback)(void* context));
        OperandResult writeOperand(Instruction* instruction, Operand* operand, uint64_t value, void* context, void(*callback)(void* context));

    
    private:
        CacheManager& cacheManager; // Assuming you have a CacheManager class to handle memory operations
        CPU& cpu; 
        DataMemoryInterface dataMemoryInterface; // Assuming you have a DataMemoryInterface class to handle memory operations
        OperandResult setRegisterValue(Operand* operand, uint64_t value);
        OperandResult getRegisterValue(Operand* operand);

        OperandResult setImmediateValue(Operand* operand, uint64_t value);
        OperandResult getImmediateValue(Operand* operand);

        OperandResult getMemoryValue(Instruction* instruction, Operand* operand, void* callbackContext, void(*callback)(void* context));
        OperandResult setMemoryValue(Instruction* instruction, Operand* operand, uint64_t value, void* callbackContext, void(*callback)(void* context));

};

#endif // OPERAND_ENGINE_HPP
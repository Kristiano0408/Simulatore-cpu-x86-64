#ifndef OPERAND_ENGINE_HPP
#define OPERAND_ENGINE_HPP

#include "operands.hpp"

class Instruction;
class CacheManager;
class CPU;

class OperandEngine
{
    public:
        OperandEngine(CacheManager& cm, CPU& c) : cacheManager(cm), cpu(c) {};

        OperandResult readOperand(Instruction* instruction, Operand* operand, std::function<void()> callback);
        OperandResult writeOperand(Instruction* instruction, Operand* operand, uint64_t value, std::function<void()> callback);

    
    private:
        CacheManager& cacheManager; // Assuming you have a CacheManager class to handle memory operations
        CPU& cpu; 
        OperandResult setRegisterValue(Operand* operand, uint64_t value);
        OperandResult getRegisterValue(Operand* operand);

        OperandResult setImmediateValue(Operand* operand, uint64_t value);
        OperandResult getImmediateValue(Operand* operand);

        OperandResult getMemoryValue(Instruction* instruction, Operand* operand, std::function<void()> callback);
        OperandResult setMemoryValue(Instruction* instruction, Operand* operand, uint64_t value, std::function<void()> callback);

};

#endif // OPERAND_ENGINE_HPP
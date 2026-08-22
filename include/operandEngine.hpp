#ifndef OPERAND_ENGINE_HPP
#define OPERAND_ENGINE_HPP

#include "operands.hpp"
#include "memoryInterface.hpp"
#include "device.hpp"

class Instruction;
class CacheManager;
class CPU;
class ExecuteEngineEventHandler;

enum class WhichOperand : uint8_t
{
    SOURCE,
    DESTINATION
};

struct OperandContextWrite
{
    Instruction* instruction;
    Operand* operand;
    uint64_t value;
};

struct OperandContextRead
{
    Instruction* instruction;
    Operand* srcOperand;
    Operand* destOperand;
};




class OperandEngine : public TickedDevice, public FaultDevice
{
    public:
        OperandEngine(CacheManager& cm, CPU& c, ExecuteEngineEventHandler& eeh) : cacheManager(cm), cpu(c), executeEngineEventHandler(eeh) {};

        void sendReadRequest(Instruction* instruction, Operand* srcOperand, Operand* destOperand);
        void sendWriteRequest(Instruction* instruction, Operand* operand, uint64_t value);

        void readOperand(Instruction* instruction, Operand* operand, WhichOperand whichoperand);
        void writeOperand(Instruction* instruction, Operand* operand, uint64_t value);


    
    private:
        CacheManager& cacheManager; // Assuming you have a CacheManager class to handle memory operations
        CPU& cpu; 
        ExecuteEngineEventHandler& executeEngineEventHandler; // Event handler for managing pipeline events
        DataMemoryInterface dataMemoryInterface; // Assuming you have a DataMemoryInterface class to handle memory operations

        FixedSizeQueueCacheFriendly<OperandContextRead, 10> readQueue; // Queue for read operations
        FixedSizeQueueCacheFriendly<OperandContextWrite, 10> writeQueue; // Queue for write operations


        void execute_operation() override; // Override of the pure virtual function from Device class

        static OperandResult setRegisterValue(Operand* operand, uint64_t value);
        static OperandResult getRegisterValue(Instruction* instruction, Operand* operand, WhichOperand whichoperand);

        static OperandResult setImmediateValue(Operand* operand, uint64_t value);
        static OperandResult getImmediateValue(Instruction* instruction, Operand* operand, WhichOperand whichoperand);

        OperandResult getMemoryValue(Instruction* instruction, Operand* operand, void* callbackContext, void(*callback)(void* context), WhichOperand whichoperand);
        OperandResult setMemoryValue(Instruction* instruction, Operand* operand, uint64_t value, void* callbackContext, void(*callback)(void* context));

};

#endif // OPERAND_ENGINE_HPP
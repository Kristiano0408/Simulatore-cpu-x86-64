#ifndef EXECUTE_ENGINE_HPP
#define EXECUTE_ENGINE_HPP

#include "operandEngine.hpp"
#include "registerFile.hpp"

#include "cache/cacheManager.hpp"
#include "instruction.hpp"
#include "device.hpp"
#include "helpers.hpp"
#include "eventHandler.hpp"

class CPU;
class ALU;


class ExecuteEngine : public TickedDevice, public FaultDevice
{
    private:
        CPU& cpu;
        RegisterFile& registerFile;
        ALU& alu;
        CacheManager& cacheManager;
        OperandEngine operandEngine;
        ExecuteEngineEventHandler executeEngineEventHandler; // Event handler for managing pipeline events
        PipelineEventHandler* pipelineEventHandler; // Event handler for managing pipeline events
        FixedSizeQueueCacheFriendly<Instruction*, 10> operandFetchQueue; // Queue for operand fetch operations
        FixedSizeQueueCacheFriendly<Instruction*, 10> executionQueue; // Queue for execution operations
        FixedSizeQueueCacheFriendly<Instruction*, 10> memoryAccessQueue; // Queue for memory access operations
        FixedSizeQueueCacheFriendly<Instruction*, 10> writeBackQueue; // Queue for write-back operations
        bool isExecuting = false; // Flag to indicate if an instruction is currently being executed
        bool operandEngineBusy = false; // Flag to indicate if the operand engine is busy processing an operation
        bool isAccessingMemory = false; // Flag to indicate if memory access is currently in progress

        
        void fetchOperands(Instruction* instruction);
        void startExecution(Instruction* instruction);
        //void updateExecution(Instruction* instruction, PipelineEventHandler& eventHandler);
        void requestMemoryAccess(Instruction* instruction);
        void accessMemory(Instruction* instruction);
        void writeBackInstruction(Instruction* instruction);

        

        

        void execute_operation() override; // Override of the pure virtual function from Device class


    public:
        ExecuteEngine(CPU& cpu, RegisterFile& registerFile, ALU& alu, CacheManager& cacheManager, PipelineEventHandler* pipelineEventHandler);

        void tick() override; //tick the execute engine for the current cycle
        void setPipelineEventHandler(PipelineEventHandler* handler) { pipelineEventHandler = handler; }
        PipelineEventHandler* getPipelineEventHandler() const { return pipelineEventHandler; }
        static void triggerPipelineMemoryWaitingExecuteCallback(void* context);
        static void triggerPipelineMemoryDoneExecuteCallback(void* context);

        static void triggerPipelineMemoryWaitingCallback(void* context);
        static void triggerPipelineMemoryCompleteCallback(void* context);

        static void completeExecutionCallback(void* context);
        static void completeMemoryAccessCallback(void* context);

        static void completeWriteBackCallback(void* context);

        void executeInstruction(Instruction* instruction);

        void resetExecutionState();
        void resetMemoryAccessState();

        const FixedSizeQueueCacheFriendly<Instruction*, 10>& getExecutionQueue() const { return executionQueue; }
        const FixedSizeQueueCacheFriendly<Instruction*, 10>& getMemoryAccessQueue() const { return memoryAccessQueue; }

        void sendOperandFetchRequest(Instruction* instruction);
        void sendExecutionRequest(Instruction* instruction);
        void sendMemoryAccessRequest(Instruction* instruction);
        void sendWriteBackRequest(Instruction* instruction);

        
       



       


};

#endif // EXECUTE_ENGINE_HPP
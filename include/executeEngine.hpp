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
        PipelineEventHandler& pipelineEventHandler; // Event handler for managing pipeline events
        FixedSizeQueueCacheFriendly<ExecutionPayload, 10> operandFetchQueue; // Queue for operand fetch operations
        FixedSizeQueueCacheFriendly<ExecutionPayload, 10> executionQueue; // Queue for execution operations
        FixedSizeQueueCacheFriendly<ExecutionPayload, 10> memoryAccessQueue; // Queue for memory access operations
        FixedSizeQueueCacheFriendly<ExecutionPayload, 10> writeBackQueue; // Queue for write-back operations
        bool isExecuting = false; // Flag to indicate if an instruction is currently being executed
        bool operandEngineBusy = false; // Flag to indicate if the operand engine is busy processing an operation
        bool isAccessingMemory = false; // Flag to indicate if memory access is currently in progress

        
        void fetchOperands(Instruction* instruction);
        void startExecution(Instruction* instruction);
        //void updateExecution(Instruction* instruction, PipelineEventHandler& eventHandler);
        void requestMemoryAccess(Instruction* instruction);
        ///void accessMemory(Instruction* instruction, PipelineEventHandler& eventHandler);
        void writeBackInstruction(Instruction* instruction);

        

        

        void execute_operation() override; // Override of the pure virtual function from Device class


    public:
        ExecuteEngine(CPU& cpu, RegisterFile& registerFile, ALU& alu, CacheManager& cacheManager, PipelineEventHandler& pipelineEventHandler)
            : cpu(cpu), registerFile(registerFile), alu(alu), cacheManager(cacheManager), operandEngine(cacheManager, cpu), pipelineEventHandler(pipelineEventHandler)
        {}

        static void triggerPipelineMemoryWaitingExecuteCallback(void* context);
        static void triggerPipelineMemoryDoneExecuteCallback(void* context);
        static void triggerPipelineExecuteCompleteCallback(void* context);

        static void triggerPipelineMemoryWaitingCallback(void* context);
        static void triggerPipelineMemoryCompleteCallback(void* context);

        static void completeExecutionCallback(void* context);
        static void resetMemoryAccessStateWrapper(void* context);

        void executeInstruction(Instruction* instruction);

        void resetExecutionState();
        void resetMemoryAccessState();

        const FixedSizeQueueCacheFriendly<Instruction*, 10>& getExecutionQueue() const { return executionQueue; }

        void sendOperandFetchRequest(Instruction* instruction, PipelineEventHandler& eventHandler);
        void sendExecutionRequest(Instruction* instruction, PipelineEventHandler& eventHandler);
        void sendMemoryAccessRequest(Instruction* instruction, PipelineEventHandler& eventHandler);
        void sendWriteBackRequest(Instruction* instruction, PipelineEventHandler& eventHandler);

        
       



       


};

#endif // EXECUTE_ENGINE_HPP
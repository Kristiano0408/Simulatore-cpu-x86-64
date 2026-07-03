#ifndef EXECUTE_ENGINE_HPP
#define EXECUTE_ENGINE_HPP

#include "operandEngine.hpp"
#include "registerFile.hpp"

#include "cacheManager.hpp"
#include "instruction_wrapper.hpp"

class CPU;
class ALU;

class ExecuteEngine
{
    private:
        CPU& cpu;
        RegisterFile& registerFile;
        ALU& alu;
        CacheManager& cacheManager;
        OperandEngine operandEngine;

        void executeInstruction(Instruction* instruction);

    public:
        ExecuteEngine(CPU& cpu, RegisterFile& registerFile, ALU& alu, CacheManager& cacheManager)
            : cpu(cpu), registerFile(registerFile), alu(alu), cacheManager(cacheManager), operandEngine(cacheManager, cpu)
        {}

        void fetchOperands(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler);
        void startExecution(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler);
        void updateExecution(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler);
        void requestMemoryAccess(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler);
        void accessMemory(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler);
        void writeBackInstruction(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler);



       


};

#endif // EXECUTE_ENGINE_HPP
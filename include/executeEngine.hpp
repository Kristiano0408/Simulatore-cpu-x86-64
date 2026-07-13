#ifndef EXECUTE_ENGINE_HPP
#define EXECUTE_ENGINE_HPP

#include "operandEngine.hpp"
#include "registerFile.hpp"

#include "cache/cacheManager.hpp"
#include "instruction.hpp"
#include "device.hpp"

class CPU;
class ALU;

class ExecuteEngine : public FaultDevice
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

        void fetchOperands(Instruction* instruction, PipelineEventHandler& eventHandler);
        void startExecution(Instruction* instruction, PipelineEventHandler& eventHandler);
        void updateExecution(Instruction* instruction, PipelineEventHandler& eventHandler);
        void requestMemoryAccess(Instruction* instruction, PipelineEventHandler& eventHandler);
        void accessMemory(Instruction* instruction, PipelineEventHandler& eventHandler);
        void writeBackInstruction(Instruction* instruction, PipelineEventHandler& eventHandler);



       


};

#endif // EXECUTE_ENGINE_HPP
#ifndef EXECUTE_ENGINE_HPP
#define EXECUTE_ENGINE_HPP

#include "operandEngine.hpp"
#include "registerFile.hpp"

#include "cache/cacheManager.hpp"
#include "device.hpp"
#include "eventHandler.hpp"
#include "instruction.hpp"

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
    ExecuteEngineEventHandler executeEngineEventHandler;             // Event handler for managing pipeline events
    PipelineEventHandler* pipelineEventHandler;                      // Event handler for managing pipeline events
    FixedSizeQueueCacheFriendly<Instruction*, 10> operandFetchQueue; // Queue for operand fetch operations
    FixedSizeQueueCacheFriendly<Instruction*, 10> executionQueue;    // Queue for execution operations
    FixedSizeQueueCacheFriendly<Instruction*, 10> memoryAccessQueue; // Queue for memory access operations
    FixedSizeQueueCacheFriendly<Instruction*, 10> writeBackQueue;    // Queue for write-back operations
    bool isExecuting = false;                                        // Flag to indicate if an instruction is currently being executed
    bool operandEngineBusy = false;                                  // Flag to indicate if the operand engine is busy processing an operation
    bool isAccessingMemory = false;                                  // Flag to indicate if memory access is currently in progress

    void fetchOperands(Instruction* instruction);
    void startExecution(Instruction* instruction);
    // void updateExecution(Instruction* instruction, PipelineEventHandler& eventHandler);
    void requestMemoryAccess(Instruction* instruction);
    void accessMemory(Instruction* instruction);
    void writeBackInstruction(Instruction* instruction);

    void execute_operation() override; // Override of the pure virtual function from Device class
    
    void updateTemporaryValuesForInstruction(Instruction* instruction, RegisterFile& regFile);
    void executeDataTransferInstruction(Instruction* instruction);
public:
    ExecuteEngine(CPU& cpuRef, RegisterFile& regFile, ALU& aLu, CacheManager& cManager, PipelineEventHandler* pEventHandler);

    void tick() override; // tick the execute engine for the current cycle
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


namespace tempValuesInit {

    using TemporaryValuesFunc = void (*)(temporaryValues&, RegisterFile&);

    void initNone(temporaryValues& tempValues, RegisterFile& regfile);
    void initDestReady(temporaryValues& tempValues, RegisterFile& regfile);
    void initSrcReady(temporaryValues& tempValues, RegisterFile& regfile);
    void initCarryIn(temporaryValues& tempValues, RegisterFile& regfile);


    constexpr  std::array<TemporaryValuesFunc, static_cast<size_t>(TypeofInstruction::COUNT)> temporaryValuesInit {
        /*MOV*/    initDestReady,
        /*ADD*/    initNone,
        /*SUB*/    initNone,
        /*LEA*/    initNone,
        /*ADC*/    initCarryIn,
        /*SBB*/    initCarryIn,
        /*INC*/    initNone,
        /*DEC*/    initNone,
        /*NEG*/    initNone,
        /*CMP*/    initNone,
        /*MUL*/    initNone,
        /*IMUL*/   initNone,
        /*DIV*/    initNone,
        /*IDIV*/   initNone,
        /*MOVZX*/  initDestReady,
        /*MOVSX*/  initDestReady,
        /*MOVSXD*/ initDestReady,
        /*PUSH*/   initDestReady,
        /*POP*/    initDestReady,
        /*AND*/    initNone,
        /*OR*/     initNone,
        /*XOR*/    initNone,
        /*NOT*/    initSrcReady,
        /*TEST*/   initNone,
        /*SALSHL*/ initNone,  
        /*SAR*/    initNone,
        /*SHR*/    initNone,
        /*RCL*/    initCarryIn,
        /*RCR*/    initCarryIn,
        /*ROL*/    initNone,
        /*ROR*/    initNone,
        /*JMP*/    initDestReady

    };
    


}

namespace dataTransferexecution {

    using DataTransferFunc = void(*)(Instruction*);

    void executeMOVandLEA(Instruction* instruction);
    void executeMOVSXandMOVSXD(Instruction* instruction);
    void executeMOVZX(Instruction* instruction);
    void executePUSH(Instruction* instruction);
    void executePOP(Instruction* instruction);


    constexpr std::array<DataTransferFunc, static_cast<size_t>(TypeofDataTransferInstruction::COUNT)> dataTransferExecutionFunc {
        /*MOV*/    executeMOVandLEA,
        /*MOVSX*/  executeMOVSXandMOVSXD,
        /*MOVZX*/  executeMOVZX,
        /*MOVSXD*/ executeMOVSXandMOVSXD,
        /*LEA*/    executeMOVandLEA,
        /*PUSH*/   executePUSH,
        /*POP*/    executePOP
    };


}


#endif // EXECUTE_ENGINE_HPP
#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "eventHandler.hpp"
#include "executeEngine.hpp"
#include "helpers/pipelineTypes.hpp"
#include "pipelineScheduler.hpp"
#include <memory>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Base Stage class

class Stage
{
public:
    virtual ~Stage() = default;

    bool isStageReady() const { return status == StageStatus::READY; }

    static bool isInstructionEmpty(Instruction* instr);

    StageStatus getStatus() const { return status; }

    void setStatus(StageStatus newStatus) { status = newStatus; }

    bool isStalledGUI() const { return StalledGUI; }

    void setStalledGUI(bool stalled) { StalledGUI = stalled; }

private:
    bool StalledGUI = false; // Flag to indicate if the stage is stalled due to GUI interaction
    StageStatus status = StageStatus::READY;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////7

// FetchStage class

class FetchStage : public Stage
{

public:
    FetchStage() : Stage()  {}

    ~FetchStage() {};

    InstructionInfo getCurrentInstructionInfo() const { return currentInstructionInfo; }

    void setCurrentInstructionInfo(InstructionInfo info) { currentInstructionInfo = info; }

private:
    // any additional members specific to the fetch stage
    InstructionInfo currentInstructionInfo; // information about the current instruction being fetched
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DecodeStage class

class DecodeStage : public Stage
{

public:
    DecodeStage() : Stage(), decoded_instruction(std::make_unique<Instruction>()) {}

    ~DecodeStage() {};

     void setInstructionToDecode(const InstructionInfo& info) { instruction_info_to_decode = info; }

    InstructionInfo getInstructionToDecode() const { return instruction_info_to_decode; }

    InstructionInfo& getInstructionToDecodeRef() { return instruction_info_to_decode; }

    std::unique_ptr<Instruction> getDecodedInstruction() { return std::move(decoded_instruction); }

    std::unique_ptr<Instruction>& getDecodedInstructionRef() { return decoded_instruction; }

    Instruction* peekInstruction() const { return decoded_instruction.get(); }

private:
    // any additional members specific to the decode stage
    InstructionInfo instruction_info_to_decode; // information about the instruction being decoded

    std::unique_ptr<Instruction> decoded_instruction; // pointer to the decoded instruction
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OperandFetchStage class

class OperandFetchStage : public Stage
{

public:
    OperandFetchStage() : Stage(), instruction_with_fetched_operands(std::make_unique<Instruction>()) {}

    ~OperandFetchStage() {};

    std::unique_ptr<Instruction> getInstructionWithFetchedOperands()
    {
        return std::move(instruction_with_fetched_operands);
    }

    void setInstructionWithFetchedOperands(std::unique_ptr<Instruction> instruction)
    {
        instruction_with_fetched_operands = std::move(instruction);
    }

    Instruction& peekInstructionRef() const { return *instruction_with_fetched_operands; }
    Instruction* peekInstruction() const { return instruction_with_fetched_operands.get(); }

private:
    // any additional members specific to the operand fetch stage
    std::unique_ptr<Instruction> instruction_with_fetched_operands; // pointer to the instruction with fetched operands
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ExecuteStage class

class ExecuteStage : public Stage
{

public:
    ExecuteStage() : Stage(), instruction_to_execute(std::make_unique<Instruction>()) {}

    ~ExecuteStage() {};

     void setInstructionToExecute(std::unique_ptr<Instruction> instruction)
    {
        instruction_to_execute = std::move(instruction);
    }

    std::unique_ptr<Instruction> getInstructionToExecute() { return std::move(instruction_to_execute); }

    Instruction& peekInstructionRef() const { return *instruction_to_execute; }
    Instruction* peekInstruction() const { return instruction_to_execute.get(); }

private:
    // any additional members specific to the execute stage
    std::unique_ptr<Instruction> instruction_to_execute; // pointer to the instruction being executed
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MemoryStage class

class MemoryStage : public Stage
{

public:
    MemoryStage() : Stage(), instruction_to_memory(std::make_unique<Instruction>()) {}

    ~MemoryStage() {};

    void setInstructionToMemory(std::unique_ptr<Instruction> instruction)
    {
        instruction_to_memory = std::move(instruction);
    }

    std::unique_ptr<Instruction> getInstructionToMemory() { return std::move(instruction_to_memory); }

    Instruction* peekInstruction() const { return instruction_to_memory.get(); }
    Instruction& peekInstructionRef() const { return *instruction_to_memory; }

private:
    // any additional members specific to the memory stage
    std::unique_ptr<Instruction> instruction_to_memory; // pointer to the instruction being processed in memory stage
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// WriteBackStage class

class WriteBackStage : public Stage
{

public:
    WriteBackStage() : Stage(), instruction_to_writeback(std::make_unique<Instruction>()) {}

    ~WriteBackStage() {};

    void setInstructionToWriteBack(std::unique_ptr<Instruction> instruction)
    {
        instruction_to_writeback = std::move(instruction);
    }

    std::unique_ptr<Instruction> getInstructionToWriteBack() { return std::move(instruction_to_writeback); }

    Instruction* peekInstruction() const { return instruction_to_writeback.get(); }
    Instruction& peekInstructionRef() const { return *instruction_to_writeback; }

private:
    // any additional members specific to the write-back stage
    std::unique_ptr<Instruction> instruction_to_writeback;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// buffers between stages

struct FetchDecodeBuffer
{

    bool valid{};
    bool stalled{};
    bool flushed{};
    InstructionInfo instructionInfo;

};

struct DecodeOperandFetchBuffer
{

    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> decodedInstruction;
    Instruction* peekInstruction() const { return decodedInstruction.get(); }

    DecodeOperandFetchBuffer()
        : valid(false), stalled(false), flushed(false), decodedInstruction(std::make_unique<Instruction>())
    {
    }
};

struct OperandFetchExecuteBuffer
{

    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> instructionWithOperands;
    Instruction* peekInstruction() const { return instructionWithOperands.get(); }

    OperandFetchExecuteBuffer()
        : valid(false), stalled(false), flushed(false), instructionWithOperands(std::make_unique<Instruction>())
    {
    }
};

struct ExecuteMemoryBuffer
{

    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> executedInstruction;
    Instruction* peekInstruction() const { return executedInstruction.get(); }

    ExecuteMemoryBuffer()
        : valid(false), stalled(false), flushed(false), executedInstruction(std::make_unique<Instruction>())
    {
    }
};

struct MemoryWriteBackBuffer
{

    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> memoryAccessedInstruction;
    Instruction* peekInstruction() const { return memoryAccessedInstruction.get(); }

    MemoryWriteBackBuffer()
        : valid(false), stalled(false), flushed(false), memoryAccessedInstruction(std::make_unique<Instruction>())
    {
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pipeline class

class Pipeline : public TickedDevice, public FaultDevice
{

public:
    Pipeline(CPU& cpuRef, PipelineEventHandler* eventHandlerPtr);

    ~Pipeline() {};

    void execute_operation() override; // execute the operation for the current cycle

    void tick() override; // tick the pipeline for the current cycle

    FetchStage& getFetchStage() { return fetchStage; }
    DecodeStage& getDecodeStage() { return decodeStage; }
    OperandFetchStage& getOperandFetchStage() { return operandFetchStage; }
    ExecuteStage& getExecuteStage() { return executeStage; }
    MemoryStage& getMemoryStage() { return memoryStage; }
    WriteBackStage& getWriteBackStage() { return writeBackStage; }

    Stage* getStage(StageType stageType) { return stageMap[static_cast<size_t>(stageType)]; }

    FetchDecodeBuffer& getFetchDecodeBuffer() { return fetchDecodeBuffer; }
    DecodeOperandFetchBuffer& getDecodeOperandFetchBuffer() { return decodeOperandFetchBuffer; }
    OperandFetchExecuteBuffer& getOperandFetchExecuteBuffer() { return operandFetchExecuteBuffer; }
    ExecuteMemoryBuffer& getExecuteMemoryBuffer() { return executeMemoryBuffer; }
    MemoryWriteBackBuffer& getMemoryWriteBackBuffer() { return memoryWriteBackBuffer; }

    void setEventHandler(PipelineEventHandler* handler);

private:
    CPU& cpu;
    PipelineScheduler pipelineScheduler;
    ExecuteEngine executeEngine;
    FetchStage fetchStage;
    DecodeStage decodeStage;
    OperandFetchStage operandFetchStage;
    ExecuteStage executeStage;
    MemoryStage memoryStage;
    WriteBackStage writeBackStage;

    // buffer between stages
    FetchDecodeBuffer fetchDecodeBuffer;
    DecodeOperandFetchBuffer decodeOperandFetchBuffer;
    OperandFetchExecuteBuffer operandFetchExecuteBuffer;
    ExecuteMemoryBuffer executeMemoryBuffer;
    MemoryWriteBackBuffer memoryWriteBackBuffer;

    std::array<Stage*, static_cast<size_t>(StageType::COUNT)> stageMap = {
        &fetchStage, &decodeStage, &operandFetchStage, &executeStage, &memoryStage, &writeBackStage};

    friend class PipelineScheduler; // Allow PipelineScheduler to access private members of Pipeline

    // event handler for pipeline events with callbacks from pipeline controller
    PipelineEventHandler* eventHandler;
};

#endif // PIPELINE_HPP
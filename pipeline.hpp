#ifndef PIPELINE_HPP
#define PIPELINE_HPP


#include <cstdint>
#include <array>
#include <string>
#include <memory>
#include "instruction.hpp"
#include "helpers.hpp"

class Bus; // Forward declaration of Bus class

class Stage
{
    public:
        Stage();
        virtual ~Stage()=default;
        bool isStageReady() const;

        StageStatus getStatus() const;

        void setStatus(StageStatus newStatus);

    private:
        StageStatus status = StageStatus::READY;

};

class FetchStage : public Stage {
    public:
        FetchStage();
        ~FetchStage();

        void startFetch(Bus& bus, uint64_t instructionId, uint64_t& index); //fetch the instruction from memory

        void updateFetch(Bus& bus, uint64_t instructionId); //update the fetch stage (take the instruction fetched and prepare for decode)

        InstructionInfo fetchInstruction(Bus& bus, uint64_t instructionId, uint64_t& index); //fetch the instruction from memory

        InstructionInfo getCurrentInstructionInfo() const;

        void setCurrentInstructionInfo(InstructionInfo info);

    private:
        //any additional members specific to the fetch stage
        InstructionInfo currentInstructionInfo; //information about the current instruction being fetched
};

class DecodeStage : public Stage {
    public:
        DecodeStage();
        ~DecodeStage();

        void setInstructionToDecode(const InstructionInfo& info);
        InstructionInfo getInstructionToDecode() const;

        void decodeInstruction(Bus& bus); //decode the fetched instruction

        std::unique_ptr<Instruction> getDecodedInstruction();

    private:
        //any additional members specific to the decode stage
        InstructionInfo  instruction_info_to_decode; //information about the instruction being decoded

        std::unique_ptr<Instruction> decoded_instruction; //pointer to the decoded instruction
};

class OperandFetchStage : public Stage {
    public:
        OperandFetchStage();
        ~OperandFetchStage();

        void fetchOperands(Bus& bus); //fetch operands for the decoded instruction

        std::unique_ptr<Instruction> getInstructionWithFetchedOperands();
        void setInstructionWithFetchedOperands(std::unique_ptr<Instruction> instruction);


    private:
        //any additional members specific to the operand fetch stage
        std::unique_ptr<Instruction> instruction_with_fetched_operands; //pointer to the instruction with fetched operands
};

class ExecuteStage : public Stage {
    public:
        ExecuteStage();
        ~ExecuteStage();

        void setInstructionToExecute(std::unique_ptr<Instruction> instruction);
        std::unique_ptr<Instruction> getInstructionToExecute();

        void executeInstruction(Bus& bus); //execute the decoded instruction

        uint64_t getExecutionResult() const;

        bool wasExecutionSuccessful() const;

    private:
        //any additional members specific to the execute stage
        std::unique_ptr<Instruction> instruction_to_execute; //pointer to the instruction being executed

        uint64_t executionResult; //result of the instruction execution

        bool executionSuccess; //flag indicating if the execution was successful


};

class MemoryStage : public Stage {
    public:
        MemoryStage();
        ~MemoryStage();


        void setInstructionToMemory(std::unique_ptr<Instruction> instruction);
        std::unique_ptr<Instruction> getInstructionToMemory();

        void accessMemory(Bus& bus); //perform memory operations if needed

        uint64_t getMemoryData() const;

        bool wasMemoryAccessSuccessful() const;

    private:
        //any additional members specific to the memory stage
        std::unique_ptr<Instruction> instruction_to_memory; //pointer to the instruction being processed in memory stage

        bool memoryAccessSuccess; //flag indicating if the memory access was successful

};

class WriteBackStage : public Stage {
    public:
        WriteBackStage();
        ~WriteBackStage();

          void setInstructionToWriteBack(std::unique_ptr<Instruction> instruction);
        std::unique_ptr<Instruction> getInstructionToWriteBack();

        void writeBack(Bus& bus); //final stage: write results to registers/memory

        bool wasWriteBackSuccessful() const;


    private:
        //any additional members specific to the write-back stage
         std::unique_ptr<Instruction> instruction_to_writeback;
        bool writeBackSuccess;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FetchDecodeBuffer {
    InstructionInfo instructionInfo;
    bool valid = false;
    bool stalled = false;
    bool flushed = false;

};

struct DecodeOperandFetchBuffer {
    std::unique_ptr<Instruction> decodedInstruction;
    bool valid = false;
    bool stalled = false;
    bool flushed = false;

    DecodeOperandFetchBuffer() : valid(false), stalled(false), flushed(false), decodedInstruction(std::make_unique<EmptyInstruction>()) {}

};

struct OperandFetchExecuteBuffer {
    std::unique_ptr<Instruction> instructionWithOperands;
    bool valid = false;
    bool stalled = false;
    bool flushed = false;

    OperandFetchExecuteBuffer() : valid(false), stalled(false), flushed(false), instructionWithOperands(std::make_unique<EmptyInstruction>()) {}

};

struct ExecuteMemoryBuffer {
    std::unique_ptr<Instruction> executedInstruction;
    bool valid = false;
    bool stalled = false;
    bool flushed = false;

    ExecuteMemoryBuffer() : valid(false), stalled(false), flushed(false), executedInstruction(std::make_unique<EmptyInstruction>()) {}

};

struct MemoryWriteBackBuffer {
    std::unique_ptr<Instruction> memoryAccessedInstruction;
    bool valid = false;
    bool stalled = false;
    bool flushed = false;

    MemoryWriteBackBuffer() : valid(false), stalled(false), flushed(false), memoryAccessedInstruction(std::make_unique<EmptyInstruction>()) {}

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class Pipeline : public Device {
    public:
        Pipeline(Bus& bus);
        ~Pipeline();

        void execute_operation() override; //execute the operation for the current cycle

        FetchStage& getFetchStage();
        DecodeStage& getDecodeStage();
        OperandFetchStage& getOperandFetchStage();
        ExecuteStage& getExecuteStage();
        MemoryStage& getMemoryStage();
        WriteBackStage& getWriteBackStage();

    private:
        Bus& bus;
        FetchStage fetchStage;
        DecodeStage decodeStage;
        OperandFetchStage operandFetchStage;
        ExecuteStage executeStage;
        MemoryStage memoryStage;
        WriteBackStage writeBackStage;

        //buffer between stages 
        FetchDecodeBuffer fetchDecodeBuffer;
        DecodeOperandFetchBuffer decodeOperandFetchBuffer;
        OperandFetchExecuteBuffer operandFetchExecuteBuffer;
        ExecuteMemoryBuffer executeMemoryBuffer;
        MemoryWriteBackBuffer memoryWriteBackBuffer;

        // Tracking instruction IDs in each stage

        uint64_t FetchstageInstructionId {};
        uint64_t DecodeStageInstructionId {};
        uint64_t OperandFetchStageInstructionId {};
        uint64_t ExecuteStageInstructionId {};
        uint64_t MemoryStageInstructionId {};
        uint64_t WriteBackStageInstructionId {};

        //necessary varaible for fetching instruction
        uint64_t index = 0; //index for fetching instruction from memory (in future could be part of fetch stage)

        





};










#endif // PIPELINE_HPP
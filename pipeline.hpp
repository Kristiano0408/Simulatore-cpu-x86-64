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

        void setBusy(bool busy);
        bool isBusy() const;

        void setStalled(bool stalled);
        bool isStalled() const;

        void setFlushed(bool flushed);
        bool isFlushed() const;

    private:
        bool busy;
        bool stalled;
        bool flushed;

};

class FetchStage : public Stage {
    public:
        FetchStage();
        ~FetchStage();

        void fetchInstruction(Bus& bus); //fetch the instruction from memory

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

        Instruction* getDecodedInstruction() const;

    private:
        //any additional members specific to the decode stage
        InstructionInfo  instruction_info_to_decode; //information about the instruction being decoded

        std::unique_ptr<Instruction> decoded_instruction; //pointer to the decoded instruction
};

class ExecuteStage : public Stage {
    public:
        ExecuteStage();
        ~ExecuteStage();

        void setInstructionToExecute(Instruction* instruction);
        Instruction* getInstructionToExecute() const;

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


        void setInstructionToMemory(Instruction* instruction);
        Instruction* getInstructionToMemory() const;

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

          void setInstructionToWriteBack(Instruction* instruction);
        Instruction* getInstructionToWriteBack() const;

        void writeBack(Bus& bus); //final stage: write results to registers/memory

       
        bool wasWriteBackSuccessful() const;

    private:
        //any additional members specific to the write-back stage
         std::unique_ptr<Instruction> instruction_to_writeback;
        bool writeBackSuccess;
};


class Pipeline : public Device {
    public:
        Pipeline(Bus& bus);
        ~Pipeline();

        void tick() override; //advance the pipeline by one clock cycle

        void execute_operation() override; //execute the operation for the current cycle

        FetchStage& getFetchStage();
        DecodeStage& getDecodeStage();
        ExecuteStage& getExecuteStage();
        MemoryStage& getMemoryStage();
        WriteBackStage& getWriteBackStage();

    private:
        Bus& bus;
        FetchStage fetchStage;
        DecodeStage decodeStage;
        ExecuteStage executeStage;
        MemoryStage memoryStage;
        WriteBackStage writeBackStage;




};










#endif // PIPELINE_HPP
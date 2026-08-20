#ifndef PIPELINE_HPP
#define PIPELINE_HPP


#include <cstdint>
#include <array>
#include <string>
#include <memory>
#include "helpers.hpp"
#include "eventHandler.hpp"
#include "executeEngine.hpp"
#include "pipelineScheduler.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Base Stage class

class Stage
{
    public:

        Stage() {};

        virtual ~Stage() = default;

        inline bool isStageReady() const {return status == StageStatus::READY; }

        bool isInstructionEmpty(Instruction* instr) const;

        inline StageStatus getStatus() const { return status; }

        inline void setStatus(StageStatus newStatus) { status = newStatus; }

        inline bool isStalledGUI() const { return StalledGUI; }

        inline void setStalledGUI(bool stalled) { StalledGUI = stalled; }

    private:

        bool StalledGUI = false; // Flag to indicate if the stage is stalled due to GUI interaction
        StageStatus status = StageStatus::READY;

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////7  

// FetchStage class

class FetchStage : public Stage {

    public:

        FetchStage();

        ~FetchStage() {};

        inline InstructionInfo getCurrentInstructionInfo() const { return currentInstructionInfo; }

        inline void setCurrentInstructionInfo(InstructionInfo info) { currentInstructionInfo = info; }

    private:
        //any additional members specific to the fetch stage
        InstructionInfo currentInstructionInfo; //information about the current instruction being fetched
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DecodeStage class

class  DecodeStage : public Stage {

    public:

        DecodeStage();

        ~DecodeStage() {};

        inline void setInstructionToDecode(const InstructionInfo& info) { instruction_info_to_decode = info; }
        
        inline InstructionInfo getInstructionToDecode() const { return instruction_info_to_decode; }

        inline InstructionInfo& getInstructionToDecodeRef() { return instruction_info_to_decode; }


        inline std::unique_ptr<Instruction> getDecodedInstruction() { return std::move(decoded_instruction);}

        inline std::unique_ptr<Instruction>& getDecodedInstructionRef() { return decoded_instruction; }

        inline Instruction* peekInstruction() const {return decoded_instruction.get();}

    private:
        //any additional members specific to the decode stage
        InstructionInfo  instruction_info_to_decode; //information about the instruction being decoded

        std::unique_ptr<Instruction> decoded_instruction; //pointer to the decoded instruction
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OperandFetchStage class

class OperandFetchStage : public Stage {

    public:

        OperandFetchStage();

        ~OperandFetchStage() {};

        std::unique_ptr<Instruction> getInstructionWithFetchedOperands();

        void setInstructionWithFetchedOperands(std::unique_ptr<Instruction> instruction);

        inline Instruction& peekInstructionRef() const {return *instruction_with_fetched_operands.get();}
        inline Instruction* peekInstruction() const {return instruction_with_fetched_operands.get();}


    private:
        //any additional members specific to the operand fetch stage
        std::unique_ptr<Instruction> instruction_with_fetched_operands; //pointer to the instruction with fetched operands
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ExecuteStage class

class ExecuteStage : public Stage {

    public:

        ExecuteStage();

        ~ExecuteStage() {};

        void setInstructionToExecute(std::unique_ptr<Instruction> instruction);
        std::unique_ptr<Instruction> getInstructionToExecute();
        
        inline Instruction& peekInstructionRef() const {return *instruction_to_execute.get();}
        inline Instruction* peekInstruction() const {return instruction_to_execute.get();}


    private:
        //any additional members specific to the execute stage
        std::unique_ptr<Instruction> instruction_to_execute; //pointer to the instruction being executed

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MemoryStage class

class MemoryStage : public Stage {

    public:

        MemoryStage();

        ~MemoryStage() {};

        void setInstructionToMemory(std::unique_ptr<Instruction> instruction);
        std::unique_ptr<Instruction> getInstructionToMemory();

        inline Instruction* peekInstruction() const {return instruction_to_memory.get();}
        inline Instruction& peekInstructionRef() const {return *instruction_to_memory.get();}


    private:
        //any additional members specific to the memory stage
        std::unique_ptr<Instruction> instruction_to_memory; //pointer to the instruction being processed in memory stage

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// WriteBackStage class

class WriteBackStage : public Stage {

    public:

        WriteBackStage();
        
        ~WriteBackStage() {};

        void setInstructionToWriteBack(std::unique_ptr<Instruction> instruction);
        
        std::unique_ptr<Instruction> getInstructionToWriteBack();

        inline Instruction* peekInstruction() const {return instruction_to_writeback.get();}
        inline Instruction& peekInstructionRef() const {return *instruction_to_writeback.get();}


    private:
        //any additional members specific to the write-back stage
         std::unique_ptr<Instruction> instruction_to_writeback;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//buffers between stages

struct FetchDecodeBuffer {
    
    bool valid{};
    bool stalled{};
    bool flushed{};
    InstructionInfo instructionInfo{};

    FetchDecodeBuffer() : valid(false), stalled(false), flushed(false), instructionInfo() {}
};

struct DecodeOperandFetchBuffer {
    
    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> decodedInstruction;
    Instruction* peekInstruction() const {return decodedInstruction.get();}

    DecodeOperandFetchBuffer() : valid(false), stalled(false), flushed(false), decodedInstruction(std::make_unique<Instruction>()) {}

};

struct OperandFetchExecuteBuffer {
    
    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> instructionWithOperands;
    Instruction* peekInstruction() const {return instructionWithOperands.get();}

    OperandFetchExecuteBuffer() : valid(false), stalled(false), flushed(false), instructionWithOperands(std::make_unique<Instruction>()) {}

};

struct ExecuteMemoryBuffer {
    
    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> executedInstruction;
    Instruction* peekInstruction() const {return executedInstruction.get();}

    ExecuteMemoryBuffer() : valid(false), stalled(false), flushed(false), executedInstruction(std::make_unique<Instruction>()) {}

};

struct MemoryWriteBackBuffer {

    bool valid{};
    bool stalled{};
    bool flushed{};
    std::unique_ptr<Instruction> memoryAccessedInstruction;
    Instruction* peekInstruction() const {return memoryAccessedInstruction.get();}

    MemoryWriteBackBuffer() : valid(false), stalled(false), flushed(false), memoryAccessedInstruction(std::make_unique<Instruction>()) {}

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pipeline class

class Pipeline : public TickedDevice, public FaultDevice
{

    public:

        Pipeline(CPU& cpu, PipelineEventHandler* eventHandler);

        ~Pipeline() {};

        void execute_operation() override; //execute the operation for the current cycle

        void tick() override; //tick the pipeline for the current cycle

        inline FetchStage& getFetchStage() { return fetchStage; }
        inline DecodeStage& getDecodeStage() { return decodeStage; }
        inline OperandFetchStage& getOperandFetchStage() { return operandFetchStage; }
        inline ExecuteStage& getExecuteStage() { return executeStage; }
        inline MemoryStage& getMemoryStage() { return memoryStage; }
        inline WriteBackStage& getWriteBackStage() { return writeBackStage; }

        inline Stage* getStage(StageType stageType) {return stageMap[static_cast<size_t>(stageType)]; }

        inline FetchDecodeBuffer& getFetchDecodeBuffer() { return fetchDecodeBuffer; }
        inline DecodeOperandFetchBuffer& getDecodeOperandFetchBuffer() { return decodeOperandFetchBuffer; }
        inline OperandFetchExecuteBuffer& getOperandFetchExecuteBuffer() { return operandFetchExecuteBuffer; }
        inline ExecuteMemoryBuffer& getExecuteMemoryBuffer() { return executeMemoryBuffer; }
        inline MemoryWriteBackBuffer& getMemoryWriteBackBuffer() { return memoryWriteBackBuffer; }

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

        //buffer between stages 
        FetchDecodeBuffer fetchDecodeBuffer;
        DecodeOperandFetchBuffer decodeOperandFetchBuffer;
        OperandFetchExecuteBuffer operandFetchExecuteBuffer;
        ExecuteMemoryBuffer executeMemoryBuffer;
        MemoryWriteBackBuffer memoryWriteBackBuffer;

        std::array<Stage*, static_cast<size_t>(StageType::COUNT)> stageMap = {
            &fetchStage, 
            &decodeStage, 
            &operandFetchStage, 
            &executeStage, 
            &memoryStage, 
            &writeBackStage
        };

        friend class PipelineScheduler; // Allow PipelineScheduler to access private members of Pipeline
        

        //event handler for pipeline events with callbacks from pipeline controller
        PipelineEventHandler* eventHandler;

        

        

        
};










#endif // PIPELINE_HPP
#include "pipeline.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include <iostream>


// Implementation of Stage class methods
Stage::Stage() : busy(false), stalled(false), flushed(false) {}


void Stage::setBusy(bool busy) {
    this->busy = busy;
}

bool Stage::isBusy() const {
    return busy;
}

void Stage::setStalled(bool stalled) {
    this->stalled = stalled;
}

bool Stage::isStalled() const {
    return stalled;
}

void Stage::setFlushed(bool flushed) {
    this->flushed = flushed;
}

bool Stage::isFlushed() const {
    return flushed;
}


// Implementation of FetchStage class methods
FetchStage::FetchStage() : Stage(), currentInstructionInfo{0,0,0,{0},false,0,0,0,0,0,0,0,0,false,false,false,false,false,{0},""} {}

FetchStage::~FetchStage() {}

void FetchStage::fetchInstruction(Bus& bus) {
    // Implementation of instruction fetching from memory using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    std::cout << "Fetching instruction from memory..." << std::endl;
    currentInstructionInfo = bus.getCPU().getControlUnit().fetchInstruction();
}

InstructionInfo FetchStage::getCurrentInstructionInfo() const {
    return currentInstructionInfo;
}

void FetchStage::setCurrentInstructionInfo(InstructionInfo info) {
    currentInstructionInfo = info;                                        //maybe it is not needed
}

// Implementation of DecodeStage class methods
DecodeStage::DecodeStage() : Stage(), instruction_info_to_decode{0,0,0,{0},false,0,0,0,0,0,0,0,0,false,false,false,false,false,{0},""}, decoded_instruction(nullptr) {}
DecodeStage::~DecodeStage() {}

void DecodeStage::setInstructionToDecode(const InstructionInfo& info) {
    instruction_info_to_decode = info;                                     
}

InstructionInfo DecodeStage::getInstructionToDecode() const {      //maybe it is not needed
    return instruction_info_to_decode;
}

void DecodeStage::decodeInstruction(Bus& bus) {
    // Implementation of instruction decoding using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    std::cout << "Decoding instruction..." << std::endl;
    decoded_instruction.reset(bus.getCPU().getControlUnit().decodeInstruction(instruction_info_to_decode));

    std::cout << "Decoded instruction: " << std::endl;
}

Instruction* DecodeStage::getDecodedInstruction() const {
    return decoded_instruction.get();
}


// Implementation of ExecuteStage class methods
ExecuteStage::ExecuteStage() : Stage(), instruction_to_execute(nullptr) {}

ExecuteStage::~ExecuteStage() {}

void ExecuteStage::setInstructionToExecute(Instruction* instruction) {
    instruction_to_execute.reset(instruction);
}

Instruction* ExecuteStage::getInstructionToExecute() const {
    return instruction_to_execute.get();
}

void ExecuteStage::executeInstruction(Bus& bus) {
   
}


uint64_t ExecuteStage::getExecutionResult() const {
    return executionResult;
}

bool ExecuteStage::wasExecutionSuccessful() const {
    return executionSuccess;
}

// Implementation of MemoryStage class methods
MemoryStage::MemoryStage() : Stage(), instruction_to_memory(nullptr), memoryAccessSuccess(false) {}

MemoryStage::~MemoryStage() {}

void MemoryStage::setInstructionToMemory(Instruction* instruction) {
    instruction_to_memory.reset(instruction);
}

Instruction* MemoryStage::getInstructionToMemory() const {
    return instruction_to_memory.get();
}

void MemoryStage::accessMemory(Bus& bus) {
    if (instruction_to_memory) {
        instruction_to_memory->accessMemory(bus);
        memoryAccessSuccess = true;
    } else {
        memoryAccessSuccess = false;
    }
    
}


bool MemoryStage::wasMemoryAccessSuccessful() const {
    return memoryAccessSuccess;
}

// Implementation of WriteBackStage class methods

WriteBackStage::WriteBackStage() : Stage(), instruction_to_writeback(nullptr), writeBackSuccess(false) {}

WriteBackStage::~WriteBackStage() {}

void WriteBackStage::setInstructionToWriteBack(Instruction* instruction) {
    instruction_to_writeback.reset(instruction);
}

Instruction* WriteBackStage::getInstructionToWriteBack() const {
    return instruction_to_writeback.get();
}

void WriteBackStage::writeBack(Bus& bus) {

    if (instruction_to_writeback) {
        instruction_to_writeback->writeBack(bus);
        writeBackSuccess = true;
    } else {
        writeBackSuccess = false;
    }
    
  
}

bool WriteBackStage::wasWriteBackSuccessful() const {
    return writeBackSuccess;
}


///////////////////////////////////////////////////////////////////////////////

//impelemtation of teh pipeline class

Pipeline::Pipeline(Bus& bus) : bus(bus),fetchStage(), decodeStage(), executeStage(), memoryStage(), writeBackStage() {
    
}
                               

    
#include "pipeline.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include <iostream>


// Implementation of Stage class methods
Stage::Stage() {}

bool Stage::isStageReady() const {
    return status == StageStatus::READY;
}

StageStatus Stage::getStatus() const {
    return status;
}

void Stage::setStatus(StageStatus newStatus) {
    status = newStatus;
}


// Implementation of FetchStage class methods
FetchStage::FetchStage() : Stage(),currentInstructionInfo{.instructionId = 0, .totalLength = 0, .opcodeLength = 0, .prefixCount = 0,
                                                          .prefix = {0,0,0,0}, .rex = false, .rexprefix = 0, .opcode = 0, .additionalBytes = 0,
                                                          .numOperands = 0, .operandLength = 0, .src_operand_length = 0, .dest_operand_length = 0,
                                                          .bit_extension = 0, .rex_w_sensitive = false, .hasModRM = false,
                                                          .hasSIB = false, .hasDisplacement = false, .hasImmediate = false,
                                                          .instruction = {}, .description = ""} {}

FetchStage::~FetchStage() {}

void FetchStage::startFetch(Bus& bus, uint64_t instructionId, uint64_t& index) {
    // Implementation of instruction fetching using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Fetching in struction from memory...");
    bus.getCPU().getControlUnit().startFetch(instructionId, index);
}

void FetchStage::updateFetch(Bus& bus, uint64_t instructionId) {
    // Implementation of updating fetch stage using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Updating fetch stage...");
    bus.getCPU().getControlUnit().updateFetch(instructionId);

}

InstructionInfo FetchStage::fetchInstruction(Bus& bus, uint64_t instructionId, uint64_t& index) {
    // Implementation of instruction fetching using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Fetching instruction from memory...");
    return bus.getCPU().getControlUnit().fetchInstruction(instructionId, index);

}

InstructionInfo FetchStage::getCurrentInstructionInfo() const {
    return currentInstructionInfo;
}

void FetchStage::setCurrentInstructionInfo(InstructionInfo info) {
    currentInstructionInfo = info;                                        //maybe it is not needed
}



// Implementation of DecodeStage class methods
DecodeStage::DecodeStage() : Stage(), instruction_info_to_decode{.instructionId = 0, .totalLength = 0, .opcodeLength = 0, .prefixCount = 0,
                                                          .prefix = {0,0,0,0}, .rex = false, .rexprefix = 0, .opcode = 0, .additionalBytes = 0,
                                                          .numOperands = 0, .operandLength = 0, .src_operand_length = 0, .dest_operand_length = 0,
                                                          .bit_extension = 0, .rex_w_sensitive = false, .hasModRM = false,
                                                          .hasSIB = false, .hasDisplacement = false, .hasImmediate = false,
                                                          .instruction = {}, .description = ""} , decoded_instruction(std::make_unique<EmptyInstruction>()) {}
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
    debugLog("Decoding instruction...");
    decoded_instruction.reset(bus.getCPU().getControlUnit().decodeInstruction(instruction_info_to_decode));

    debugLog("Decoded instruction"); 
}

std::unique_ptr<Instruction> DecodeStage::getDecodedInstruction() {
    return std::move(decoded_instruction);
}



// Implementation of OperandFetchStage class methods
OperandFetchStage::OperandFetchStage() : Stage(), instruction_with_fetched_operands(std::make_unique<EmptyInstruction>()) {}
OperandFetchStage::~OperandFetchStage() {}

OperandFetchStage& Pipeline::getOperandFetchStage() {
    return operandFetchStage;
}

std::unique_ptr<Instruction> OperandFetchStage::getInstructionWithFetchedOperands() {
    return std::move(instruction_with_fetched_operands);
}


void OperandFetchStage::fetchOperands(Bus& bus) {
    // Implementation of operand fetching using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Fetching operands for the instruction...");
    if (instruction_with_fetched_operands) {
        instruction_with_fetched_operands->fetchOperands(bus);
    }
}

void OperandFetchStage::setInstructionWithFetchedOperands(std::unique_ptr<Instruction> instruction) {
    instruction_with_fetched_operands = std::move(instruction);
}



// Implementation of ExecuteStage class methods
ExecuteStage::ExecuteStage() : Stage(), instruction_to_execute(std::make_unique<EmptyInstruction>()) {}

ExecuteStage::~ExecuteStage() {}

void ExecuteStage::setInstructionToExecute(std::unique_ptr<Instruction> instruction) {
    instruction_to_execute = std::move(instruction);
}

std::unique_ptr<Instruction> ExecuteStage::getInstructionToExecute() {
    return std::move(instruction_to_execute);
}

void ExecuteStage::startExecution(Bus& bus) {
    // Implementation of starting execution using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Starting execution of instruction...");
    if (instruction_to_execute) {
        instruction_to_execute->startExecution(bus);
    }
}

void ExecuteStage::updateExecution(Bus& bus) {
    // Implementation of updating execution using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Updating execution of instruction...");
    if (instruction_to_execute) {
        instruction_to_execute->updateExecution(bus);
    }
}

void ExecuteStage::executeInstruction(Bus& bus) {
    // Implementation of instruction execution using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Executing instruction...");
    if (instruction_to_execute) {
        instruction_to_execute->execute(bus);
        executionSuccess = true;
    } else {
        executionSuccess = false;
    }
   
}


uint64_t ExecuteStage::getExecutionResult() const {
    return executionResult;
}

bool ExecuteStage::wasExecutionSuccessful() const {
    return executionSuccess;
}



// Implementation of MemoryStage class methods
MemoryStage::MemoryStage() : Stage(), instruction_to_memory(std::make_unique<EmptyInstruction>()), memoryAccessSuccess(false) {}

MemoryStage::~MemoryStage() {}

void MemoryStage::setInstructionToMemory(std::unique_ptr<Instruction> instruction) {
    instruction_to_memory = std::move(instruction);
}
std::unique_ptr<Instruction> MemoryStage::getInstructionToMemory(){
    return std::move(instruction_to_memory);
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


WriteBackStage::WriteBackStage() : Stage(), instruction_to_writeback(std::make_unique<EmptyInstruction>()), writeBackSuccess(false) {}

WriteBackStage::~WriteBackStage() {}

void WriteBackStage::setInstructionToWriteBack(std::unique_ptr<Instruction> instruction) {
    instruction_to_writeback = std::move(instruction);
}

std::unique_ptr<Instruction> WriteBackStage::getInstructionToWriteBack() {
    return std::move(instruction_to_writeback);
}

void WriteBackStage::writeBack(Bus& bus) {

    debugLog("Write-Back stage processing...");
    if (instruction_to_writeback) {
        debugLog("Writing back instruction results...");
        instruction_to_writeback->writeBack(bus);
        writeBackSuccess = true;
        debugLog("Write-Back completed successfully.");
    } else {
        writeBackSuccess = false;
        debugLog("No instruction to write back.");
    }
    
  
}

bool WriteBackStage::wasWriteBackSuccessful() const {
    return writeBackSuccess;
}

///////////////////////////////////////////////////////////////////////////////

//implementation of the pipeline class

Pipeline::Pipeline(Bus& bus) : bus(bus),fetchStage(), decodeStage(), executeStage(), memoryStage(), writeBackStage() {
    
}
                               

Pipeline::~Pipeline() {
    // Clean up resources if needed
}


FetchStage& Pipeline::getFetchStage() {
    return fetchStage;
}

DecodeStage& Pipeline::getDecodeStage() {
    return decodeStage;
}

ExecuteStage& Pipeline::getExecuteStage() {
    return executeStage;
}

MemoryStage& Pipeline::getMemoryStage() {
    return memoryStage;
}

WriteBackStage& Pipeline::getWriteBackStage() {
    return writeBackStage;
}

void Pipeline::execute_operation() {
    // Implementation of pipeline operation execution for the current cycle
    

    //controllare se buffer inetrmedi sono validi e non stalled prima di spostare le istruzioni tra le stage
    debugLog("Executing pipeline operation for the current cycle...");

    //index value for fetching instruction
    

    /*

   
    if(writeBackStage.isStageReady()) 
    {
        std::cout << "WRITE-BACK STAGE processing..." << std::endl;
        if(memoryWriteBackBuffer.valid && !memoryWriteBackBuffer.stalled)
        {
            writeBackStage.setInstructionToWriteBack(std::move(memoryWriteBackBuffer.memoryAccessedInstruction));
            memoryWriteBackBuffer.valid = false;
            writeBackStage.writeBack(bus);
        }
        else if (memoryStage.isStageReady() && memoryStage.getInstructionToMemory())
        {
            writeBackStage.setInstructionToWriteBack(memoryStage.getInstructionToMemory());
            writeBackStage.writeBack(bus);
        }
        else 
        {
            std::cout << "Write-Back stage has no instruction to process." << std::endl;
            writeBackStage.setStatus(StageStatus::EMPTY);
        }
    }
    else 
    {
        std::cout << "WRITE-BACK STAGE is not ready." << std::endl;
    }

    ///////////////////////////////////////////////////////////////////////
    
    if(memoryStage.isStageReady()) 
    {
        std::cout << "MEMORY STAGE processing..." << std::endl;
        if(executeMemoryBuffer.valid) 
        {
            memoryStage.setInstructionToMemory(std::move(executeMemoryBuffer.executedInstruction));
            executeMemoryBuffer.valid = false;
            memoryStage.accessMemory(bus);
            // After memory access, move instruction to Memory-WriteBack buffer
            memoryStage.setStatus(StageStatus::WAITING_MEMORY);
           
        }
        else if (executeStage.isStageReady() && executeStage.getInstructionToExecute())
        {
            memoryStage.setInstructionToMemory(executeStage.getInstructionToExecute());
            memoryStage.accessMemory(bus);
            memoryStage.setStatus(StageStatus::WAITING_MEMORY);
            
        }
        else 
        {
            std::cout << "Memory stage has no instruction to process." << std::endl;
        }
    }
    else if (memoryStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        std::cout << "MEMORY STAGE is waiting for memory operation to complete." << std::endl;
        
        
    }
    else if (memoryStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        std::cout << "MEMORY STAGE memory operation completed." << std::endl;
        // Move instruction to Memory-WriteBack buffer
        memoryWriteBackBuffer.memoryAccessedInstruction = memoryStage.getInstructionToMemory();
        memoryWriteBackBuffer.valid = true;
        memoryWriteBackBuffer.stalled = false;
        memoryWriteBackBuffer.flushed = false;
        memoryStage.setStatus(StageStatus::READY);
    }
    else 
    {
        std::cout << "MEMORY STAGE is not ready." << std::endl;
    }

    ///////////////////////////////////////////////////////////////////////
    */
    if(executeStage.isStageReady()) 
    {
        debugLog("EXECUTE STAGE processing...");
        if(operandFetchExecuteBuffer.valid) 
        {
            debugLog("OperandFetch-Execute buffer has valid instruction.");
            executeStage.setInstructionToExecute(std::move(operandFetchExecuteBuffer.instructionWithOperands));
            operandFetchExecuteBuffer.valid = false;
            executeStage.startExecution(bus);
        }
        else if (operandFetchStage.isStageReady() && operandFetchStage.getInstructionWithFetchedOperands())
        {
            debugLog("Operand Fetch stage has valid instruction.");
            executeStage.setInstructionToExecute(operandFetchStage.getInstructionWithFetchedOperands());
            executeStage.startExecution(bus);
        }
        else 
        {
            debugLog("Execute stage has no instruction to process.");
        }
    }
    else if (executeStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("EXECUTE STAGE is waiting for instruction execution to complete.");

        executeStage.updateExecution(bus);
    }
    else if (executeStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        debugLog("EXECUTE STAGE instruction execution completed.");

        // Move instruction to Execute-Memory buffer
        debugLog("INDEX VALUE: " + to_string_hex(index));
        executeMemoryBuffer.executedInstruction = executeStage.getInstructionToExecute();
        executeMemoryBuffer.valid = true;
        executeMemoryBuffer.stalled = false;
        executeMemoryBuffer.flushed = false;
        executeStage.setStatus(StageStatus::READY);
    }
    else 
    {
        debugLog("EXECUTE STAGE is not ready.");
    }
    ///////////////////////////////////////////////////////////////////////
    if(operandFetchStage.isStageReady()) 
    {
        debugLog("OPERAND FETCH STAGE processing...");
        if(decodeOperandFetchBuffer.valid) 
        {
            debugLog("Decoding-OperandFetch buffer has valid instruction.");
            operandFetchStage.setInstructionWithFetchedOperands(std::move(decodeOperandFetchBuffer.decodedInstruction));
            decodeOperandFetchBuffer.valid = false;
            operandFetchStage.fetchOperands(bus);
            
            // After fetching operands, move instruction to OperandFetch-Execute buffer

            operandFetchExecuteBuffer.instructionWithOperands = operandFetchStage.getInstructionWithFetchedOperands();
            operandFetchExecuteBuffer.valid = true;
            operandFetchExecuteBuffer.stalled = false;
            operandFetchExecuteBuffer.flushed = false;
        }
        else if (decodeStage.isStageReady() && decodeStage.getDecodedInstruction())
        {
            debugLog("Decode stage has valid instruction.");
            operandFetchStage.setInstructionWithFetchedOperands(decodeStage.getDecodedInstruction());
            operandFetchStage.fetchOperands(bus);
            
            // After fetching operands, move instruction to OperandFetch-Execute buffer
            operandFetchExecuteBuffer.instructionWithOperands = operandFetchStage.getInstructionWithFetchedOperands();
            operandFetchExecuteBuffer.valid = true;
            operandFetchExecuteBuffer.stalled = false;
            operandFetchExecuteBuffer.flushed = false;
        }
        else 
        {
            debugLog("Operand Fetch stage has no instruction to process.");
        }
    }
    else 
    {
        std::cout << "OPERAND FETCH STAGE is not ready." << std::endl;
    }
    

    ///////////////////////////////////////////////////////////////////////////////

    if(decodeStage.isStageReady()) 
    {
        debugLog("DECODE STAGE processing...");
        if(fetchDecodeBuffer.valid) 
        {
            decodeStage.setInstructionToDecode(fetchDecodeBuffer.instructionInfo);
            fetchDecodeBuffer.valid = false;
            decodeStage.decodeInstruction(bus);
            // After decoding, move instruction to Decode-OperandFetch buffer
            decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
            decodeOperandFetchBuffer.valid = true;
            decodeOperandFetchBuffer.stalled = false;
            decodeOperandFetchBuffer.flushed = false;
        }
        else if (fetchStage.isStageReady() && fetchStage.getCurrentInstructionInfo().instruction.size() > 0)
        {
            decodeStage.setInstructionToDecode(fetchStage.getCurrentInstructionInfo());
            decodeStage.decodeInstruction(bus);
            // After decoding, move instruction to Decode-OperandFetch buffer
            decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
            decodeOperandFetchBuffer.valid = true;
            decodeOperandFetchBuffer.stalled = false;
            decodeOperandFetchBuffer.flushed = false;
        }
        else 
        {
            debugLog("Decode stage has no instruction to process.");
        }
    }
    else 
        debugLog("DECODE STAGE is not ready.");
 
    //////////////////////////////////////////////////////////////////////////////// 

    if(fetchStage.isStageReady()) 
    {
        debugLog("FETCH STAGE processing...");
        FetchstageInstructionId = bus.getCPU().getInstructionIdCounter();
        debugLog("FetchstageInstructionId: " + std::to_string(FetchstageInstructionId));
        bus.getCPU().incrementInstructionIdCounter();
        fetchStage.startFetch(bus, FetchstageInstructionId, index);
        fetchStage.setStatus(StageStatus::WAITING_MEMORY);
    }
    else if (fetchStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("FETCH STAGE is waiting for instruction fetch to complete.");

        fetchStage.updateFetch(bus, FetchstageInstructionId);
    }
    else if (fetchStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        debugLog("FETCH STAGE instruction fetch completed.");

        // Move instruction to Fetch-Decode buffer
        debugLog("INDEX VALUE: " + to_string_hex(index));
        fetchDecodeBuffer.instructionInfo = fetchStage.fetchInstruction(bus, FetchstageInstructionId, index);
        fetchDecodeBuffer.valid = true;
        fetchDecodeBuffer.stalled = false;
        fetchDecodeBuffer.flushed = false;
        fetchStage.setStatus(StageStatus::READY);
    }
    else 
    {
        debugLog("FETCH STAGE is not ready.");
    }
    
}
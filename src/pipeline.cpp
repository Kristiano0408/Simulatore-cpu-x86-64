#include "pipeline.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include <iostream>


// Implementation of Stage class methods

bool Stage::isInstructionEmpty(const Instruction* instr) const {
    if(!instr) {
        return true;
    }
    return instr->isEmpty();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of FetchStage class methods

FetchStage::FetchStage() : Stage(),currentInstructionInfo() {}

void FetchStage::startFetch(Bus& bus, uint64_t instructionId, uint64_t& index, EventHandler& eventHandler) {
    // Implementation of instruction fetching using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Fetching in struction from memory...");
    bus.getCPU().getControlUnit().startFetch(instructionId, index, eventHandler);
}

void FetchStage::updateFetch(Bus& bus, uint64_t instructionId, EventHandler& eventHandler) {
    // Implementation of updating fetch stage using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Updating fetch stage...");
    bus.getCPU().getControlUnit().updateFetch(instructionId, eventHandler);

}

InstructionInfo FetchStage::fetchInstruction(Bus& bus, uint64_t instructionId, uint64_t& index) {
    // Implementation of instruction fetching using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Fetching instruction from memory...");
    return bus.getCPU().getControlUnit().fetchInstruction(instructionId, index);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of DecodeStage class methods

DecodeStage::DecodeStage() : Stage(), instruction_info_to_decode(), decoded_instruction(std::make_unique<EmptyInstruction>()) {}


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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of OperandFetchStage class methods

OperandFetchStage::OperandFetchStage() : Stage(), instruction_with_fetched_operands(std::make_unique<EmptyInstruction>()) {}

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of ExecuteStage class methods

ExecuteStage::ExecuteStage() : Stage(), instruction_to_execute(std::make_unique<EmptyInstruction>()) {}

void ExecuteStage::setInstructionToExecute(std::unique_ptr<Instruction> instruction) {
    instruction_to_execute = std::move(instruction);
}

std::unique_ptr<Instruction> ExecuteStage::getInstructionToExecute() {
    return std::move(instruction_to_execute);
}


void ExecuteStage::startExecution(Bus& bus, EventHandler& eventHandler) {
    // Implementation of starting execution using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Starting execution of instruction...");
    if (instruction_to_execute) {
        instruction_to_execute->startExecution(bus, eventHandler);
    }
}

void ExecuteStage::updateExecution(Bus& bus, EventHandler& eventHandler) {
    // Implementation of updating execution using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Updating execution of instruction...");
    if (instruction_to_execute) {
        instruction_to_execute->updateExecution(bus, eventHandler);
    }
}

void ExecuteStage::executeInstruction(Bus& bus) {
    // Implementation of instruction execution using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Executing instruction...");
    if (instruction_to_execute) 
        instruction_to_execute->execute(bus);
   
   
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of MemoryStage class methods

MemoryStage::MemoryStage() : Stage(), instruction_to_memory(std::make_unique<EmptyInstruction>()){}


void MemoryStage::setInstructionToMemory(std::unique_ptr<Instruction> instruction) {
    instruction_to_memory = std::move(instruction);
}

std::unique_ptr<Instruction> MemoryStage::getInstructionToMemory(){
    return std::move(instruction_to_memory);
}


void MemoryStage::requestMemoryAccess(Bus& bus, EventHandler& eventHandler) {
    // Implementation of starting memory access using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Requesting memory access for instruction...");
    if (instruction_to_memory) {
        instruction_to_memory->requestMemoryAccess(bus, eventHandler);
    } 
    
}

void MemoryStage::updateMemoryAccess(Bus& bus, EventHandler& eventHandler) {
    // Implementation of updating memory access using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Updating memory access for instruction...");
    if (instruction_to_memory) {
        instruction_to_memory->updateMemoryAccess(bus, eventHandler);
    }
}

void MemoryStage::accessMemory(Bus& bus) {
    if (instruction_to_memory)
        instruction_to_memory->accessMemory(bus);
}

////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of WriteBackStage class methods

WriteBackStage::WriteBackStage() : Stage(), instruction_to_writeback(std::make_unique<EmptyInstruction>()) {}

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
        debugLog("Write-Back completed successfully.");
    } else {
        debugLog("No instruction to write back.");
    }
    
  
}


///////////////////////////////////////////////////////////////////////////////

//implementation of the pipeline class

Pipeline::Pipeline(Bus& bus, EventHandler* eventHandler) : bus(bus),fetchStage(), decodeStage(), executeStage(), memoryStage(), writeBackStage(), eventHandler(eventHandler) {
    
}
                               
void Pipeline::execute_operation() {
    // Implementation of pipeline operation execution for the current cycle
    

    //controllare se buffer inetrmedi sono validi e non stalled prima di spostare le istruzioni tra le stage
    debugLog("Executing pipeline operation for the current cycle...");
    /*
    //index value for fetching instruction
    if(writeBackStage.isStageReady()) 
    {
        debugLog("WRITEBACK STAGE processing...");
        if(memoryWriteBackBuffer.valid) 
        {
            debugLog("Memory-WriteBack buffer has valid instruction.");
            writeBackStage.setInstructionToWriteBack(std::move(memoryWriteBackBuffer.memoryAccessedInstruction));
            memoryWriteBackBuffer.valid = false;
            writeBackStage.writeBack(bus);
            // After memory access, move instruction to Memory-WriteBack buffer

        }
        else if (memoryStage.isStageReady() && !memoryStage.isInstructionEmpty(memoryStage.peekInstruction()))
        {
            debugLog("Memory stage has valid instruction.");
            writeBackStage.setInstructionToWriteBack(memoryStage.getInstructionToMemory());
            writeBackStage.writeBack(bus);

        }
        else 
        {
            debugLog("WriteBack stage has no instruction to process.");
        }
    }
    else 
    {
        debugLog("WRITEBACK STAGE is not ready.");
    }
    ///////////////////////////////////////////////////////////////////////

    if(memoryStage.isStageReady()) 
    {
        debugLog("MEMORY STAGE processing...");
        if(executeMemoryBuffer.valid) 
        {
            debugLog("Execute-Memory buffer has valid instruction.");
            memoryStage.setInstructionToMemory(std::move(executeMemoryBuffer.executedInstruction));
            executeMemoryBuffer.valid = false;
            memoryStage.requestMemoryAccess(bus, *eventHandler);
            // After memory access, move instruction to Memory-WriteBack buffer

        }
        else if (executeStage.isStageReady() && !executeStage.isInstructionEmpty(executeStage.peekInstruction()))
        {
            debugLog("Execute stage has valid instruction.");
            memoryStage.setInstructionToMemory(executeStage.getInstructionToExecute());
            memoryStage.requestMemoryAccess(bus, *eventHandler);

        }
        else 
        {
            debugLog("Memory stage has no instruction to process.");
        }
    }
    else if (memoryStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("MEMORY STAGE is waiting for memory operation to complete.");
        memoryStage.updateMemoryAccess(bus, *eventHandler);


    }
    else if (memoryStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        debugLog("MEMORY STAGE memory operation completed.");
        // Move instruction to Memory-WriteBack buffer
        memoryStage.accessMemory(bus);
        memoryWriteBackBuffer.memoryAccessedInstruction = memoryStage.getInstructionToMemory();
        memoryWriteBackBuffer.valid = true;
        memoryWriteBackBuffer.stalled = false;
        memoryWriteBackBuffer.flushed = false;
        memoryStage.setStatus(StageStatus::READY);
    }
    else 
    {
        debugLog("MEMORY STAGE is not ready.");
    }
    ///////////////////////////////////////////////////////////////////////
   
    if(executeStage.isStageReady()) 
    {
        debugLog("EXECUTE STAGE processing...");
        if(operandFetchExecuteBuffer.valid) 
        {
            debugLog("OperandFetch-Execute buffer has valid instruction.");
            executeStage.setInstructionToExecute(std::move(operandFetchExecuteBuffer.instructionWithOperands));
            operandFetchExecuteBuffer.valid = false;
            executeStage.startExecution(bus, *eventHandler);
        }
        else if (operandFetchStage.isStageReady() && !operandFetchStage.isInstructionEmpty(operandFetchStage.peekInstruction()))
        {
            debugLog("Operand Fetch stage has valid instruction.");
            executeStage.setInstructionToExecute(operandFetchStage.getInstructionWithFetchedOperands());
            executeStage.startExecution(bus, *eventHandler);
        }
        else 
        {
            debugLog("Execute stage has no instruction to process.");
        }
    }
    else if (executeStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("EXECUTE STAGE is waiinline Stage* getStage(StageType stageType) {return stageMap.at(stageType);}ting for instruction execution to complete.");

        executeStage.updateExecution(bus, *eventHandler);
    }
    else if (executeStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        executeStage.executeInstruction(bus);
        debugLog("EXECUTE STAGE instruction execution completed.");

        // Move instruction to Execute-Memory buffer
        debugLog("INDEX VALUE: " + to_string_hex(index));
        executeMemoryBuffer.executedInstruction = executeStage.getInstructionToExecute();
        executeMemoryBuffer.valid = true;
        debugLog("Setting Execute-Memory buffer valid.");
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
        else if (decodeStage.isStageReady() && !decodeStage.isInstructionEmpty(decodeStage.peekInstruction()))
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
            debugLog("Fetch-Decode buffer has valid instruction.");
            decodeStage.setInstructionToDecode(fetchDecodeBuffer.instructionInfo);
            fetchDecodeBuffer.valid = false;
            decodeStage.decodeInstruction(bus);
            // After decoding, move instruction to Decode-OperandFetch buffer
            decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
            decodeOperandFetchBuffer.valid = true;
            decodeOperandFetchBuffer.stalled = false;
            decodeOperandFetchBuffer.flushed = false;
            //reset the istruictioninfo of decode stage
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
            //reset the istruictioninfo of decode stage
        }
        else 
        {
            debugLog("Decode stage has no instruction to process.");
        }
    }
    else 
        debugLog("DECODE STAGE is not ready.");
    */
    //////////////////////////////////////////////////////////////////////////////// 

    if(fetchStage.isStageReady()) 
    {
        debugLog("FETCH STAGE processing...");
        bus.getCPU().incrementInstructionIdCounter();
        FetchstageInstructionId = bus.getCPU().getInstructionIdCounter();
        debugLog("FetchstageInstructionId: " + std::to_string(FetchstageInstructionId));
        
        fetchStage.startFetch(bus, FetchstageInstructionId, index, *eventHandler); //the event handler pointer is tranformed to reference for the function call
        debugLog("FETCH STAGE fetch started.");
        
    }
    else if (fetchStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("FETCH STAGE is waiting for instruction fetch to complete.");

        //fetchStage.updateFetch(bus, FetchstageInstructionId, *eventHandler);
    }
    else if (fetchStage.getStatus() == StageStatus::MEMORY_DONE)
    { 
        debugLog("FETCH STAGE instruction fetch completed.");

        // Move instruction to Fetch-Decode buffer
        debugLog("INDEX VALUE: " + to_string_hex(index));
        fetchDecodeBuffer.instructionInfo = fetchStage.fetchInstruction(bus, FetchstageInstructionId, index);
        debugLog("Fetched Instruction ID: " + std::to_string(fetchDecodeBuffer.instructionInfo.instructionId));
        fetchDecodeBuffer.valid = true;
        fetchDecodeBuffer.stalled = false;
        fetchDecodeBuffer.flushed = false;
        fetchStage.setStatus(StageStatus::READY);
        //reset the instructionInfo of fetch stage
        fetchStage.setCurrentInstructionInfo(InstructionInfo());
    }
    else 
    {
        debugLog("FETCH STAGE is not ready.");
    }
    
}
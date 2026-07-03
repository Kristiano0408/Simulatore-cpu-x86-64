#include "pipeline.hpp"
#include "cpu.hpp"
#include "executeEngine.hpp"


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

void FetchStage::startFetch(CPU& cpu, uint64_t instructionId, uint64_t& index, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Fetching in struction from memory...");
    cpu.getControlUnit().startFetch(instructionId, index, eventHandler);
}

void FetchStage::updateFetch(CPU& cpu, uint64_t instructionId, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Updating fetch stage...");
    cpu.getControlUnit().updateFetch(instructionId, eventHandler);

}

InstructionInfo FetchStage::fetchInstruction(CPU& cpu, uint64_t instructionId, uint64_t& index, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Fetching instruction from memory...");
    return cpu.getControlUnit().fetchInstruction(instructionId, index, eventHandler);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of DecodeStage class methods

DecodeStage::DecodeStage() : Stage(), instruction_info_to_decode(), decoded_instruction(std::make_unique<EmptyInstruction>()) {}


void DecodeStage::decodeInstruction(CPU& cpu, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{

    debugLog("Decoding instruction...");
    cpu.getControlUnit().decodeInstruction(instruction_info_to_decode, decoded_instruction, eventHandler);
}

std::unique_ptr<Instruction> DecodeStage::getDecodedInstruction() 
{
    return std::move(decoded_instruction);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of OperandFetchStage class methods

OperandFetchStage::OperandFetchStage() : Stage(), instruction_with_fetched_operands(std::make_unique<EmptyInstruction>()) {}

std::unique_ptr<Instruction> OperandFetchStage::getInstructionWithFetchedOperands() 
{
    return std::move(instruction_with_fetched_operands);
}

void OperandFetchStage::fetchOperands(ExecuteEngine& executeEngine, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Fetching operands for the instruction...");
    if (instruction_with_fetched_operands) {
        executeEngine.fetchOperands(peekInstruction(), eventHandler);
    }
}

void OperandFetchStage::setInstructionWithFetchedOperands(std::unique_ptr<Instruction> instruction) 
{
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


void ExecuteStage::startExecution(ExecuteEngine& executeEngine, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    // This is a placeholder implementation and should be replaced with actual logic
    debugLog("Starting execution of instruction...");
    if (instruction_to_execute) {
        executeEngine.startExecution(peekInstruction(), eventHandler);
    }
}

void ExecuteStage::updateExecution(ExecuteEngine& executeEngine, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Updating execution of instruction...");
    if (instruction_to_execute) {
        executeEngine.updateExecution(peekInstruction(), eventHandler);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of MemoryStage class methods

MemoryStage::MemoryStage() : Stage(), instruction_to_memory(std::make_unique<EmptyInstruction>()){}


void MemoryStage::setInstructionToMemory(std::unique_ptr<Instruction> instruction) 
{
    instruction_to_memory = std::move(instruction);
}

std::unique_ptr<Instruction> MemoryStage::getInstructionToMemory()
{
    return std::move(instruction_to_memory);
}


void MemoryStage::requestMemoryAccess(ExecuteEngine& executeEngine, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Requesting memory access for instruction...");
    if (instruction_to_memory) 
    {
        executeEngine.requestMemoryAccess(peekInstruction(), eventHandler);
    } 
    
}

void MemoryStage::accessMemory(ExecuteEngine& executeEngine, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    if (instruction_to_memory)
        executeEngine.accessMemory(peekInstruction(), eventHandler);
}

////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of WriteBackStage class methods

WriteBackStage::WriteBackStage() : Stage(), instruction_to_writeback(std::make_unique<EmptyInstruction>()) {}

void WriteBackStage::setInstructionToWriteBack(std::unique_ptr<Instruction> instruction) 
{
    instruction_to_writeback = std::move(instruction);
}

std::unique_ptr<Instruction> WriteBackStage::getInstructionToWriteBack() 
{
    return std::move(instruction_to_writeback);
}

void WriteBackStage::writeBack(ExecuteEngine& executeEngine, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    debugLog("Write-Back stage processing...");
    if (instruction_to_writeback) {
        executeEngine.writeBackInstruction(peekInstruction(), eventHandler);
    } else {
        debugLog("No instruction to write back.");
    }
}


///////////////////////////////////////////////////////////////////////////////

//implementation of the pipeline class

Pipeline::Pipeline(CPU& cpu, EventHandler<EventHandlerPipelineEventType>* eventHandler) : cpu(cpu), executeEngine(cpu, cpu.getRegisters(), cpu.getALU(), cpu.getCacheManager()), fetchStage(), decodeStage(), executeStage(), memoryStage(), writeBackStage(), eventHandler(eventHandler) 
{
    
}
                               
void Pipeline::execute_operation() 
{
    // Implementation of pipeline operation execution for the current cycle
    
    //controllare se buffer inetrmedi sono validi e non stalled prima di spostare le istruzioni tra le stage
    debugLog("Executing pipeline operation for the current cycle...");

    processWriteBackStage();

    processMemoryStage();

    processExecuteStage();

    processOperandFetchStage();

    processDecodeStage();

    processFetchStage();
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processWriteBackStage() 
{
    if(writeBackStage.isStageReady()) 
    {
        debugLog("WRITEBACK STAGE processing...");
        if(memoryWriteBackBuffer.valid) 
        {
            debugLog("Memory-WriteBack buffer has valid instruction.");
            writeBackStage.setInstructionToWriteBack(std::move(memoryWriteBackBuffer.memoryAccessedInstruction));
            memoryWriteBackBuffer.valid = false;
            writeBackStage.writeBack(executeEngine, *eventHandler);

        }
        else if (memoryStage.isStageReady() && !memoryStage.isInstructionEmpty(memoryStage.peekInstruction()))
        {
            debugLog("Memory stage has valid instruction.");
            writeBackStage.setInstructionToWriteBack(memoryStage.getInstructionToMemory());
            writeBackStage.writeBack(executeEngine, *eventHandler);

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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void Pipeline::processMemoryStage() 
{
     if(memoryStage.isStageReady()) 
    {
        debugLog("MEMORY STAGE processing...");
        if(executeMemoryBuffer.valid) 
        {
            debugLog("Execute-Memory buffer has valid instruction.");
            memoryStage.setInstructionToMemory(std::move(executeMemoryBuffer.executedInstruction));
            executeMemoryBuffer.valid = false;
            memoryStage.requestMemoryAccess(executeEngine, *eventHandler);
        }
        else if (executeStage.isStageReady() && !executeStage.isInstructionEmpty(executeStage.peekInstruction()))
        {
            debugLog("Execute stage has valid instruction.");
            memoryStage.setInstructionToMemory(executeStage.getInstructionToExecute());
            memoryStage.requestMemoryAccess(executeEngine, *eventHandler);

        }
        else 
        {
            debugLog("Memory stage has no instruction to process.");
        }
    }
    else if (memoryStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("MEMORY STAGE is waiting for memory operation to complete.");


    }
    else if (memoryStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        debugLog("MEMORY STAGE memory operation completed.");
        // Move instruction to Memory-WriteBack buffer
        memoryStage.accessMemory(executeEngine, *eventHandler);
    }
    else 
    {
        debugLog("MEMORY STAGE is not ready.");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processExecuteStage()
{
      if(executeStage.isStageReady()) 
    {
        debugLog("EXECUTE STAGE processing...");
        if(operandFetchExecuteBuffer.valid) 
        {
            debugLog("OperandFetch-Execute buffer has valid instruction.");
            executeStage.setInstructionToExecute(std::move(operandFetchExecuteBuffer.instructionWithOperands));
            operandFetchExecuteBuffer.valid = false;
            executeStage.startExecution(executeEngine,*eventHandler);
        }
        else if (operandFetchStage.isStageReady() && !operandFetchStage.isInstructionEmpty(operandFetchStage.peekInstruction()))
        {
            debugLog("Operand Fetch stage has valid instruction.");
            executeStage.setInstructionToExecute(operandFetchStage.getInstructionWithFetchedOperands());
            executeStage.startExecution(executeEngine, *eventHandler);
        }
        else 
        {
            debugLog("Execute stage has no instruction to process.");
        }
    }
    else if (executeStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("EXECUTE STAGE is waiting for instruction execution to complete.");

    }
    else if (executeStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        executeStage.updateExecution(executeEngine, *eventHandler);
        debugLog("EXECUTE STAGE instruction execution completed.");

        // Move instruction to Execute-Memory buffer
        debugLog("INDEX VALUE: " + to_string_hex(index));
        
    }
    else 
    {
        debugLog("EXECUTE STAGE is not ready.");
    }


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processOperandFetchStage()
{
    if(operandFetchStage.isStageReady()) 
    {
        debugLog("OPERAND FETCH STAGE processing...");
        if(decodeOperandFetchBuffer.valid) 
        {
            debugLog("Decoding-OperandFetch buffer has valid instruction.");
            operandFetchStage.setInstructionWithFetchedOperands(std::move(decodeOperandFetchBuffer.decodedInstruction));
            decodeOperandFetchBuffer.valid = false;
            operandFetchStage.fetchOperands(executeEngine, *eventHandler);
            
            // After fetching operands, move instruction to OperandFetch-Execute buffer

            
        }
        else if (decodeStage.isStageReady() && !decodeStage.isInstructionEmpty(decodeStage.peekInstruction()))
        {
            debugLog("Decode stage has valid instruction.");
            operandFetchStage.setInstructionWithFetchedOperands(decodeStage.getDecodedInstruction());
            operandFetchStage.fetchOperands(executeEngine, *eventHandler);
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

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processDecodeStage()
{
    if(decodeStage.isStageReady()) 
    {
        debugLog("DECODE STAGE processing...");
        if(fetchDecodeBuffer.valid) 
        {
            debugLog("Fetch-Decode buffer has valid instruction.");
            decodeStage.setInstructionToDecode(fetchDecodeBuffer.instructionInfo);
            fetchDecodeBuffer.valid = false;
            decodeStage.decodeInstruction(cpu, *eventHandler);
            //reset the istruictioninfo of decode stage
        }
        else if (fetchStage.isStageReady() && fetchStage.getCurrentInstructionInfo().instruction.size() > 0)
        {
            decodeStage.setInstructionToDecode(fetchStage.getCurrentInstructionInfo());
            decodeStage.decodeInstruction(cpu, *eventHandler);
            // After decoding, move instruction to Decode-OperandFetch buffer
            //reset the istruictioninfo of decode stage
        }
        else 
        {
            debugLog("Decode stage has no instruction to process.");
        }
    }
    else 
        debugLog("DECODE STAGE is not ready.");

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processFetchStage()
{
      if(fetchStage.isStageReady()) 
    {
        debugLog("FETCH STAGE processing...");
        cpu.incrementInstructionIdCounter();
        debugLog("incementing counter");
        FetchstageInstructionId = cpu.getInstructionIdCounter();
        debugLog("FetchstageInstructionId: " + std::to_string(FetchstageInstructionId));
        
        fetchStage.startFetch(cpu, FetchstageInstructionId, index, *eventHandler); //the event handler pointer is tranformed to reference for the function call
        debugLog("FETCH STAGE fetch started.");
        
    }
    else if (fetchStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        debugLog("FETCH STAGE is waiting for instruction fetch to complete.");

        fetchStage.updateFetch(cpu, FetchstageInstructionId, *eventHandler);
    }
    else if (fetchStage.getStatus() == StageStatus::MEMORY_DONE)
    { 
        debugLog("FETCH STAGE instruction fetch completed.");

        // Move instruction to Fetch-Decode buffer
        fetchDecodeBuffer.instructionInfo = fetchStage.fetchInstruction(cpu, FetchstageInstructionId, index, *eventHandler);
        
    }
    else 
    {
        debugLog("FETCH STAGE is not ready.");
    }

}


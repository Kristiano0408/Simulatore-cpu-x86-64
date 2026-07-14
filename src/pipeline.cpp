#include "pipeline.hpp"
#include "cpu.hpp"
#include "executeEngine.hpp"


// Implementation of Stage class methods

bool Stage::isInstructionEmpty(Instruction* instr) const {
    
    if(!instr || (instr->isEmpty()))
        return true;
    
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of FetchStage class methods

FetchStage::FetchStage() : Stage(),currentInstructionInfo() {}

void FetchStage::startFetch(CPU& cpu, uint64_t instructionId, uint64_t& index, PipelineEventHandler& eventHandler) 
{
    DEBUG_LOG(debugLog("Fetching in struction from memory..."));
    cpu.getControlUnit().startFetch(instructionId, index, eventHandler);
}

void FetchStage::updateFetch(CPU& cpu, uint64_t instructionId, PipelineEventHandler& eventHandler) 
{
    DEBUG_LOG(debugLog("Updating fetch stage..."));
    cpu.getControlUnit().updateFetch(instructionId, eventHandler);

}

InstructionInfo FetchStage::fetchInstruction(CPU& cpu, uint64_t instructionId, uint64_t& index, PipelineEventHandler& eventHandler) 
{
    DEBUG_LOG(debugLog("Fetching instruction from memory..."));
    return cpu.getControlUnit().fetchInstruction(instructionId, index, eventHandler);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of DecodeStage class methods

DecodeStage::DecodeStage() : Stage(), instruction_info_to_decode(), decoded_instruction(std::make_unique<Instruction>()) {}


void DecodeStage::decodeInstruction(CPU& cpu, PipelineEventHandler& eventHandler) 
{

    DEBUG_LOG(debugLog("Decoding instruction..."));
    cpu.getControlUnit().decodeInstruction(instruction_info_to_decode, decoded_instruction, eventHandler);
}

std::unique_ptr<Instruction> DecodeStage::getDecodedInstruction() 
{
    return std::move(decoded_instruction);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of OperandFetchStage class methods

OperandFetchStage::OperandFetchStage() : Stage(), instruction_with_fetched_operands(std::make_unique<Instruction>()) {}

std::unique_ptr<Instruction> OperandFetchStage::getInstructionWithFetchedOperands() 
{
    return std::move(instruction_with_fetched_operands);
}

void OperandFetchStage::fetchOperands(ExecuteEngine& executeEngine) 
{
    DEBUG_LOG(debugLog("Fetching operands for the instruction..."));
    if (instruction_with_fetched_operands) {
        if (instruction_with_fetched_operands->isEmpty()) 
        {
            DEBUG_LOG(debugLog("Instruction is empty. No operand fetch needed."));
            return;
        }
        executeEngine.sendOperandFetchRequest(peekInstruction());
    }
}

void OperandFetchStage::setInstructionWithFetchedOperands(std::unique_ptr<Instruction> instruction) 
{
    instruction_with_fetched_operands = std::move(instruction);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of ExecuteStage class methods

ExecuteStage::ExecuteStage() : Stage(), instruction_to_execute(std::make_unique<Instruction>()) {}

void ExecuteStage::setInstructionToExecute(std::unique_ptr<Instruction> instruction) {
    instruction_to_execute = std::move(instruction);
}

std::unique_ptr<Instruction> ExecuteStage::getInstructionToExecute() {
    return std::move(instruction_to_execute);
}


void ExecuteStage::startExecution(ExecuteEngine& executeEngine) 
{
    // This is a placeholder implementation and should be replaced with actual logic
    DEBUG_LOG(debugLog("Starting execution of instruction..."));
    if (instruction_to_execute) {
        if (instruction_to_execute->isEmpty()) 
        {
            DEBUG_LOG(debugLog("Instruction is empty. No execution needed."));
            return;
        }
        executeEngine.sendExecutionRequest(peekInstruction());
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of MemoryStage class methods

MemoryStage::MemoryStage() : Stage(), instruction_to_memory(std::make_unique<Instruction>()){}


void MemoryStage::setInstructionToMemory(std::unique_ptr<Instruction> instruction) 
{
    instruction_to_memory = std::move(instruction);
}

std::unique_ptr<Instruction> MemoryStage::getInstructionToMemory()
{
    return std::move(instruction_to_memory);
}


void MemoryStage::requestMemoryAccess(ExecuteEngine& executeEngine) 
{
    DEBUG_LOG(debugLog("Requesting memory access for instruction..."));
    if (instruction_to_memory) 
    {
        if (instruction_to_memory->isEmpty()) 
        {
            DEBUG_LOG(debugLog("Instruction is empty. No memory access needed."));
            return;
        }
        
        executeEngine.sendMemoryAccessRequest(peekInstruction());
    } 
    
}


////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of WriteBackStage class methods

WriteBackStage::WriteBackStage() : Stage(), instruction_to_writeback(std::make_unique<Instruction>()) {}

void WriteBackStage::setInstructionToWriteBack(std::unique_ptr<Instruction> instruction) 
{
    instruction_to_writeback = std::move(instruction);
}

std::unique_ptr<Instruction> WriteBackStage::getInstructionToWriteBack() 
{
    return std::move(instruction_to_writeback);
}

void WriteBackStage::writeBack(ExecuteEngine& executeEngine) 
{
    DEBUG_LOG(debugLog("Write-Back stage processing..."));
    if (instruction_to_writeback) {
        if (instruction_to_writeback->isEmpty()) {
            DEBUG_LOG(debugLog("Instruction is empty. No write-back needed."));
            return;
        }
        executeEngine.sendWriteBackRequest(peekInstruction());
    } else {
        DEBUG_LOG(debugLog("No instruction to write back."));
    }
}


///////////////////////////////////////////////////////////////////////////////

//implementation of the pipeline class

Pipeline::Pipeline(CPU& cpu, PipelineEventHandler* eventHandler) : cpu(cpu), executeEngine(cpu, cpu.getRegisters(), cpu.getALU(), cpu.getCacheManager(), eventHandler), fetchStage(), decodeStage(), executeStage(), memoryStage(), writeBackStage(), eventHandler(eventHandler) 
{
}

void Pipeline::tick() 
{
    ticks_progress++;
    DEBUG_LOG(debugLog("Device ticked, progress: " + std::to_string(ticks_progress) + "/" + std::to_string(getTicksNeeded())));
    if (ticks_progress >= getTicksNeeded()) {
        execute_operation();
        ticks_progress = 0; // Reset progress after operation is executed
    }
    executeEngine.tick(); // Tick the execute engine to process any ongoing operations
}
                               
void Pipeline::execute_operation() 
{
    // Implementation of pipeline operation execution for the current cycle
    
    //controllare se buffer inetrmedi sono validi e non stalled prima di spostare le istruzioni tra le stage
    DEBUG_LOG(debugLog("Executing pipeline operation for the current cycle..."));

    processWriteBackStage();

    processMemoryStage();

    processExecuteStage();

    processOperandFetchStage();

    processDecodeStage();

    processFetchStage();
    
}

void Pipeline::setEventHandler(PipelineEventHandler* handler)
{
    eventHandler = handler; 
    executeEngine.setPipelineEventHandler(handler); 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processWriteBackStage() 
{
    if(writeBackStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("WRITEBACK STAGE processing..."));
        if(memoryWriteBackBuffer.valid) 
        {
            DEBUG_LOG(debugLog("Memory-WriteBack buffer has valid instruction."));
            writeBackStage.setInstructionToWriteBack(std::move(memoryWriteBackBuffer.memoryAccessedInstruction));
            memoryWriteBackBuffer.valid = false;
            #ifdef GUI_ENABLED
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            writeBackStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            #else
            writeBackStage.writeBack(executeEngine);
            #endif

        }
        else if (memoryStage.isStageReady() && !memoryStage.isInstructionEmpty(memoryStage.peekInstruction()))
        {
            DEBUG_LOG(debugLog("Memory stage has valid instruction."));
            writeBackStage.setInstructionToWriteBack(memoryStage.getInstructionToMemory());
            #ifdef GUI_ENABLED
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            writeBackStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            #else
            writeBackStage.writeBack(executeEngine);
            #endif

        }
        else 
        {
            DEBUG_LOG(debugLog("WriteBack stage has no instruction to process."));
        }
    }
    #ifdef GUI_ENABLED
    else if(writeBackStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            DEBUG_LOG(debugLog("WriteBack stage is waiting for GUI buffer update."));
            writeBackStage.writeBack(executeEngine);

    }
    else if(writeBackStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            writeBackStage.setStalledGUI(false); // Reset the stalled flag
            writeBackStage.setInstructionToWriteBack(nullptr); //reset the instruction of write back stage
            writeBackStage.setStatus(StageStatus::READY); // Set the status to ready for the next instruction
    }
    #endif
    else 
    {
        DEBUG_LOG(debugLog("WRITEBACK STAGE is not ready."));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void Pipeline::processMemoryStage() 
{
     if(memoryStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("MEMORY STAGE processing..."));
        if(executeMemoryBuffer.valid) 
        {
            DEBUG_LOG(debugLog("Execute-Memory buffer has valid instruction."));
            memoryStage.setInstructionToMemory(std::move(executeMemoryBuffer.executedInstruction));
            executeMemoryBuffer.valid = false;
            #ifdef GUI_ENABLED
            memoryStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            memoryStage.setStalledGUI(true); // Set the stalled flag
            #else
            memoryStage.requestMemoryAccess(executeEngine);
            #endif
        }
        else if (executeStage.isStageReady() && !executeStage.isInstructionEmpty(executeStage.peekInstruction()))
        {
            DEBUG_LOG(debugLog("Execute stage has valid instruction."));
            memoryStage.setInstructionToMemory(executeStage.getInstructionToExecute());
            #ifdef GUI_ENABLED
            memoryStage.setStalledGUI(true); // Set the stalled flag
            memoryStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            #else
            memoryStage.requestMemoryAccess(executeEngine);
            #endif

        }
        else 
        {
            DEBUG_LOG(debugLog("Memory stage has no instruction to process."));
        }
    }
    #ifdef GUI_ENABLED
    else if(memoryStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Memory stage is waiting for GUI buffer update."));
            memoryStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            memoryStage.requestMemoryAccess(executeEngine);
            memoryStage.setStalledGUI(false); // Set the stalled flag

    }
    else if(memoryStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            DEBUG_LOG(debugLog("Memory stage is waiting for GUI execution to complete."));
            memoryWriteBackBuffer.memoryAccessedInstruction = memoryStage.getInstructionToMemory();
            memoryWriteBackBuffer.valid = true;
            memoryWriteBackBuffer.stalled = false;
            memoryWriteBackBuffer.flushed = false;
            memoryStage.setInstructionToMemory(nullptr); //reset the instruction of write back stage
            memoryStage.setStatus(StageStatus::READY); // Set the status to waiting for GUI buffer update
            memoryStage.setStalledGUI(false); // Set the stalled flag
    }

    #endif
    else if (memoryStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        DEBUG_LOG(debugLog("MEMORY STAGE is waiting for memory operation to complete."));


    }
    else if (memoryStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        DEBUG_LOG(debugLog("MEMORY STAGE memory operation completed."));

    }
    else 
    {
        DEBUG_LOG(debugLog("MEMORY STAGE is not ready."));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processExecuteStage()
{
      if(executeStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE processing..."));
        if(operandFetchExecuteBuffer.valid) 
        {
            DEBUG_LOG(debugLog("OperandFetch-Execute buffer has valid instruction."));
            executeStage.setInstructionToExecute(std::move(operandFetchExecuteBuffer.instructionWithOperands));
            operandFetchExecuteBuffer.valid = false;
            #ifdef GUI_ENABLED
            executeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            executeStage.setStalledGUI(true); // Set the stalled flag
            #else
            executeStage.startExecution(executeEngine);
            #endif
        }
        else if (operandFetchStage.isStageReady() && !operandFetchStage.isInstructionEmpty(operandFetchStage.peekInstruction()))
        {
            DEBUG_LOG(debugLog("Operand Fetch stage has valid instruction."));
            executeStage.setInstructionToExecute(operandFetchStage.getInstructionWithFetchedOperands());
            #ifdef GUI_ENABLED
            executeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            executeStage.setStalledGUI(true); // Set the stalled flag
            #else
            executeStage.startExecution(executeEngine);
            #endif
        }
        else 
        {
            DEBUG_LOG(debugLog("Execute stage has no instruction to process."));
        }
    }
    #ifdef GUI_ENABLED
    else if(executeStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Execute stage is waiting for GUI buffer update."));
            executeStage.startExecution(executeEngine);
            executeStage.setStalledGUI(false); // Set the stalled flag

    }
    else if(executeStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            DEBUG_LOG(debugLog("Execute stage is waiting for GUI execution to complete."));
            executeMemoryBuffer.executedInstruction = executeStage.getInstructionToExecute();
            executeMemoryBuffer.valid = true;
            executeMemoryBuffer.stalled = false;
            executeMemoryBuffer.flushed = false;
            executeStage.setStalledGUI(false); // Set the stalled flag
            executeStage.setStatus(StageStatus::READY); // Set the status to waiting for GUI buffer update
    }
    #endif
    else if (executeStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE is waiting for instruction execution to complete."));

    }
    else if (executeStage.getStatus() == StageStatus::MEMORY_DONE)
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE instruction execution completed."));
    }
    else 
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE is not ready."));
    }


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processOperandFetchStage()
{
    if(operandFetchStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("OPERAND FETCH STAGE processing..."));
        if(decodeOperandFetchBuffer.valid) 
        {
            DEBUG_LOG(debugLog("Decoding-OperandFetch buffer has valid instruction."));
            operandFetchStage.setInstructionWithFetchedOperands(std::move(decodeOperandFetchBuffer.decodedInstruction));
            decodeOperandFetchBuffer.valid = false;
            #ifdef GUI_ENABLED
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            operandFetchStage.setStalledGUI(true); // Set the stalled flag
            #else
            operandFetchStage.fetchOperands(executeEngine);
            #endif   
        }
        else if (decodeStage.isStageReady() && !decodeStage.isInstructionEmpty(decodeStage.peekInstruction()))
        {
            DEBUG_LOG(debugLog("Decode stage has valid instruction."));
            operandFetchStage.setInstructionWithFetchedOperands(decodeStage.getDecodedInstruction());
            #ifdef GUI_ENABLED
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            operandFetchStage.setStalledGUI(true); // Set the stalled flag
            #else
            operandFetchStage.fetchOperands(executeEngine);
            #endif

        }
        else 
        {
            DEBUG_LOG(debugLog("Operand Fetch stage has no instruction to process."));
        }

    }
    #ifdef GUI_ENABLED
    else if(operandFetchStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Operand Fetch stage is waiting for GUI buffer update."));
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            operandFetchStage.fetchOperands(executeEngine);
            operandFetchStage.setStalledGUI(true); // Set the stalled flag

    }
    else if(operandFetchStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            DEBUG_LOG(debugLog("Operand Fetch stage is waiting for GUI execution to complete."));
            operandFetchExecuteBuffer.instructionWithOperands = operandFetchStage.getInstructionWithFetchedOperands();
            operandFetchExecuteBuffer.valid = true;
            operandFetchExecuteBuffer.stalled = false;
            operandFetchExecuteBuffer.flushed = false;
            operandFetchStage.setInstructionWithFetchedOperands(nullptr); //reset the instruction of write back stage
            operandFetchStage.setStatus(StageStatus::READY); // Set the status to ready for the next instruction
            operandFetchStage.setStalledGUI(false); // Set the stalled flag
    }
    #endif
    else 
    {
        DEBUG_LOG(debugLog("OPERAND FETCH STAGE is not ready."));
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processDecodeStage()
{
    if(decodeStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("DECODE STAGE processing..."));
        if(fetchDecodeBuffer.valid) 
        {
            DEBUG_LOG(debugLog("Fetch-Decode buffer has valid instruction."));
            decodeStage.setInstructionToDecode(fetchDecodeBuffer.instructionInfo);
            fetchDecodeBuffer.valid = false;
            #ifdef GUI_ENABLED
            decodeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            decodeStage.setStalledGUI(true); // Set the stalled flag
            #else
            decodeStage.decodeInstruction(cpu, *eventHandler);
            #endif
        }
        else if (fetchStage.isStageReady() && fetchStage.getCurrentInstructionInfo().instruction.size() > 0)
        {
            decodeStage.setInstructionToDecode(fetchStage.getCurrentInstructionInfo());
            #ifdef GUI_ENABLED
            decodeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status  
            decodeStage.setStalledGUI(true); // Set the stalled flag
            #else
            decodeStage.decodeInstruction(cpu, *eventHandler);
            #endif
        }
        else 
        {
            DEBUG_LOG(debugLog("Decode stage has no instruction to process."));
        }
    }
    #ifdef GUI_ENABLED
    else if(decodeStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Decode stage is waiting for GUI buffer update."));
            decodeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            decodeStage.decodeInstruction(cpu, *eventHandler);
            decodeStage.setStalledGUI(false); // Set the stalled flag

    }
    else if(decodeStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            DEBUG_LOG(debugLog("Decode stage is waiting for GUI execution to complete."));
            decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
            decodeOperandFetchBuffer.valid = true;
            decodeOperandFetchBuffer.stalled = false;
            decodeOperandFetchBuffer.flushed = false;
            decodeStage.setStalledGUI(false); // Set the stalled flag
            decodeStage.setInstructionToDecode(InstructionInfo{}); //reset the instruction of write back stage
            decodeStage.setStatus(StageStatus::READY); // Set the status to ready for the next instruction
    }
    #endif
    else
    {
        DEBUG_LOG(debugLog("DECODE STAGE is not ready."));
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::processFetchStage()
{
      if(fetchStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("FETCH STAGE processing..."));
        cpu.incrementInstructionIdCounter();
        DEBUG_LOG(debugLog("Incrementing counter"));
        FetchstageInstructionId = cpu.getInstructionIdCounter();
        DEBUG_LOG(debugLog("FetchstageInstructionId: " + std::to_string(FetchstageInstructionId)));
        
        fetchStage.startFetch(cpu, FetchstageInstructionId, index, *eventHandler); //the event handler pointer is tranformed to reference for the function call
        DEBUG_LOG(debugLog("FETCH STAGE fetch started."));
        
    }
    else if (fetchStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        DEBUG_LOG(debugLog("FETCH STAGE is waiting for instruction fetch to complete."));

        fetchStage.updateFetch(cpu, FetchstageInstructionId, *eventHandler);
    }
    else if (fetchStage.getStatus() == StageStatus::MEMORY_DONE)
    { 
        DEBUG_LOG(debugLog("FETCH STAGE instruction fetch completed."));

        // Move instruction to Fetch-Decode buffer
        fetchDecodeBuffer.instructionInfo = fetchStage.fetchInstruction(cpu, FetchstageInstructionId, index, *eventHandler);
        
    }
    #ifdef GUI_ENABLED
    else if(fetchStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            fetchDecodeBuffer.valid = true;
            fetchDecodeBuffer.stalled = false;
            fetchDecodeBuffer.flushed = false;
            fetchStage.setCurrentInstructionInfo(InstructionInfo()); //reset the instructionInfo of fetch stage
            fetchStage.setStatus(StageStatus::READY); // Set the status to ready for the next instruction
            fetchStage.setStalledGUI(false); // Set the stalled flag
    }
    else 
    {
        DEBUG_LOG(debugLog("FETCH STAGE is not ready."));
    }
    #endif

}


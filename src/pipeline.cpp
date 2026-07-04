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
            #ifdef GUI_ENABLED
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            writeBackStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            #else
            writeBackStage.writeBack(executeEngine, *eventHandler);
            #endif

        }
        else if (memoryStage.isStageReady() && !memoryStage.isInstructionEmpty(memoryStage.peekInstruction()))
        {
            debugLog("Memory stage has valid instruction.");
            writeBackStage.setInstructionToWriteBack(memoryStage.getInstructionToMemory());
            #ifdef GUI_ENABLED
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            writeBackStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            #else
            writeBackStage.writeBack(executeEngine, *eventHandler);
            #endif

        }
        else 
        {
            debugLog("WriteBack stage has no instruction to process.");
        }
    }
    #ifdef GUI_ENABLED
    else if(writeBackStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            debugLog("WriteBack stage is waiting for GUI buffer update.");
            writeBackStage.writeBack(executeEngine, *eventHandler);

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
            #ifdef GUI_ENABLED
            memoryStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            memoryStage.setStalledGUI(true); // Set the stalled flag
            #else
            memoryStage.requestMemoryAccess(executeEngine, *eventHandler);
            #endif
        }
        else if (executeStage.isStageReady() && !executeStage.isInstructionEmpty(executeStage.peekInstruction()))
        {
            debugLog("Execute stage has valid instruction.");
            memoryStage.setInstructionToMemory(executeStage.getInstructionToExecute());
            #ifdef GUI_ENABLED
            memoryStage.setStalledGUI(true); // Set the stalled flag
            memoryStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            #else
            memoryStage.requestMemoryAccess(executeEngine, *eventHandler);
            #endif

        }
        else 
        {
            debugLog("Memory stage has no instruction to process.");
        }
    }
    #ifdef GUI_ENABLED
    else if(memoryStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            debugLog("Memory stage is waiting for GUI buffer update.");
            memoryStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            memoryStage.requestMemoryAccess(executeEngine, *eventHandler);
            memoryStage.setStalledGUI(false); // Set the stalled flag

    }
    else if(memoryStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            debugLog("Memory stage is waiting for GUI execution to complete.");
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
            #ifdef GUI_ENABLED
            executeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            executeStage.setStalledGUI(true); // Set the stalled flag
            #else
            executeStage.startExecution(executeEngine,*eventHandler);
            #endif
        }
        else if (operandFetchStage.isStageReady() && !operandFetchStage.isInstructionEmpty(operandFetchStage.peekInstruction()))
        {
            debugLog("Operand Fetch stage has valid instruction.");
            executeStage.setInstructionToExecute(operandFetchStage.getInstructionWithFetchedOperands());
            #ifdef GUI_ENABLED
            executeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            executeStage.setStalledGUI(true); // Set the stalled flag
            #else
            executeStage.startExecution(executeEngine, *eventHandler);
            #endif
        }
        else 
        {
            debugLog("Execute stage has no instruction to process.");
        }
    }
    #ifdef GUI_ENABLED
    else if(executeStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            debugLog("Execute stage is waiting for GUI buffer update.");
            executeStage.startExecution(executeEngine, *eventHandler);
            executeStage.setStalledGUI(false); // Set the stalled flag

    }
    else if(executeStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            debugLog("Execute stage is waiting for GUI execution to complete.");
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
            #ifdef GUI_ENABLED
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            operandFetchStage.setStalledGUI(true); // Set the stalled flag
            #else
            operandFetchStage.fetchOperands(executeEngine, *eventHandler);
            #endif   
        }
        else if (decodeStage.isStageReady() && !decodeStage.isInstructionEmpty(decodeStage.peekInstruction()))
        {
            debugLog("Decode stage has valid instruction.");
            operandFetchStage.setInstructionWithFetchedOperands(decodeStage.getDecodedInstruction());
            #ifdef GUI_ENABLED
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
            operandFetchStage.setStalledGUI(true); // Set the stalled flag
            #else
            operandFetchStage.fetchOperands(executeEngine, *eventHandler);
            #endif

        }
        else 
        {
            debugLog("Operand Fetch stage has no instruction to process.");
        }

    }
    #ifdef GUI_ENABLED
    else if(operandFetchStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            debugLog("Operand Fetch stage is waiting for GUI buffer update.");
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            operandFetchStage.fetchOperands(executeEngine, *eventHandler);
            operandFetchStage.setStalledGUI(true); // Set the stalled flag

    }
    else if(operandFetchStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            debugLog("Operand Fetch stage is waiting for GUI execution to complete.");
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
            debugLog("Decode stage has no instruction to process.");
        }
    }
    #ifdef GUI_ENABLED
    else if(decodeStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            debugLog("Decode stage is waiting for GUI buffer update.");
            decodeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            decodeStage.decodeInstruction(cpu, *eventHandler);
            decodeStage.setStalledGUI(false); // Set the stalled flag

    }
    else if(decodeStage.getStatus() == StageStatus::WAITING_GUI_EXECUTION)
    {
            debugLog("Decode stage is waiting for GUI execution to complete.");
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
        debugLog("FETCH STAGE is not ready.");
    }
    #endif

}


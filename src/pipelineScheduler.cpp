#include "pipelineScheduler.hpp"
#include "pipeline.hpp"
#include "cpu.hpp"
#include "eventHandler.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define sendRequestToExecuteEngine(instruction, function, stageName) \
    DEBUG_LOG(debugLog(std::string(stageName) + "stage processing...")); \
    if (instruction) \
    { \
        if (instruction->isEmpty()) \
        { \
            DEBUG_LOG(debugLog("Instruction is empty. No " + std::string(stageName) + " needed.")); \
            return; \
        } \
        function(instruction); \
    } \
    else \
    { \
        DEBUG_LOG(debugLog("No instruction to " + std::string(stageName) + ".")); \
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void PipelineScheduler::startFetch(CPU& cpu) 
{
    DEBUG_LOG(debugLog("Starting instruction fetch..."));
    cpu.getControlUnit().startFetch(fetchStageInstructionId, index, *pipeline.eventHandler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void PipelineScheduler::updateFetch(CPU& cpu) 
{
    DEBUG_LOG(debugLog("Updating instruction fetch..."));
    cpu.getControlUnit().updateFetch(fetchStageInstructionId, *pipeline.eventHandler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

InstructionInfo PipelineScheduler::fetchInstruction(CPU& cpu) 
{
    DEBUG_LOG(debugLog("Fetching instruction..."));
    return cpu.getControlUnit().fetchInstruction(fetchStageInstructionId, index, *pipeline.eventHandler);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineScheduler::decodeInstruction(CPU& cpu)
{
    DEBUG_LOG(debugLog("Decoding instruction..."));
    InstructionInfo& instruction_info_to_decode = pipeline.decodeStage.getInstructionToDecodeRef();
    std::unique_ptr<Instruction>& decoded_instruction = pipeline.decodeStage.getDecodedInstructionRef();
    PipelineEventHandler& eventHandler = *pipeline.eventHandler;
    cpu.getControlUnit().decodeInstruction(instruction_info_to_decode, decoded_instruction, eventHandler);
}

///////////////////////////////////////////////////////////////////////

void PipelineScheduler::sendOperandFetchRequestToExecuteEngine()
{
    OperandFetchStage& operandFetchStage = pipeline.operandFetchStage;
    ExecuteEngine& executeEngine = pipeline.executeEngine;
    Instruction* instructionToFetchOperands = operandFetchStage.peekInstruction();
    sendRequestToExecuteEngine(instructionToFetchOperands, executeEngine.sendOperandFetchRequest, "Operand Fetch");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void PipelineScheduler::sendExecutionRequestToExecuteEngine()
{
    ExecuteStage& executeStage = pipeline.executeStage;
    ExecuteEngine& executeEngine = pipeline.executeEngine;
    Instruction* instructionToExecute = executeStage.peekInstruction();
    sendRequestToExecuteEngine(instructionToExecute, executeEngine.sendExecutionRequest, "Execution");

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineScheduler::sendMemoryAccessRequestToExecuteEngine()
{
    MemoryStage& memoryStage = pipeline.memoryStage;
    ExecuteEngine& executeEngine = pipeline.executeEngine;
    Instruction* instructionToMemory = memoryStage.peekInstruction();
    sendRequestToExecuteEngine(instructionToMemory, executeEngine.sendMemoryAccessRequest, "Memory Access");
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineScheduler::sendWriteBackRequestToExecuteEngine()
{
    WriteBackStage& writeBackStage = pipeline.writeBackStage;
    ExecuteEngine& executeEngine = pipeline.executeEngine;
    Instruction* instructionToWriteBack = writeBackStage.peekInstruction();
    sendRequestToExecuteEngine(instructionToWriteBack, executeEngine.sendWriteBackRequest, "Write Back");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////Macros 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_BUFFER(stage, buffer) \
    stage.isStageReady() && buffer.valid

#define STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_PREVIOUS_STAGE(stage2, stage1) \
    stage2.isStageReady() && stage1.isStageReady() && !stage1.isInstructionEmpty(stage1.peekInstruction())

#define STAGE_HAS_NOT_INSTRUCTION_TO_PROCESS(stage2, stage1, buffer) \
    stage2.isStageReady() && buffer.valid == false && (stage1.isInstructionEmpty(stage1.peekInstruction()) || stage1.isStageReady() == false)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineScheduler::processFetchStage(CPU& cpu) 
{
    FetchStage& fetchStage = pipeline.fetchStage;
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.fetchDecodeBuffer;

    if(fetchStage.isStageReady()) 
    {
        DEBUG_LOG(debugLog("FETCH STAGE processing..."));
        cpu.incrementInstructionIdCounter();
        DEBUG_LOG(debugLog("Incrementing counter"));
        fetchStageInstructionId = cpu.getInstructionIdCounter();
        DEBUG_LOG(debugLog("FetchstageInstructionId: " + std::to_string(fetchStageInstructionId)));
        
        startFetch(cpu); //the event handler pointer is tranformed to reference for the function call
        DEBUG_LOG(debugLog("FETCH STAGE fetch started."));
        
    }
    else if (fetchStage.getStatus() == StageStatus::WAITING_MEMORY)
    {
        DEBUG_LOG(debugLog("FETCH STAGE is waiting for instruction fetch to complete."));

        updateFetch(cpu);
    }
    else if (fetchStage.getStatus() == StageStatus::MEMORY_DONE)
    { 
        DEBUG_LOG(debugLog("FETCH STAGE instruction fetch completed."));

        // Move instruction to Fetch-Decode buffer
        fetchDecodeBuffer.instructionInfo = fetchInstruction(cpu);
        
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void PipelineScheduler::processDecodeStage(CPU& cpu)
{
    DecodeStage& decodeStage = pipeline.decodeStage;
    FetchStage& fetchStage = pipeline.fetchStage;
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.fetchDecodeBuffer;

    if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_BUFFER(decodeStage, fetchDecodeBuffer)) 
    {
        DEBUG_LOG(debugLog("DECODE STAGE processing..."));
        DEBUG_LOG(debugLog("Fetch-Decode buffer has valid instruction."));
        decodeStage.setInstructionToDecode(fetchDecodeBuffer.instructionInfo);
        fetchDecodeBuffer.valid = false;
        #ifdef GUI_ENABLED
        decodeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        decodeStage.setStalledGUI(true); // Set the stalled flag
        #else
        decodeInstruction(cpu);
        #endif
    }
    else if(decodeStage.isStageReady() && fetchStage.isStageReady() && fetchStage.getCurrentInstructionInfo().instruction.size() > 0)
    {
        DEBUG_LOG(debugLog("DECODE STAGE processing..."));
        DEBUG_LOG(debugLog("Fetch stage has valid instruction."));
        decodeStage.setInstructionToDecode(fetchStage.getCurrentInstructionInfo());
        #ifdef GUI_ENABLED
        decodeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status  
        decodeStage.setStalledGUI(true); // Set the stalled flag
        #else
        decodeInstruction(cpu);
        #endif
    }
    else if(!decodeStage.isStageReady() && !fetchDecodeBuffer.valid && (fetchStage.getCurrentInstructionInfo().instruction.size() == 0 || !fetchStage.isStageReady()))
    {
        DEBUG_LOG(debugLog("DECODE STAGE has no valid instruction to process."));
    }
    #ifdef GUI_ENABLED
    else if(decodeStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Decode stage is waiting for GUI buffer update."));
            decodeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            decodeInstruction(cpu);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void PipelineScheduler::processOperandFetchStage()
{
    OperandFetchStage& operandFetchStage = pipeline.operandFetchStage;
    DecodeStage& decodeStage = pipeline.decodeStage;
    DecodeOperandFetchBuffer& decodeOperandFetchBuffer = pipeline.decodeOperandFetchBuffer;


    if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_BUFFER(operandFetchStage, decodeOperandFetchBuffer)) 
    {
        DEBUG_LOG(debugLog("OPERAND FETCH STAGE processing..."));
        DEBUG_LOG(debugLog("Decode-OperandFetch buffer has valid instruction."));
        operandFetchStage.setInstructionWithFetchedOperands(std::move(decodeOperandFetchBuffer.decodedInstruction));
        decodeOperandFetchBuffer.valid = false;
        #ifdef GUI_ENABLED
        operandFetchStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        operandFetchStage.setStalledGUI(true); // Set the stalled flag
        #else
        sendOperandFetchRequestToExecuteEngine();
        #endif   
    }
    else if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_PREVIOUS_STAGE(operandFetchStage, decodeStage))
    {
        DEBUG_LOG(debugLog("OPERAND FETCH STAGE processing..."));
        DEBUG_LOG(debugLog("Decode stage has valid instruction."));
        operandFetchStage.setInstructionWithFetchedOperands(decodeStage.getDecodedInstruction());
        #ifdef GUI_ENABLED
        operandFetchStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        operandFetchStage.setStalledGUI(true); // Set the stalled flag
        #else
        sendOperandFetchRequestToExecuteEngine();
        #endif

    }
    else if(STAGE_HAS_NOT_INSTRUCTION_TO_PROCESS(operandFetchStage, decodeStage, decodeOperandFetchBuffer))
    {
        DEBUG_LOG(debugLog("OPERAND FETCH STAGE has no valid instruction to process."));
    }
    #ifdef GUI_ENABLED
    else if(operandFetchStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Operand Fetch stage is waiting for GUI buffer update."));
            operandFetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            sendOperandFetchRequestToExecuteEngine();
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineScheduler::processExecuteStage()
{

    ExecuteStage& executeStage = pipeline.executeStage;
    OperandFetchStage& operandFetchStage = pipeline.operandFetchStage;
    OperandFetchExecuteBuffer& operandFetchExecuteBuffer = pipeline.operandFetchExecuteBuffer;

    if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_BUFFER(executeStage, operandFetchExecuteBuffer))
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE processing..."));
        DEBUG_LOG(debugLog("OperandFetch-Execute buffer has valid instruction."));
        executeStage.setInstructionToExecute(std::move(operandFetchExecuteBuffer.instructionWithOperands));
        operandFetchExecuteBuffer.valid = false;
        #ifdef GUI_ENABLED
        executeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        executeStage.setStalledGUI(true); // Set the stalled flag
        #else
        sendExecutionRequestToExecuteEngine();
        #endif

    }
    else if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_PREVIOUS_STAGE(executeStage, operandFetchStage))
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE processing..."));
        DEBUG_LOG(debugLog("Operand Fetch stage has valid instruction."));
        executeStage.setInstructionToExecute(operandFetchStage.getInstructionWithFetchedOperands());
        #ifdef GUI_ENABLED
        executeStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        executeStage.setStalledGUI(true); // Set the stalled flag
        #else
        sendExecutionRequestToExecuteEngine();
        #endif

    }
    else if(STAGE_HAS_NOT_INSTRUCTION_TO_PROCESS(executeStage, operandFetchStage, operandFetchExecuteBuffer))
    {
        DEBUG_LOG(debugLog("EXECUTE STAGE has no valid instruction to process."));
    }
    #ifdef GUI_ENABLED
    else if(executeStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Execute stage is waiting for GUI buffer update."));
            sendExecutionRequestToExecuteEngine();
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void PipelineScheduler::processMemoryStage()
{

    MemoryStage& memoryStage = pipeline.memoryStage;
    ExecuteStage& executeStage = pipeline.executeStage;
    ExecuteMemoryBuffer& executeMemoryBuffer = pipeline.executeMemoryBuffer;

    if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_BUFFER(memoryStage, executeMemoryBuffer)) 
    {
        DEBUG_LOG(debugLog("MEMORY STAGE processing..."));
        DEBUG_LOG(debugLog("Execute-Memory buffer has valid instruction."));
        memoryStage.setInstructionToMemory(std::move(executeMemoryBuffer.executedInstruction));
        executeMemoryBuffer.valid = false;
        #ifdef GUI_ENABLED
        memoryStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        memoryStage.setStalledGUI(true); // Set the stalled flag
        #else
        sendMemoryAccessRequestToExecuteEngine();
        #endif
    }
    else if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_PREVIOUS_STAGE(memoryStage, executeStage))
    {
        DEBUG_LOG(debugLog("MEMORY STAGE processing..."));
        DEBUG_LOG(debugLog("Execute stage has valid instruction."));
        memoryStage.setInstructionToMemory(executeStage.getInstructionToExecute());
        #ifdef GUI_ENABLED
        memoryStage.setStalledGUI(true); // Set the stalled flag
        memoryStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        #else
        sendMemoryAccessRequestToExecuteEngine();
        #endif

    }
    else if(STAGE_HAS_NOT_INSTRUCTION_TO_PROCESS(memoryStage, executeStage, executeMemoryBuffer))
    {
        DEBUG_LOG(debugLog("MEMORY STAGE has no valid instruction to process."));
    }
    #ifdef GUI_ENABLED
    else if(memoryStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            DEBUG_LOG(debugLog("Memory stage is waiting for GUI buffer update."));
            memoryStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI execution
            sendMemoryAccessRequestToExecuteEngine();
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineScheduler::processWriteBackStage()
{

    WriteBackStage& writeBackStage = pipeline.writeBackStage;
    MemoryStage& memoryStage = pipeline.memoryStage;
    MemoryWriteBackBuffer& memoryWriteBackBuffer = pipeline.memoryWriteBackBuffer;


    if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_BUFFER(writeBackStage, memoryWriteBackBuffer)) 
    {
        DEBUG_LOG(debugLog("WRITEBACK STAGE processing..."));
        DEBUG_LOG(debugLog("Memory-WriteBack buffer has valid instruction."));
        writeBackStage.setInstructionToWriteBack(std::move(memoryWriteBackBuffer.memoryAccessedInstruction));
        memoryWriteBackBuffer.valid = false;
        #ifdef GUI_ENABLED
        writeBackStage.setStalledGUI(true); // Reset the stalled flag
        writeBackStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        #else
        sendWriteBackRequestToExecuteEngine();
        #endif

    }
    else if(STAGE_HAS_INSTRUCTION_TO_PROCESS_FROM_PREVIOUS_STAGE(writeBackStage, memoryStage))
    {
        DEBUG_LOG(debugLog("WRITEBACK STAGE processing..."));
        DEBUG_LOG(debugLog("Memory stage has valid instruction."));
        writeBackStage.setInstructionToWriteBack(memoryStage.getInstructionToMemory());
        #ifdef GUI_ENABLED
        writeBackStage.setStalledGUI(true); // Reset the stalled flag
        writeBackStage.setStatus(StageStatus::WAITING_GUI_BUFFER1); // Set the status
        #else
        sendWriteBackRequestToExecuteEngine();
        #endif

    }
    else if(STAGE_HAS_NOT_INSTRUCTION_TO_PROCESS(writeBackStage, memoryStage, memoryWriteBackBuffer))
    {
        DEBUG_LOG(debugLog("WRITEBACK STAGE has no valid instruction to process."));
    }
    #ifdef GUI_ENABLED
    else if(writeBackStage.getStatus()== StageStatus::WAITING_GUI_BUFFER1)
    {
            writeBackStage.setStalledGUI(true); // Reset the stalled flag
            DEBUG_LOG(debugLog("WriteBack stage is waiting for GUI buffer update."));
            sendWriteBackRequestToExecuteEngine();

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
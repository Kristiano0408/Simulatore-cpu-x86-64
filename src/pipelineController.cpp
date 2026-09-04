#include "pipelineController.hpp"


PipelineController::PipelineController(Pipeline& pipelineRef)
    : pipeline(pipelineRef)
{
    setupEvents();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void PipelineController::setupEvents()
{
    // Register events for stage completions
    eventHandler.setContext(this); // Set the context for the event handler to this PipelineController instance
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::FETCH_COMPLETE, &PipelineController::onFetchCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::DECODE_COMPLETE, &PipelineController::onDecodeCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE, &PipelineController::onOperandFetchCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE, &PipelineController::onExecuteCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE, &PipelineController::onMemoryStageCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE, &PipelineController::onWriteBackCompleteWrapper);

    // Register events for memory waiting and completion
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING, &PipelineController::onWaitingMemoryWrapperMemoryStage);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE, &PipelineController::onMemoryDoneWrapperMemoryStage);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE, &PipelineController::onWaitingMemoryWrapperExecute);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE, &PipelineController::onMemoryDoneWrapperExecute);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_FETCH, &PipelineController::onWaitingMemoryWrapperFetch);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_FETCH, &PipelineController::onMemoryDoneWrapperFetch);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineController::onWaitingMemory(StageType stage)
{
    Stage* currentStage = pipeline.getStage(stage);

    currentStage->setStatus(StageStatus::WAITING_MEMORY);
}

void PipelineController::onWaitingMemoryWrapperFetch(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onWaitingMemory(StageType::FETCH);
}

void PipelineController::onWaitingMemoryWrapperExecute(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onWaitingMemory(StageType::EXECUTE);
}

void PipelineController::onWaitingMemoryWrapperMemoryStage(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onWaitingMemory(StageType::MEMORY);
}



void PipelineController::onMemoryDone(StageType stage)
{
    Stage* currentStage = pipeline.getStage(stage);

    currentStage->setStatus(StageStatus::MEMORY_DONE);
}

void PipelineController::onMemoryDoneWrapperFetch(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onMemoryDone(StageType::FETCH);
}

void PipelineController::onMemoryDoneWrapperExecute(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onMemoryDone(StageType::EXECUTE);
}

void PipelineController::onMemoryDoneWrapperMemoryStage(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onMemoryDone(StageType::MEMORY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PipelineController::onFetchComplete()
{
    FetchStage& fetchStage = pipeline.getFetchStage();

    #ifdef GUI_ENABLED
    fetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI buffer update
    fetchStage.setStalledGUI(true); // Set the stalled flag
    #else
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.getFetchDecodeBuffer();
    fetchDecodeBuffer.valid = true;
    fetchDecodeBuffer.stalled = false;
    fetchDecodeBuffer.flushed = false;
    fetchStage.setStatus(StageStatus::READY);
    fetchStage.setCurrentInstructionInfo(InstructionInfo());
    #endif
}

void PipelineController::onFetchCompleteWrapper(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onFetchComplete();
}


void PipelineController::onDecodeComplete()
{
    DecodeStage& decodeStage = pipeline.getDecodeStage();
    #ifdef GUI_ENABLED
    decodeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI buffer update
    #else
    DecodeOperandFetchBuffer& decodeOperandFetchBuffer = pipeline.getDecodeOperandFetchBuffer();
    decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
    decodeStage.setInstructionToDecode(InstructionInfo()); //reset the instructionInfo of decode stage
    decodeOperandFetchBuffer.valid = true;
    decodeOperandFetchBuffer.stalled = false;
    decodeOperandFetchBuffer.flushed = false;
    decodeStage.setStatus(StageStatus::READY);
    #endif
}

void PipelineController::onDecodeCompleteWrapper(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onDecodeComplete();
}


void PipelineController::onOperandFetchComplete()
{
    OperandFetchStage& operandFetchStage = pipeline.getOperandFetchStage();
    
    #ifdef GUI_ENABLED
    operandFetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status
    #else
    OperandFetchExecuteBuffer& operandFetchExecuteBuffer = pipeline.getOperandFetchExecuteBuffer();
    operandFetchExecuteBuffer.instructionWithOperands = operandFetchStage.getInstructionWithFetchedOperands();
    operandFetchExecuteBuffer.valid = true;
    operandFetchExecuteBuffer.stalled = false;
    operandFetchExecuteBuffer.flushed = false;
    operandFetchStage.setStatus(StageStatus::READY);
    #endif
}

void PipelineController::onOperandFetchCompleteWrapper(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onOperandFetchComplete();
}


void PipelineController::onExecuteComplete()
{
    ExecuteStage& executeStage = pipeline.getExecuteStage();
    #ifdef GUI_ENABLED
    executeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI buffer update
    executeStage.setStalledGUI(true); // Set the stalled flag
    #else
    ExecuteMemoryBuffer& executeMemoryBuffer = pipeline.getExecuteMemoryBuffer();
    executeMemoryBuffer.executedInstruction = executeStage.getInstructionToExecute();
    executeMemoryBuffer.valid = true;
    executeMemoryBuffer.stalled = false;
    executeMemoryBuffer.flushed = false;
    executeStage.setStatus(StageStatus::READY);
    #endif
}

void PipelineController::onExecuteCompleteWrapper(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onExecuteComplete();
}


void PipelineController::onMemoryStageComplete()
{
    MemoryStage& memoryStage = pipeline.getMemoryStage();

    #ifdef GUI_ENABLED
   
    memoryStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to memory done
    memoryStage.setStalledGUI(true);

    #else
    MemoryWriteBackBuffer& memoryWriteBackBuffer = pipeline.getMemoryWriteBackBuffer();
    memoryWriteBackBuffer.memoryAccessedInstruction = memoryStage.getInstructionToMemory();
    memoryWriteBackBuffer.valid = true;
    memoryWriteBackBuffer.stalled = false;
    memoryWriteBackBuffer.flushed = false;
    memoryStage.setInstructionToMemory(nullptr); //reset the instruction of write back stage
    memoryStage.setStatus(StageStatus::READY); // Set the status to waiting for GUI buffer update
    memoryStage.setStalledGUI(false); // Set the stalled flag
    #endif
}

void PipelineController::onMemoryStageCompleteWrapper(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onMemoryStageComplete();
}

void PipelineController::onWriteBackComplete()
    {
       
    WriteBackStage& writeBackStage = pipeline.getWriteBackStage();
    #ifdef GUI_ENABLED
    writeBackStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to execution done
    #else
    writeBackStage.setStatus(StageStatus::READY);
    writeBackStage.setInstructionToWriteBack(nullptr); //reset the instruction of write back stage
    #endif
   
    
}

void PipelineController::onWriteBackCompleteWrapper(void* context)
{
    PipelineController* controller = static_cast<PipelineController*>(context);
    controller->onWriteBackComplete();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool PipelineController::isPipelineStalledForGUI() const
{
    // Check if any stage is stalled for GUI
    return pipeline.getFetchStage().isStalledGUI() ||
           pipeline.getDecodeStage().isStalledGUI() ||
           pipeline.getOperandFetchStage().isStalledGUI() ||
           pipeline.getExecuteStage().isStalledGUI() ||
           pipeline.getMemoryStage().isStalledGUI() ||
           pipeline.getWriteBackStage().isStalledGUI();
}

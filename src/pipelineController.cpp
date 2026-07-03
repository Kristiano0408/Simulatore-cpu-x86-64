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
    eventHandler.registerEvent(EventHandlerPipelineEventType::FETCH_COMPLETE, [this]() { onFetchComplete(); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::DECODE_COMPLETE, [this]() { onDecodeComplete(); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE, [this]() { onOperandFetchComplete(); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE, [this]() { onExecuteComplete(); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE, [this]() { onMemoryStageComplete(); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE, [this]() { onWriteBackComplete(); });

    // Register events for memory waiting and completion
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_WAITING, [this]() { onWaitingMemory(StageType::MEMORY); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_DONE, [this]() { onMemoryDone(StageType::MEMORY); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE, [this]() { onWaitingMemory(StageType::EXECUTE); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE, [this]() { onMemoryDone(StageType::EXECUTE); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_WAITING_FETCH, [this]() { onWaitingMemory(StageType::FETCH); });
    eventHandler.registerEvent(EventHandlerPipelineEventType::MEMORY_DONE_FETCH, [this]() { onMemoryDone(StageType::FETCH); });

    eventHandler.registerEvent(EventHandlerPipelineEventType::STALL_FOR_GUI, [this]() { stallForGUI(); });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineController::onWaitingMemory(StageType stage)
{
    Stage* currentStage = pipeline.getStage(stage);

    currentStage->setStatus(StageStatus::WAITING_MEMORY);
}

void PipelineController::onMemoryDone(StageType stage)
{
    Stage* currentStage = pipeline.getStage(stage);

    currentStage->setStatus(StageStatus::MEMORY_DONE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PipelineController::onFetchComplete()
{
    FetchStage& fetchStage = pipeline.getFetchStage();
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.getFetchDecodeBuffer();
    fetchDecodeBuffer.valid = true;
    fetchDecodeBuffer.stalled = false;
    fetchDecodeBuffer.flushed = false;
    fetchStage.setStatus(StageStatus::READY);
     //reset the instructionInfo of fetch stage
    fetchStage.setCurrentInstructionInfo(InstructionInfo());
}

void PipelineController::onDecodeComplete()
{
    DecodeStage& decodeStage = pipeline.getDecodeStage();
    DecodeOperandFetchBuffer& decodeOperandFetchBuffer = pipeline.getDecodeOperandFetchBuffer();
    decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
    decodeStage.setInstructionToDecode(InstructionInfo()); //reset the instructionInfo of decode stage
    decodeOperandFetchBuffer.valid = true;
    decodeOperandFetchBuffer.stalled = false;
    decodeOperandFetchBuffer.flushed = false;
    decodeStage.setStatus(StageStatus::READY);
}

void PipelineController::onOperandFetchComplete()
{
    OperandFetchStage& operandFetchStage = pipeline.getOperandFetchStage();
    OperandFetchExecuteBuffer& operandFetchExecuteBuffer = pipeline.getOperandFetchExecuteBuffer();
    operandFetchExecuteBuffer.instructionWithOperands = operandFetchStage.getInstructionWithFetchedOperands();
    operandFetchExecuteBuffer.valid = true;
    operandFetchExecuteBuffer.stalled = false;
    operandFetchExecuteBuffer.flushed = false;
    operandFetchStage.setStatus(StageStatus::READY);
}

void PipelineController::onExecuteComplete()
{
    ExecuteStage& executeStage = pipeline.getExecuteStage();
    ExecuteMemoryBuffer& executeMemoryBuffer = pipeline.getExecuteMemoryBuffer();
    executeMemoryBuffer.executedInstruction = executeStage.getInstructionToExecute();
    executeMemoryBuffer.valid = true;
    executeMemoryBuffer.stalled = false;
    executeMemoryBuffer.flushed = false;
    executeStage.setStatus(StageStatus::READY);
}

void PipelineController::onMemoryStageComplete()
{
    MemoryStage& memoryStage = pipeline.getMemoryStage();
    MemoryWriteBackBuffer& memoryWriteBackBuffer = pipeline.getMemoryWriteBackBuffer();
    memoryWriteBackBuffer.memoryAccessedInstruction = memoryStage.getInstructionToMemory();
    memoryWriteBackBuffer.valid = true;
    memoryWriteBackBuffer.stalled = false;
    memoryWriteBackBuffer.flushed = false;
    memoryStage.setStatus(StageStatus::READY);
}

void PipelineController::onWriteBackComplete()
{
    WriteBackStage& writeBackStage = pipeline.getWriteBackStage();
    writeBackStage.setInstructionToWriteBack(nullptr); //reset the instruction of write back stage
    writeBackStage.setStatus(StageStatus::READY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PipelineController::stallForGUI()
{
    pipeline.setWaitingGUI(true);
}



#include "hazardUnit.hpp"

HazardUnit::HazardUnit(Pipeline& pipeline) : pipelineController(pipeline) , pipeline(pipeline) {

    setupEvents(); // Set up the events for the HazardUnit
};

HazardUnit::~HazardUnit() {};


void HazardUnit::setupEvents()
{
    // Register events for stage completions
    eventHandler.setContext(this); // Set the context for the event handler to this PipelineController instance
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::FETCH_COMPLETE, &HazardUnit::onFetchCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::DECODE_COMPLETE, &HazardUnit::onDecodeCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE, &HazardUnit::onOperandFetchCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE, &HazardUnit::onExecuteCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE, &HazardUnit::onMemoryStageCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE, &HazardUnit::onWriteBackCompleteWrapper);

    // Register events for memory waiting and completion
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING, &HazardUnit::onWaitingMemoryWrapperMemoryStage);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE, &HazardUnit::onMemoryDoneWrapperMemoryStage);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE, &HazardUnit::onWaitingMemoryWrapperExecute);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE, &HazardUnit::onMemoryDoneWrapperExecute);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_FETCH, &HazardUnit::onWaitingMemoryWrapperFetch);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_FETCH, &HazardUnit::onMemoryDoneWrapperFetch);

}


void HazardUnit::onFetchCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onFetchComplete();
}

void HazardUnit::onDecodeCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onDecodeComplete();
}

void HazardUnit::onOperandFetchCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onOperandFetchComplete();
}

void HazardUnit::onExecuteCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onExecuteComplete();
}

void HazardUnit::onMemoryStageCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryStageComplete();
}

void HazardUnit::onWriteBackCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWriteBackComplete();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HazardUnit::onWaitingMemoryWrapperMemoryStage(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWaitingMemory(StageType::MEMORY);
}

void HazardUnit::onWaitingMemoryWrapperFetch(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWaitingMemory(StageType::FETCH);
}

void HazardUnit::onWaitingMemoryWrapperExecute(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWaitingMemory(StageType::EXECUTE);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HazardUnit::onMemoryDoneWrapperFetch(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryDone(StageType::FETCH);
}

void HazardUnit::onMemoryDoneWrapperExecute(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryDone(StageType::EXECUTE);
}

void HazardUnit::onMemoryDoneWrapperMemoryStage(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryDone(StageType::MEMORY);
}





void  HazardUnit::execute_operation() {
    // Implement hazard detection and resolution logic here
    // This function will be called when the HazardUnit is ticked and the ticks_needed is reached
    DEBUG_LOG(debugLog("HazardUnit executing operation for hazard detection and resolution..."));
    // Example: Check for data hazards, control hazards, etc., and take appropriate actions
}
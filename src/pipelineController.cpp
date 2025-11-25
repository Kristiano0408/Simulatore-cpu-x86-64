#include "pipelineController.hpp"


PipelineController::PipelineController(Pipeline& pipelineRef)
    : pipeline(pipelineRef)
{
    setupEvents();
}

void PipelineController::setupEvents()
{
    // Register events for stage completions
    eventHandler.registerEvent("FETCH_COMPLETE", [this]() { onStageComplete(StageType::FETCH); });
    eventHandler.registerEvent("DECODE_COMPLETE", [this]() { onStageComplete(StageType::DECODE); });
    eventHandler.registerEvent("OPERAND_FETCH_COMPLETE", [this]() { onStageComplete(StageType::OPERAND_FETCH); });
    eventHandler.registerEvent("EXECUTE_COMPLETE", [this]() { onStageComplete(StageType::EXECUTE); });
    eventHandler.registerEvent("MEMORY_COMPLETE", [this]() { onStageComplete(StageType::MEMORY); });
    eventHandler.registerEvent("WRITEBACK_COMPLETE", [this]() { onStageComplete(StageType::WRITE_BACK); });

    // Register events for memory waiting and completion
    eventHandler.registerEvent("MEMORY_WAITING", [this]() { onWaitingMemory(StageType::MEMORY); });
    eventHandler.registerEvent("MEMORY_DONE", [this]() { onMemoryDone(StageType::MEMORY); });
    eventHandler.registerEvent("MEMORY_WAITING_EXECUTE", [this]() { onWaitingMemory(StageType::EXECUTE); });
    eventHandler.registerEvent("MEMORY_DONE_EXECUTE", [this]() { onMemoryDone(StageType::EXECUTE); });
    eventHandler.registerEvent("MEMORY_WAITING_FETCH", [this]() { onWaitingMemory(StageType::FETCH); });
    eventHandler.registerEvent("MEMORY_DONE_FETCH", [this]() { onMemoryDone(StageType::FETCH); });

    eventHandler.registerEvent("STALL_FOR_GUI", [this]() { stallForGUI(); });
}


void PipelineController::onStageComplete(StageType stage)
{
    Stage* currentStage = pipeline.getStage(stage);

    currentStage->setStatus(StageStatus::READY);
}

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

void PipelineController::stallForGUI()
{
    pipeline.setWaitingGUI(true);
}


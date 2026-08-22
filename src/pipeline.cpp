#include "pipeline.hpp"
#include "cpu.hpp"
#include "executeEngine.hpp"


// Implementation of Stage class methods

bool Stage::isInstructionEmpty(Instruction* instr) {
    
    return (instr == nullptr) || (instr->isEmpty());
}


//implementation of the pipeline class

Pipeline::Pipeline(CPU& cpuRef, PipelineEventHandler* eventHandlerPtr) : cpu(cpuRef), pipelineScheduler(*this), executeEngine(cpuRef, cpuRef.getRegisters(), cpuRef.getALU(), cpuRef.getCacheManager(), eventHandlerPtr), eventHandler(eventHandlerPtr) 
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

    pipelineScheduler.processWriteBackStage(); // Pass the necessary parameters to processWriteBackStage
    pipelineScheduler.processMemoryStage(); // Pass the necessary parameters to processMemoryStage
    pipelineScheduler.processExecuteStage(); // Pass the necessary parameters to processExecuteStage
    pipelineScheduler.processOperandFetchStage(); // Pass the necessary parameters to processOperandFetchStage
    pipelineScheduler.processDecodeStage(cpu); // Pass the necessary parameters to processDecodeStage
    pipelineScheduler.processFetchStage(cpu); // Pass the necessary parameters to processFetchStage
    
}

void Pipeline::setEventHandler(PipelineEventHandler* handler)
{
    eventHandler = handler; 
    executeEngine.setPipelineEventHandler(handler); 
}

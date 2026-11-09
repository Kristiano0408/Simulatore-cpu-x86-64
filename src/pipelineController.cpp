#include "pipelineController.hpp"


PipelineController::PipelineController(Pipeline& pipelineRef)
    : pipeline(pipelineRef)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7



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

    #ifdef GUI_ENABLED
    fetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI buffer update
    fetchStage.setStalledGUI(true); // Set the stalled flag
    #else
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.getFetchDecodeBuffer();
    if(fetchDecodeBuffer.valid == true)
    {
        fetchStage.setStatus(StageStatus::COMPLETED_BUT_BUFFER_FULL);
        return;
    }
 
    #endif
}




void PipelineController::onDecodeComplete()
{
    DecodeStage& decodeStage = pipeline.getDecodeStage();
    #ifdef GUI_ENABLED
    decodeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI buffer update
    #else
    DecodeOperandFetchBuffer& decodeOperandFetchBuffer = pipeline.getDecodeOperandFetchBuffer();
    if(decodeOperandFetchBuffer.valid == true)
    {
        //buffer pieno 
        decodeStage.setStatus(StageStatus::COMPLETED_BUT_BUFFER_FULL);
        return;
    }
    decodeOperandFetchBuffer.decodedInstruction = decodeStage.getDecodedInstruction();
    decodeStage.setInstructionToDecode(InstructionInfo()); //reset the instructionInfo of decode stage
    decodeOperandFetchBuffer.valid = true;
    decodeOperandFetchBuffer.stalled = false;
    decodeOperandFetchBuffer.flushed = false;
    decodeStage.setStatus(StageStatus::READY);
    #endif
}




void PipelineController::onOperandFetchComplete()
{
    OperandFetchStage& operandFetchStage = pipeline.getOperandFetchStage();
    
    #ifdef GUI_ENABLED
    operandFetchStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status
    #else
    OperandFetchExecuteBuffer& operandFetchExecuteBuffer = pipeline.getOperandFetchExecuteBuffer();
     if(operandFetchExecuteBuffer.valid == true)
    {
        //buffer pieno 
        operandFetchStage.setStatus(StageStatus::COMPLETED_BUT_BUFFER_FULL);
        return;
    }
    operandFetchExecuteBuffer.instructionWithOperands = operandFetchStage.getInstructionWithFetchedOperands();
    operandFetchExecuteBuffer.valid = true;
    operandFetchExecuteBuffer.stalled = false;
    operandFetchExecuteBuffer.flushed = false;
    operandFetchStage.setStatus(StageStatus::READY);
    #endif
}




void PipelineController::onExecuteComplete()
{
    ExecuteStage& executeStage = pipeline.getExecuteStage();
    #ifdef GUI_ENABLED
    executeStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to waiting for GUI buffer update
    executeStage.setStalledGUI(true); // Set the stalled flag
    #else
    ExecuteMemoryBuffer& executeMemoryBuffer = pipeline.getExecuteMemoryBuffer();
    if(executeMemoryBuffer.valid == true)
    {
        //buffer pieno 
        executeStage.setStatus(StageStatus::COMPLETED_BUT_BUFFER_FULL);
        return;
    }
    executeMemoryBuffer.executedInstruction = executeStage.getInstructionToExecute();
    executeMemoryBuffer.valid = true;
    executeMemoryBuffer.stalled = false;
    executeMemoryBuffer.flushed = false;
    executeStage.setStatus(StageStatus::READY);
    #endif
}




void PipelineController::onMemoryStageComplete()
{
    MemoryStage& memoryStage = pipeline.getMemoryStage();

    #ifdef GUI_ENABLED
   
    memoryStage.setStatus(StageStatus::WAITING_GUI_EXECUTION); // Set the status to memory done
    memoryStage.setStalledGUI(true);

    #else
    MemoryWriteBackBuffer& memoryWriteBackBuffer = pipeline.getMemoryWriteBackBuffer();
    if(memoryWriteBackBuffer.valid == true)
    {
        //buffer pieno 
        memoryStage.setStatus(StageStatus::COMPLETED_BUT_BUFFER_FULL);
        return;
    }
    memoryWriteBackBuffer.memoryAccessedInstruction = memoryStage.getInstructionToMemory();
    memoryWriteBackBuffer.valid = true;
    memoryWriteBackBuffer.stalled = false;
    memoryWriteBackBuffer.flushed = false;
    memoryStage.setInstructionToMemory(nullptr); //reset the instruction of write back stage
    memoryStage.setStatus(StageStatus::READY); // Set the status to waiting for GUI buffer update
    memoryStage.setStalledGUI(false); // Set the stalled flag
    #endif
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

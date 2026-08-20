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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of DecodeStage class methods

DecodeStage::DecodeStage() : Stage(), instruction_info_to_decode(), decoded_instruction(std::make_unique<Instruction>()) {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of OperandFetchStage class methods

OperandFetchStage::OperandFetchStage() : Stage(), instruction_with_fetched_operands(std::make_unique<Instruction>()) {}

std::unique_ptr<Instruction> OperandFetchStage::getInstructionWithFetchedOperands() 
{
    return std::move(instruction_with_fetched_operands);
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



///////////////////////////////////////////////////////////////////////////////

//implementation of the pipeline class

Pipeline::Pipeline(CPU& cpu, PipelineEventHandler* eventHandler) : cpu(cpu), pipelineScheduler(*this), executeEngine(cpu, cpu.getRegisters(), cpu.getALU(), cpu.getCacheManager(), eventHandler), fetchStage(), decodeStage(), executeStage(), memoryStage(), writeBackStage(), eventHandler(eventHandler) 
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

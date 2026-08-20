#include "executeEngine.hpp"
#include "cpu.hpp"
#include "alu.hpp"

ExecuteEngine::ExecuteEngine(CPU& cpu, RegisterFile& registerFile, ALU& alu, CacheManager& cacheManager, PipelineEventHandler* pipelineEventHandler)
    : cpu(cpu), registerFile(registerFile), alu(alu), cacheManager(cacheManager), operandEngine(cacheManager, cpu, executeEngineEventHandler), pipelineEventHandler(pipelineEventHandler)
{
    executeEngineEventHandler.setContext(this); // Set the context for the event handler to this ExecuteEngine instance
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::MEMORY_WAITING, &triggerPipelineMemoryWaitingCallback);
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::MEMORY_DONE, &triggerPipelineMemoryDoneExecuteCallback);
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::MEMORY_WAITING_EXECUTE, &triggerPipelineMemoryWaitingExecuteCallback);
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::MEMORY_DONE_EXECUTE, &triggerPipelineMemoryDoneExecuteCallback);
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::OPERAND_COMPLETE_READ_EXECUTION, &completeExecutionCallback);
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::OPERAND_COMPLETE_WRITE_MEMORY, &completeMemoryAccessCallback);
    executeEngineEventHandler.registerExecuteEngineEvent(EventHandlerExecuteEngineEventType::OPERAND_COMPLETE_WRITE_WRITEBACK, &completeWriteBackCallback);
    
}

void ExecuteEngine::resetExecutionState() 
{
    isExecuting = false;
    executionQueue.pop(); // Remove the completed operation from the queue
}

void ExecuteEngine::resetMemoryAccessState() 
{
    isAccessingMemory = false;
    memoryAccessQueue.pop(); // Remove the completed operation from the queue
}

void ExecuteEngine::triggerPipelineMemoryWaitingExecuteCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE);
}

void ExecuteEngine::triggerPipelineMemoryDoneExecuteCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE);
}


void ExecuteEngine::triggerPipelineMemoryWaitingCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING);
}

void ExecuteEngine::triggerPipelineMemoryCompleteCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE);
}

void ExecuteEngine::completeExecutionCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->executeInstruction(engine->getExecutionQueue().front());
    engine->resetExecutionState();
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
}

void ExecuteEngine::completeMemoryAccessCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->accessMemory(engine->getMemoryAccessQueue().front());
    engine->resetMemoryAccessState();
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
}


void ExecuteEngine::completeWriteBackCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExecuteEngine::tick()
{
    ticks_progress++;;
    if (ticks_progress >= getTicksNeeded()) {
        execute_operation();
        ticks_progress = 0; // Reset progress after operation is executed
    }
    operandEngine.tick(); // Tick the operand engine to process any ongoing operations
}

void ExecuteEngine::execute_operation() 
{
    if (!writeBackQueue.empty()) 
    {
        Instruction* instruction = writeBackQueue.front();
        if(instruction == nullptr)
        {
            writeBackQueue.pop(); // Remove the null instruction from the queue
            return;
        }
        writeBackInstruction(instruction);
        writeBackQueue.pop();
    }

    if (!memoryAccessQueue.empty()) 
    {
        Instruction* instruction = memoryAccessQueue.front();
        if(instruction == nullptr)
        {
            memoryAccessQueue.pop(); // Remove the null instruction from the queue
            return;
        }
        requestMemoryAccess(instruction);
    } 

    if (!executionQueue.empty())
    {
        
        
        Instruction* instruction = executionQueue.front();
        if(instruction == nullptr)
        {
            executionQueue.pop(); // Remove the null instruction from the queue
            return;
        }
        DEBUG_LOG(debugLog("ExecuteEngine: Starting execution for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
        startExecution(instruction);
    } 

    // Check if there are any pending operations in the queues
    if (!operandFetchQueue.empty())
    {
        Instruction* instruction = operandFetchQueue.front();
        if(instruction == nullptr)
        {
            operandFetchQueue.pop(); // Remove the null instruction from the queue
            return;
        }
        DEBUG_LOG(debugLog("ExecuteEngine: Fetching operands for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
        fetchOperands(instruction);
        DEBUG_LOG(debugLog("ExecuteEngine: Operands fetched for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
        operandFetchQueue.pop(); // Remove the completed operation from the queue
        DEBUG_LOG(debugLog("ExecuteEngine: Operands fetched for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
    } 
   

   
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExecuteEngine::sendOperandFetchRequest(Instruction* instruction) 
{
    operandFetchQueue.push(instruction);
}

void ExecuteEngine::sendExecutionRequest(Instruction* instruction) 
{
    executionQueue.push(instruction);
}

void ExecuteEngine::sendMemoryAccessRequest(Instruction* instruction) 
{
    memoryAccessQueue.push(instruction);
}

void ExecuteEngine::sendWriteBackRequest(Instruction* instruction) 
{
    writeBackQueue.push(instruction);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void ExecuteEngine::fetchOperands(Instruction* instruction) 
{
    //fetch the operands
    //std::cout << "Fetching operands for Sub Instruction" << std::endl;
    //fetch the operands
    //using switch case to get the operands
    InstructionCore& core = instruction->getCore();
    switch (core.addressingMode)
    {
        case AddressingMode::MR:                     //sub register to R/M
            DEBUG_LOG(debugLog("SUB_MR"));
            operandFetch::fetchMR(instruction, registerFile);
            break;  
        case AddressingMode::RM:                    //sub R/M to register
            DEBUG_LOG(debugLog("SUB_RM"));
            operandFetch::fetchRM(instruction, registerFile);
            break;
        case AddressingMode::MI:                   //sub immediate to memory/register
            DEBUG_LOG(debugLog("SUB_MI"));
            operandFetch::fetchMI(instruction, registerFile);
            break;
        case AddressingMode::I:                  //sub immediate to accumulator
            DEBUG_LOG(debugLog("SUB_I"));
            operandFetch::fetchI(instruction, registerFile);
            break;
        case AddressingMode::OI:
            DEBUG_LOG(debugLog("SUB_OI"));
            operandFetch::fetchOI(instruction, registerFile, core.opcode);
            break;
        case AddressingMode::FD:
            DEBUG_LOG(debugLog("SUB_FD"));
            operandFetch::fetchFD(instruction, registerFile);
            break;
        case AddressingMode::TD:
            DEBUG_LOG(debugLog("SUB_TD"));
            operandFetch::fetchTD(instruction, registerFile);
            break;
        default:
            break;
   }
   DEBUG_LOG(debugLog("ExecuteEngine: Operands fetched for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
   pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE);
   DEBUG_LOG(debugLog("ExecuteEngine: Triggered OPERAND_FETCH_COMPLETE  event for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExecuteEngine::startExecution(Instruction* instruction) 
{   
    if(isExecuting)
    {
        DEBUG_LOG(debugLog("ExecuteEngine is already executing an instruction. Cannot execute another instruction simultaneously with ID " + std::to_string(instruction->getCore().InstructionId)));
        return;
    }
    DEBUG_LOG(debugLog("ExecuteEngine: Starting execution for instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
    isExecuting = true;
    InstructionCore& core = instruction->getCore();
    //setting the size of the operands
    uint8_t bit = instruction->calculating_number_of_bits();

    core.nbit = bit;

    instruction->getSourceOperand()->setSize(bit);
    instruction->getDestinationOperand()->setSize(bit);

    operandEngine.sendReadRequest(instruction, instruction->getSourceOperand(), instruction->getDestinationOperand());



}


void ExecuteEngine::executeInstruction(Instruction* instruction)
{

    DEBUG_LOG(debugLog("ExecuteEngine: Executing instruction with ID " + std::to_string(instruction->getCore().InstructionId)));
    DEBUG_LOG(debugLog("ExecuteEngine: Source Operand Value: " + std::to_string(instruction->getTemporaryValuesRef().srcValue)));
    DEBUG_LOG(debugLog("ExecuteEngine: Destination Operand Value: " + std::to_string(instruction->getTemporaryValuesRef().destValue)));


    InstructionCore& core = instruction->getCore();

    switch(core.executionMode)
    {
        case InstructionExecutionMode::ALU:
            alu.executeOperation(instruction->getTemporaryValuesRef(), core.type, core.nbit);
            break;
        case InstructionExecutionMode::DATA_TRANSFER:
            //nothing to do here, the data transfer is handled by other parts of the execute engine
            break;
        case InstructionExecutionMode::CONTROL_FLOW:
            //executeControlFlowOperation(instruction);
            break;
        case InstructionExecutionMode::SYSTEM:
            //executeSystemOperation(instruction);
            break;
        default:
            break;
    }
    
    DEBUG_LOG(debugLog("Subtraction executed"));
    DEBUG_LOG(debugLog("Result: " + to_string_hex(instruction->getTemporaryValuesRef().resultValue)));
    
}


void ExecuteEngine::requestMemoryAccess(Instruction* instruction) 
{
    if(isAccessingMemory)
    {
        DEBUG_LOG(debugLog("ExecuteEngine is already accessing memory. Cannot access memory for another instruction simultaneously with ID " + std::to_string(instruction->getCore().InstructionId)));
        return;
    }
    isAccessingMemory = true;

    InstructionFlags& flags = instruction->getFlags();

    if(!flags.regToMem)
    {
        DEBUG_LOG(debugLog("No memory access needed for SubInstruction (not register to memory)."));
        resetMemoryAccessState();
        pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
        return;
    }
        
    //writing result back to memory

    operandEngine.sendWriteRequest(instruction, instruction->getDestinationOperand(), instruction->getTemporaryValues().resultValue);


    

}

void ExecuteEngine::accessMemory(Instruction* instruction)
{   
    
    InstructionFlags& flags = instruction->getFlags();
    InstructionCore& core = instruction->getCore();

    if(!flags.regToMem)
        return;
    
    //serching in cache response queue for the result

    cpu.eraseCacheResponseIfFound(core.InstructionId);

    pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
    

}






void ExecuteEngine::writeBackInstruction(Instruction* instruction) 
{
    InstructionFlags& instructionFlags = instruction->getFlags();

    //writing back the result to destination operand if it's register
    if(!instructionFlags.regToReg && !instructionFlags.memToReg)
    {
        DEBUG_LOG(debugLog("getRegToReg(): " + std::to_string(instructionFlags.regToReg)));
        DEBUG_LOG(debugLog("getMemToReg(): " + std::to_string(instructionFlags.memToReg)));
        DEBUG_LOG(debugLog("getRegToMem(): " + std::to_string(instructionFlags.regToMem)));
        DEBUG_LOG(debugLog("No write-back needed for SubInstruction (not register to register or memory to register)."));
        pipelineEventHandler->triggerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE);
        return;
    }

    DEBUG_LOG(debugLog("Writing back result for SubInstruction."));


    //std::cout<< std::is_same_v(*a, RegOperand);
    operandEngine.sendWriteRequest(instruction, instruction->getDestinationOperand(), instruction->getTemporaryValues().resultValue);
    
    //update flags in CPU
    FlagReg& flags = registerFile.getFlags();

    flags.setFlag(Flagbit::ZF, instruction->getTemporaryValues().ZF);
    flags.setFlag(Flagbit::SF, instruction->getTemporaryValues().SF);
    flags.setFlag(Flagbit::OF, instruction->getTemporaryValues().OF);
    flags.setFlag(Flagbit::CF, instruction->getTemporaryValues().CF);
    flags.setFlag(Flagbit::PF, instruction->getTemporaryValues().PF);
    flags.setFlag(Flagbit::AF, instruction->getTemporaryValues().AF);  


}
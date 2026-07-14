#include "executeEngine.hpp"
#include "cpu.hpp"
#include "alu.hpp"



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
    engine->pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE);
}

void ExecuteEngine::triggerPipelineMemoryDoneExecuteCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE);
}

void ExecuteEngine::triggerPipelineExecuteCompleteCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
}


void ExecuteEngine::triggerPipelineMemoryWaitingCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING);
}

void ExecuteEngine::triggerPipelineMemoryCompleteCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE);
}

void ExecuteEngine::completeExecutionCallback(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->executeInstruction(engine->getExecutionQueue().front().instruction);
    engine->resetExecutionState();
    engine->pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
}

void ExecuteEngine::resetMemoryAccessStateWrapper(void* context) 
{
    ExecuteEngine* engine = static_cast<ExecuteEngine*>(context);
    engine->resetMemoryAccessState();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExecuteEngine::execute_operation() 
{
    // Check if there are any pending operations in the queues
    if (!operandFetchQueue.empty())
    {
        Instruction* instruction = operandFetchQueue.front();
        fetchOperands(instruction);
        operandFetchQueue.pop(); // Remove the completed operation from the queue
    } 
    else if (!executionQueue.empty())
    {
        Instruction* instruction = executionQueue.front();
        executeInstruction(instruction);
    } 

    else if (!memoryAccessQueue.empty()) 
    {
        Instruction* instruction = memoryAccessQueue.front();
        requestMemoryAccess(instruction);
    } 
    else if (!writeBackQueue.empty()) 
    {
        Instruction* instruction = writeBackQueue.front();
        writeBackInstruction(instruction);
        writeBackQueue.pop();
    }
}

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

   pipelineEventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExecuteEngine::startExecution(Instruction* instruction) 
{   
    if(isExecuting)
    {
        DEBUG_LOG(debugLog("ExecuteEngine is already executing an instruction. Cannot execute another instruction simultaneously."));
        return;
    }
    isExecuting = true;
    InstructionCore& core = instruction->getCore();
    InstructionFlags& flags = instruction->getFlags();
    //setting the size of the operands
    uint8_t bit = instruction->calculating_number_of_bits();

    core.nbit = bit;

    instruction->getSourceOperand()->setSize(bit);
    instruction->getDestinationOperand()->setSize(bit);

    operandEngine.readOperand(instruction, instruction->getSourceOperand(), executeEngineEventHandler);


    operandEngine.readOperand(instruction, instruction->getDestinationOperand(), executeEngineEventHandler);




}


void ExecuteEngine::executeInstruction(Instruction* instruction)
{
    if(isExecuting)
    {
        DEBUG_LOG(debugLog("ExecuteEngine is already executing an instruction. Cannot execute another instruction simultaneously."));
        return;
    }
    isExecuting = true;


    InstructionCore& core = instruction->getCore();
    InstructionFlags& flags = instruction->getFlags();

    switch(core.executionMode)
    {
        case InstructionExecutionMode::ALU:
            alu.executeOperation(instruction->getTemporaryValuesRef(), core.type);
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


void ExecuteEngine::requestMemoryAccess(Instruction* instruction, PipelineEventHandler& eventHandler) 
{
    InstructionFlags& flags = instruction->getFlags();

    if(!flags.regToMem)
    {
        DEBUG_LOG(debugLog("No memory access needed for SubInstruction (not register to memory)."));
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
        return;
    }
        
    //writing result back to memory
    OperandResult response = operandEngine.writeOperand(instruction, instruction->getDestinationOperand(), instruction->getTemporaryValues().resultValue, eventHandler.getContext(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE));

    if(response.status == OperandStatus::WAITING_MEMORY)
    {
        //set the stage to waiting memory using the callback to the pipeline
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING);
    }
    else if(response.status == OperandStatus::ERROR)
    {
        return;
    }
    else
    {
        
        
    }

}

void ExecuteEngine::accessMemory(Instruction* instruction, PipelineEventHandler& eventHandler)
{   
    
    InstructionFlags& flags = instruction->getFlags();
    InstructionCore& core = instruction->getCore();

    if(!flags.regToMem)
        return;
    
    //serching in cache response queue for the result

    cpu.eraseCacheResponseIfFound(core.InstructionId);

    eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
    

}






void ExecuteEngine::writeBackInstruction(Instruction* instruction, PipelineEventHandler& eventHandler) 
{
    InstructionFlags& instructionFlags = instruction->getFlags();

    //writing back the result to destination operand if it's register
    if(!instructionFlags.regToReg && !instructionFlags.memToReg)
    {
        DEBUG_LOG(debugLog("getRegToReg(): " + std::to_string(instructionFlags.regToReg)));
        DEBUG_LOG(debugLog("getMemToReg(): " + std::to_string(instructionFlags.memToReg)));
        DEBUG_LOG(debugLog("getRegToMem(): " + std::to_string(instructionFlags.regToMem)));
        DEBUG_LOG(debugLog("No write-back needed for SubInstruction (not register to register or memory to register)."));
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE);
        return;
    }

    DEBUG_LOG(debugLog("Writing back result for SubInstruction."));


    //std::cout<< std::is_same_v(*a, RegOperand);

    OperandResult response = operandEngine.writeOperand(instruction, instruction->getDestinationOperand(), instruction->getTemporaryValues().resultValue, nullptr, nullptr);
    

    if(response.status == OperandStatus::ERROR)
    {
        return;
    }
    else
    {

    }
    //update flags in CPU
    FlagReg& flags = registerFile.getFlags();

    flags.setFlag(Flagbit::ZF, instruction->getTemporaryValues().ZF);
    flags.setFlag(Flagbit::SF, instruction->getTemporaryValues().SF);
    flags.setFlag(Flagbit::OF, instruction->getTemporaryValues().OF);
    flags.setFlag(Flagbit::CF, instruction->getTemporaryValues().CF);
    flags.setFlag(Flagbit::PF, instruction->getTemporaryValues().PF);
    flags.setFlag(Flagbit::AF, instruction->getTemporaryValues().AF);  

    eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE);

}
#include "executeEngine.hpp"
#include "cpu.hpp"
#include "alu.hpp"


void ExecuteEngine::fetchOperands(Instruction* instruction, PipelineEventHandler& eventHandler) 
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

   eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE);
}



void ExecuteEngine::startExecution(Instruction* instruction, PipelineEventHandler& eventHandler) 
{   
    InstructionCore& core = instruction->getCore();
    InstructionFlags& flags = instruction->getFlags();
    //setting the size of the operands
    uint8_t bit = instruction->calculating_number_of_bits();

    core.nbit = bit;

    instruction->getSourceOperand()->setSize(bit);
    instruction->getDestinationOperand()->setSize(bit);

    OperandResult response;
    response = operandEngine.readOperand(instruction, instruction->getSourceOperand(), eventHandler.getContext(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE));

    DEBUG_LOG(debugLog(std::to_string(static_cast<int>(response.status))));
    DEBUG_LOG(debugLog("Source operand value: " + to_string_hex(response.value)));
    if(response.status == OperandStatus::ERROR)
    {
        DEBUG_LOG(debugLog("Error occurred while fetching source operand."));
        return;
    }
    else if(response.status == OperandStatus::WAITING_MEMORY)
    {
        //set the stage to waiting memory using the callback to the pipeline
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE);
        flags.waitingSrcOperand = true;
    }
    else
    {
        flags.waitingSrcOperand = false;
        instruction->getTemporaryValuesRef().srcValue = response.value;
    }

    response = operandEngine.readOperand(instruction, instruction->getDestinationOperand(), eventHandler.getContext(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE));


    if(response.status == OperandStatus::ERROR)
    {
        DEBUG_LOG(debugLog("Error occurred while fetching destination operand."));
        return;
    }
    else if(response.status == OperandStatus::WAITING_MEMORY)
    {
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE);
        flags.waitingDestOperand = true;
    }
    else
    {
        flags.waitingDestOperand = false;
        instruction->getTemporaryValuesRef().destValue = response.value;
    }

    if(!flags.waitingSrcOperand && !flags.waitingDestOperand)
    {
        //both operands are ready, we can proceed to execute( non multi-cycle instruction only for non-memory operands)
        executeInstruction(instruction);
        DEBUG_LOG(debugLog("esecuzioen diretta"));
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
    }


}



void ExecuteEngine::updateExecution(Instruction* instruction, PipelineEventHandler& eventHandler) 
{
    OperandResult response;
    InstructionFlags& flags = instruction->getFlags();

    response = operandEngine.readOperand(instruction, instruction->getSourceOperand(), nullptr, nullptr);

    if(response.status == OperandStatus::OK)
    {
        flags.waitingSrcOperand = false;
        instruction->getTemporaryValuesRef().srcValue = response.value;
        
    }
    else if (response.status == OperandStatus::WAITING_MEMORY)
    {
        //still waiting for memory access to complete
        flags.waitingSrcOperand = true;
        return;
    }
    else
    {
        //error occurred while fetching the source operand
        return;
    }
    

    response = operandEngine.readOperand(instruction, instruction->getDestinationOperand(), nullptr, nullptr);

    if(response.status == OperandStatus::OK)
    {
        flags.waitingDestOperand = false;
        instruction->getTemporaryValuesRef().destValue = response.value;
        
    }
    else if (response.status == OperandStatus::WAITING_MEMORY)
    {
        //still waiting for memory access to complete
        flags.waitingDestOperand = true;
        return;
    }
    else
    {
        //error occurred while fetching the destination operand
        return;
    }

    executeInstruction(instruction);
    eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
    
}




void ExecuteEngine::executeInstruction(Instruction* instruction) 
{
    InstructionCore& core = instruction->getCore();
    InstructionFlags& flags = instruction->getFlags();
    DEBUG_LOG(debugLog(toStringTypeofInstruction(core.type) + " instruction execution started."));
    //we have both operands ready, we can proceed to execute the subtraction (first we must visist the variant to get the values)
    if(!flags.waitingSrcOperand && !flags.waitingDestOperand)
    {
        alu.executeOperation(instruction->getTemporaryValuesRef(), core.type);
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
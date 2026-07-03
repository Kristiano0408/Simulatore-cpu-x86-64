#include "executeEngine.hpp"
#include "cpu.hpp"
#include "alu.hpp"


void ExecuteEngine::fetchOperands(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    //fetch the operands
    //std::cout << "Fetching operands for Sub Instruction" << std::endl;
    //fetch the operands
    //using switch case to get the operands
    switch (instruction->getAddressingMode())
    {
        case AddressingMode::MR:                     //sub register to R/M
            debugLog("SUB_MR");
            operandFetch::fetchMR(instruction, registerFile);
            break;  
        case AddressingMode::RM:                    //sub R/M to register
            debugLog("SUB_RM");
            operandFetch::fetchRM(instruction, registerFile);
            break;
        case AddressingMode::MI:                   //sub immediate to memory/register
            debugLog("SUB_MI");
            operandFetch::fetchMI(instruction, registerFile);
            break;
        case AddressingMode::I:                  //sub immediate to accumulator
            debugLog("SUB_I");
            operandFetch::fetchI(instruction, registerFile);
            break;
        case AddressingMode::OI:
            debugLog("SUB_OI");
            operandFetch::fetchOI(instruction, registerFile, instruction->getOpcode());
            break;
        case AddressingMode::FD:
            debugLog("SUB_FD");
            operandFetch::fetchFD(instruction, registerFile);
            break;
        case AddressingMode::TD:
            debugLog("SUB_TD");
            operandFetch::fetchTD(instruction, registerFile);
            break;
        default:
            break;
   }

   eventHandler.triggerEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE);
}



void ExecuteEngine::startExecution(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    //setting the size of the operands
    int bit = instruction->calculating_number_of_bits();

    instruction->setNbit(bit);

    instruction->getSourceOperand()->setSize(bit);
    instruction->getDestinationOperand()->setSize(bit);

    OperandResult response;
    response = operandEngine.readOperand(instruction, instruction->getSourceOperand(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE));

    if(response.status == OperandStatus::ERROR)
    {
        return;
    }
    else if(response.status == OperandStatus::WAITING_MEMORY)
    {
        //set the stage to waiting memory using the callback to the pipeline
        eventHandler.triggerEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE);
        instruction->setWaitingSrcOperand(true);
    }
    else
    {
        instruction->setWaitingSrcOperand(false);
        instruction->getTemporaryValuesRef().srcValue = response.value;
    }

    response = operandEngine.readOperand(instruction, instruction->getDestinationOperand(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE));


    if(response.status == OperandStatus::ERROR)
    {
        
        return;
    }
    else if(response.status == OperandStatus::WAITING_MEMORY)
    {
        eventHandler.triggerEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE);
        instruction->setWaitingDestOperand(true);
    }
    else
    {
        instruction->setWaitingDestOperand(false);
        instruction->getTemporaryValuesRef().destValue = response.value;
    }

    if(!instruction->isWaitingSrcOperand() && !instruction->isWaitingDestOperand())
    {
        //both operands are ready, we can proceed to execute( non multi-cycle instruction only for non-memory operands)
        executeInstruction(instruction);
        debugLog("esecuzioen diretta");
        eventHandler.triggerEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
    }


}



void ExecuteEngine::updateExecution(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{
    OperandResult response;

    response = operandEngine.readOperand(instruction, instruction->getSourceOperand(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE));

    if(response.status == OperandStatus::OK)
    {
        instruction->setWaitingSrcOperand(false);
        instruction->getTemporaryValuesRef().srcValue = response.value;
        
    }
    else if (response.status == OperandStatus::WAITING_MEMORY)
    {
        //still waiting for memory access to complete
        instruction->setWaitingSrcOperand(true);
        return;
    }
    else
    {
        //error occurred while fetching the source operand
        return;
    }
    

    response = operandEngine.readOperand(instruction, instruction->getDestinationOperand(), eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE));

    if(response.status == OperandStatus::OK)
    {
        instruction->setWaitingDestOperand(false);
        instruction->getTemporaryValuesRef().destValue = response.value;
        
    }
    else if (response.status == OperandStatus::WAITING_MEMORY)
    {
        //still waiting for memory access to complete
        instruction->setWaitingDestOperand(true);
        return;
    }
    else
    {
        //error occurred while fetching the destination operand
        return;
    }

    executeInstruction(instruction);
    eventHandler.triggerEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE);
    
}




void ExecuteEngine::executeInstruction(Instruction* instruction) 
{
    debugLog(toStringTypeofInstruction(instruction->getType()) + " instruction execution started.");
    //we have both operands ready, we can proceed to execute the subtraction (first we must visist the variant to get the values)
    if(!instruction->isWaitingSrcOperand() && !instruction->isWaitingDestOperand())
    {
        alu.executeOperation(instruction->getTemporaryValuesRef(), instruction->getType());
    }
    
    
    debugLog("Subtraction executed");
    debugLog("Result: " + to_string_hex(instruction->getTemporaryValuesRef().resultValue));
    
}


void ExecuteEngine::requestMemoryAccess(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler) 
{

    if(!instruction->getRegToMem())
    {
        debugLog("No memory access needed for SubInstruction (not register to memory).");
        eventHandler.triggerEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
        return;
    }
        
    //writing result back to memory
    OperandResult response = operandEngine.writeOperand(instruction, instruction->getDestinationOperand(), instruction->getTemporaryValues().resultValue, eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE));

    if(response.status == OperandStatus::WAITING_MEMORY)
    {
        //set the stage to waiting memory using the callback to the pipeline
        eventHandler.triggerEvent(EventHandlerPipelineEventType::MEMORY_WAITING);
    }
    else if(response.status == OperandStatus::ERROR)
    {
        return;
    }
    else
    {
        
        
    }

}

void ExecuteEngine::accessMemory(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler)
{
    if(!instruction->getRegToMem())
        return;
    
    //serching in cache response queue for the result

    cpu.eraseCacheResponseIfFound(instruction->getInstructionId());

    eventHandler.triggerEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE);
    

}






void ExecuteEngine::writeBackInstruction(Instruction* instruction, EventHandler<EventHandlerPipelineEventType>& eventHandler) {

    //writing back the result to destination operand if it's register
    if(!instruction->getRegToReg() && !instruction->getMemToReg())
    {
        debugLog("getRegToReg(): " + std::to_string(instruction->getRegToReg()));
        debugLog("getMemToReg(): " + std::to_string(instruction->getMemToReg()));
        debugLog("getRegToMem(): " + std::to_string(instruction->getRegToMem()));
        debugLog("No write-back needed for SubInstruction (not register to register or memory to register).");
        eventHandler.triggerEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE);
        return;
    }

    debugLog("Writing back result for SubInstruction.");


    //std::cout<< std::is_same_v(*a, RegOperand);

    OperandResult response = operandEngine.writeOperand(instruction, instruction->getDestinationOperand(), instruction->getTemporaryValues().resultValue, nullptr);
    

    if(response.status == OperandStatus::ERROR)
    {
        return;
    }
    else
    {

    }
    //ubdate flags in CPU
    FlagReg& flags = registerFile.getFlags();

    flags.setFlag(Flagbit::ZF, instruction->getTemporaryValues().ZF);
    flags.setFlag(Flagbit::SF, instruction->getTemporaryValues().SF);
    flags.setFlag(Flagbit::OF, instruction->getTemporaryValues().OF);
    flags.setFlag(Flagbit::CF, instruction->getTemporaryValues().CF);
    flags.setFlag(Flagbit::PF, instruction->getTemporaryValues().PF);
    flags.setFlag(Flagbit::AF, instruction->getTemporaryValues().AF);  

    eventHandler.triggerEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE);

}
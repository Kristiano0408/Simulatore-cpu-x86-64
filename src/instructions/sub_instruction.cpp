#include "../../include/instruction_wrapper.hpp"
#include "../../include/helpers.hpp"
#include "../../include/controlUnit.hpp"
#include "../../include/bus.hpp"
#include "../../include/registerFile.hpp"
#include "../../include/cpu.hpp"
#include "../../include/pipeline.hpp"
#include "../../include/eventHandler.hpp"

//Sub instruction

//fetch the operands
void SubInstruction::fetchOperands(Bus& bus)
{
    //std::cout << "Fetching operands for Sub Instruction" << std::endl;
    //fetch the operands
    //using switch case to get the operands
    switch (getAddressingMode())
    {
        case AddressingMode::MR:                     //sub register to R/M
            debugLog("SUB_MR");
            operandFetch::fetchMR(this, bus);
            break;  

        case AddressingMode::RM:                    //sub R/M to register
            debugLog("SUB_RM");
            operandFetch::fetchRM(this, bus);
            break;
        case AddressingMode::MI:                   //sub immediate to memory/register
            debugLog("SUB_MI");
            operandFetch::fetchMI(this, bus);
            break;
        case AddressingMode::I:                  //sub immediate to accumulator
            debugLog("SUB_I");
            operandFetch::fetchI(this, bus);
            break;
        default:
            break;
   }
}


void SubInstruction::startExecution(Bus& bus, EventHandler& eventHandler)
{
    //setting the size of the operands
    int bit = calculating_number_of_bits();

    setNbit(bit);

    getSourceOperand()->setSize(bit);
    getDestinationOperand()->setSize(bit);

    Result<anydata> response;

    response = getSourceOperand()->getValue(eventHandler.getCallback("MEMORY_DONE_EXECUTE"));

    if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Error getting source operand value: " << response.errorInfo.message << std::endl;
        return;
    }
    else if(!response.success && response.errorInfo.error == ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Warning getting source operand value: " << response.errorInfo.message << std::endl;
        //set the stage to waiting memory using the callback to the pipeline
        eventHandler.triggerEvent("MEMORY_WAITING_EXECUTE");
    }
    else
    {
        tempValues.srcValue = response.data;
    }

    response = getDestinationOperand()->getValue(eventHandler.getCallback("MEMORY_DONE_EXECUTE"));


    if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Error getting destination operand value: " << response.errorInfo.message << std::endl;
        return;
    }
    else if(!response.success && response.errorInfo.error == ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Warning getting destination operand value: " << response.errorInfo.message << std::endl;
        eventHandler.triggerEvent("MEMORY_WAITING_EXECUTE");
    }
    else
    {
        tempValues.destValue = response.data;
    }

    if(!isWaitingSrcOperand() && !isWaitingDestOperand())
    {
        //both operands are ready, we can proceed to execute( non multi-cycle instruction only for non-memory operands)
        execute(bus);
        bus.getCPU().getPipeline().getExecuteMemoryBuffer().executedInstruction = bus.getCPU().getPipeline().getExecuteStage().getInstructionToExecute();
        bus.getCPU().getPipeline().getExecuteMemoryBuffer().valid = true;
        bus.getCPU().getPipeline().getExecuteMemoryBuffer().stalled = false;
        bus.getCPU().getPipeline().getExecuteMemoryBuffer().flushed = false;
    }


}

void SubInstruction::updateExecution(Bus& bus, [[maybe_unused]] EventHandler& eventHandler)
{
    Result<anydata> response;

    response = getSourceOperand()->getValue(nullptr);

    if(response.success)
    {
        tempValues.srcValue = response.data;
        
    }
    else 
    {
        std::cerr << "Error getting source operand value: " << response.errorInfo.message << std::endl;
        return;
    }
    

    response = getDestinationOperand()->getValue(nullptr);

    if(response.success)
    {
        tempValues.destValue = response.data;
        
    }
    else 
    {
        std::cerr << "Error getting destination operand value: " << response.errorInfo.message << std::endl;
        return;
    }

    execute(bus);
    
}

void SubInstruction::execute([[maybe_unused]] Bus& bus) 
{   
    //we have both operands ready, we can proceed to execute the subtraction (first we must visist the variant to get the values)
    if(!isWaitingSrcOperand() && !isWaitingDestOperand())
    {
        bus.getCPU().getALU().executeOperation(tempValues, typeofInstruction::SUB);
    }
    
    debugLog("Subtraction executed");
    debugLog("Result: " + to_string_hex(std::get<uint64_t>(tempValues.resultValue)));
    
}


void SubInstruction::requestMemoryAccess([[maybe_unused]] Bus& bus, EventHandler& eventHandler) 
{
    if(!getRegToMem())
    {
        debugLog("No memory access needed for SubInstruction (not register to memory).");
        return;
    }
        
    //writing result back to memory
    Result<void> response = getDestinationOperand()->setValue(tempValues.resultValue, eventHandler.getCallback("MEMORY_DONE"));

    if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Error writing result to destination operand: " << response.errorInfo.message << std::endl;
        return;
    }
    else if(!response.success && response.errorInfo.error == ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Warning writing result to destination operand: " << response.errorInfo.message << std::endl;
        eventHandler.triggerEvent("MEMORY_WAITING");
    }
    else
    {
        debugLog("Result written to destination operand successfully.");
    }
}

void SubInstruction::accessMemory([[maybe_unused]] Bus& bus) 
{
    if(!getRegToMem())
        return;
    
    //serching in cache response queue for the result

    auto it = bus.getCPU().cacheResponseQueue.find(getInstructionId());

    if (it != bus.getCPU().cacheResponseQueue.end())
    {
        //request completed
        debugLog("SubInstruction: Write request completed for instruction ID " + std::to_string(getInstructionId()) + ".");
        
        //extracting the result
        Result<void> result;
        Result<anydata>& response = *(it->second);

        result.success = response.success;
        result.errorInfo = response.errorInfo;
        
        if (result.success)
        {
            debugLog("SubInstruction: Write request successful for instruction ID " + std::to_string(getInstructionId()) + ".");
            //removing from the queue
            bus.getCPU().cacheResponseQueue.erase(it);
        }
        else
        {
            debugLog("SubInstruction: Write request failed for instruction ID " + std::to_string(getInstructionId()) + ": " + response.errorInfo.message);
        }
    }
    else
    {
        //request not completed //impossible to reach here but for safety
        debugLog("SubInstruction: Write request not completed for instruction ID " + std::to_string(getInstructionId()) + ".");
    }
    


    
    debugLog("Memory access for SubInstruction completed.");

}




void SubInstruction::writeBack([[maybe_unused]] Bus& bus) 
{
    //writing back the result to destination operand if it's register
    if(!getRegToReg() && !getMemToReg())
    {
        debugLog("getRegToReg(): " + std::to_string(getRegToReg()));
        debugLog("getMemToReg(): " + std::to_string(getMemToReg()));
        debugLog("getRegToMem(): " + std::to_string(getRegToMem()));
        debugLog("No write-back needed for SubInstruction (not register to register or memory to register).");
        return;
    }

    debugLog("Writing back result for SubInstruction.");

    Result<void> response = getDestinationOperand()->setValue(tempValues.resultValue, nullptr);

    if(!response.success)
    {
        std::cerr << "Error writing back result to destination operand: " << response.errorInfo.message << std::endl;
        return;
    }
    else
    {
        debugLog("Result written back to destination operand successfully.");
    }


    //ubdate flags in CPU
    FlagReg& flags = bus.getCPU().getRegisters().getFlags();

    flags.setFlag(Flagbit::ZF, tempValues.ZF);
    flags.setFlag(Flagbit::SF, tempValues.SF);
    flags.setFlag(Flagbit::OF, tempValues.OF);
    flags.setFlag(Flagbit::CF, tempValues.CF);
    flags.setFlag(Flagbit::PF, tempValues.PF);
    flags.setFlag(Flagbit::AF, tempValues.AF);  


}



          


    

    



#include "../../include/instruction_wrapper.hpp"
#include "../../include/helpers.hpp"
#include "../../include/controlUnit.hpp"
#include "../../include/bus.hpp"
#include "../../include/registerFile.hpp"
#include "../../include/cpu.hpp"
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


void SubInstruction::startExecution(Bus& bus) 
{
    //setting the size of the operands
    int bit = calculating_number_of_bits();

    setNbit(bit);

    getSourceOperand()->setSize(bit);
    getDestinationOperand()->setSize(bit);

    Result<anydata> response;

    response = getSourceOperand()->getValue();

    if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Error getting source operand value: " << response.errorInfo.message << std::endl;
        return;
    }
    else if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Warning getting source operand value: " << response.errorInfo.message << std::endl;
        bus.getCPU().getPipeline().getExecuteStage().setStatus(StageStatus::WAITING_MEMORY);
        setWaitingSrcOperand(true);
    }
    else
    {
        tempValues.srcValue = response.data;
    }

    response = getDestinationOperand()->getValue();


    if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Error getting destination operand value: " << response.errorInfo.message << std::endl;
        return;
    }
    else if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Warning getting destination operand value: " << response.errorInfo.message << std::endl;
        bus.getCPU().getPipeline().getExecuteStage().setStatus(StageStatus::WAITING_MEMORY);
        setWaitingDestOperand(true);
    }
    else
    {
        tempValues.destValue = response.data;
    }

    if(!isWaitingSrcOperand() && !isWaitingDestOperand())
    {
        //both operands are ready, we can proceed to execute( non multi-cycle instruction only for non-memory operands)
        execute(bus);
    }


}

void SubInstruction::updateExecution(Bus& bus) 
{
    Result<anydata> response;

    if(isWaitingSrcOperand())
    {
        response = getSourceOperand()->getValue();

        if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
        {
            std::cerr << "Error getting source operand value: " << response.errorInfo.message << std::endl;
            return;
        }
        else if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
        {
            std::cerr << "Warning getting source operand value: " << response.errorInfo.message << std::endl;
            bus.getCPU().getPipeline().getExecuteStage().setStatus(StageStatus::WAITING_MEMORY);
            return;
        }
        else
        {
            tempValues.srcValue = response.data;
            setWaitingSrcOperand(false);
        }
    }

    if(isWaitingDestOperand())
    {
        response = getDestinationOperand()->getValue();

        if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
        {
            std::cerr << "Error getting destination operand value: " << response.errorInfo.message << std::endl;
            return;
        }
        else if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
        {
            std::cerr << "Warning getting destination operand value: " << response.errorInfo.message << std::endl;
            bus.getCPU().getPipeline().getExecuteStage().setStatus(StageStatus::WAITING_MEMORY);
            return;
        }
        else
        {
            tempValues.destValue = response.data;
            setWaitingDestOperand(false);
        }
    }

    if(!isWaitingSrcOperand() && !isWaitingDestOperand())
    {
        //both operands are ready, we can proceed to execute
        bus.getCPU().getPipeline().getExecuteStage().setStatus(StageStatus::MEMORY_DONE);
    }
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


void SubInstruction::requestMemoryAccess([[maybe_unused]] Bus& bus) 
{
    if(!getRegToMem())
        return;
    
        
    //writing result back to memory
    Result<void> response = getDestinationOperand()->setValue(tempValues.resultValue);

    if(!response.success && response.errorInfo.error != ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Error writing result to destination operand: " << response.errorInfo.message << std::endl;
        return;
    }
    else if(!response.success && response.errorInfo.error == ErrorType::WAITING_MEMORY)
    {
        std::cerr << "Warning writing result to destination operand: " << response.errorInfo.message << std::endl;
        bus.getCPU().getPipeline().getMemoryStage().setStatus(StageStatus::WAITING_MEMORY);
    }
    else
    {
        debugLog("Result written to destination operand successfully.");
        bus.getCPU().getPipeline().getMemoryStage().setStatus(StageStatus::MEMORY_DONE);
    }
}

void SubInstruction::updateMemoryAccess([[maybe_unused]] Bus& bus) 
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
            bus.getCPU().getPipeline().getMemoryStage().setStatus(StageStatus::MEMORY_DONE);
            accessMemory(bus);
        }
        else
        {
            debugLog("SubInstruction: Write request failed for instruction ID " + std::to_string(getInstructionId()) + ": " + response.errorInfo.message);
            std::cerr << "Error writing result to destination operand: " << response.errorInfo.message << std::endl;
        }
    }
    else
    {
        //request not completed
        debugLog("SubInstruction: Write request not completed for instruction ID " + std::to_string(getInstructionId()) + ".");
        bus.getCPU().getPipeline().getMemoryStage().setStatus(StageStatus::WAITING_MEMORY);
    }
    



}

    

    
void SubInstruction::accessMemory([[maybe_unused]] Bus& bus) 
{
    if(!getRegToMem())
        return;
    
    debugLog("Memory access for SubInstruction completed.");

}

void SubInstruction::writeBack([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}



          


    

    



#include "../../include/instruction_wrapper.hpp"
#include "../../include/helpers.hpp"
#include "../../include/controlUnit.hpp"
#include "../../include/bus.hpp"
#include "../../include/registerFile.hpp"
#include "../../include/cpu.hpp"
#include "../../include/pipeline.hpp"

//Add instruction

//fetch the operands
void AddInstruction::fetchOperands(Bus& bus)
{
    //std::cout << "Fetching operands for Add Instruction" << std::endl;
    //fetch the operands
    //using switch case to get the operands
    switch (getAddressingMode())
    {
        case AddressingMode::MR:                     //add register to R/M
            debugLog("ADD_MR");
            operandFetch::fetchMR(this, bus);
            break;  

        case AddressingMode::RM:                    //add R/M to register
            debugLog("ADD_RM");
            operandFetch::fetchRM(this, bus);
            break;
        case AddressingMode::MI:                   //add immediate to memory/register
            debugLog("ADD_MI");
            operandFetch::fetchMI(this, bus);
            break;
        case AddressingMode::I:                  //add immediate to accumulator
            debugLog("ADD_I");
            operandFetch::fetchI(this, bus);
            break;
        default:
            break;
   }
}


void AddInstruction::startExecution([[maybe_unused]] Bus& bus) 
{

}

void AddInstruction::updateExecution([[maybe_unused]] Bus& bus) 
{
    
}

void AddInstruction::execute([[maybe_unused]] Bus& bus) 
{   
    /*
    //setting the size of the operands
    int bit = calculating_number_of_bits();

    setNbit(bit);

    uint64_t mask = this->mask(bit);

    //setting the size of the operands(it might not be necessary but for know we dont have a geeneic function for fethcing 
    //from memory so we have to set the size of the operands to know what to fetch

    getSourceOperand()->setSize(bit);
    getDestinationOperand()->setSize(bit);

    uint64_t src_value = 0, dst_value = 0, res = 0;

    if(getDestinationOperand() && getSourceOperand())
    {
         //getting the value from the source operand
        src_value = getSourceOperand()->getValue().data & mask;

        if(getOpcode() == 0x83) // it is only encessary to mask the source 
        {
            dst_value = getDestinationOperand()->getValue().data;

            res = bus.getCPU().getALU().add(dst_value, src_value);

             //setting the value to the destination operand
            getDestinationOperand()->setValue(res);
            
        }
        else
        {
            //getting the value from the destination operand
            dst_value = getDestinationOperand()->getValue().data & mask;

            res = bus.getCPU().getALU().add(dst_value, src_value) & mask;


            //setting the value to the destination operand (old destination + result of the addition masked)
            getDestinationOperand()->setValue((getDestinationOperand()->getValue().data & ~mask) | (res & mask));
        }
    }
    else
    {
        std::cerr << "Error: Source or destination operand is null" << std::endl;
    }*/

}

void AddInstruction::requestMemoryAccess([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}
void AddInstruction::updateMemoryAccess([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}
void AddInstruction::accessMemory([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}

void AddInstruction::writeBack([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}

///////////////////////////////////////////////////////////////////////////////////////////
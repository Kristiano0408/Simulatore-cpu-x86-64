
#include "../../include/instruction_wrapper.hpp"
#include "../../include/helpers.hpp"
#include "../../include/controlUnit.hpp"
#include "../../include/bus.hpp"
#include "../../include/registerFile.hpp"
#include "../../include/cpu.hpp"
#include "../../include/pipeline.hpp"
#include "../../include/eventHandler.hpp"   

// Move instruction


//fetch the operands
void MoveInstruction::fetchOperands(Bus& bus) {

    //std::cout << "Fetching operands for Move Instruction" << std::endl;
    debugLog("MoveInstruction startFetchOperands called");

    //getting the opcode
    uint32_t opcode = getOpcode();

    debugLog("Opcode: " + to_string_hex(opcode));




    //fetch the operands



    
    //using switch case to get the operands
    switch (getAddressingMode())
    {
        case AddressingMode::MR:                     //move register to R/M
            debugLog("MOV_MR");
            operandFetch::fetchMR(this, bus);
            break;
        
        case AddressingMode::RM:                    //move R/M to register
            debugLog("MOV_RM");
            //std::cout << "opcode: " << opcode << std::endl;
            operandFetch::fetchRM(this, bus);
            break;
        
        
        case AddressingMode::MI:                   //move immediate to memory/register
            debugLog("MOV_MI");
            operandFetch::fetchMI(this, bus);
            break;
        
        case AddressingMode::OI:                  //move immediate to reg
            debugLog("MOV_OI");
            operandFetch::fetchOI(this, bus, getOpcode());
            break;
        
        case AddressingMode::FD:                     //move from offset to Rax
            debugLog("MOV_FD");
            operandFetch::fetchFD(this, bus);
            break;
        
        case AddressingMode::TD:                    //move from Rax to offset
            debugLog("MOV_TD");
            operandFetch::fetchTD(this, bus);
            break;
        
        default:
            break;
    }


}

void MoveInstruction::startExecution([[maybe_unused]] Bus& bus, [[maybe_unused]] EventHandler& eventHandler) 
{

}

void MoveInstruction::updateExecution([[maybe_unused]] Bus& bus, [[maybe_unused]] EventHandler& eventHandler) 
{
    
}

void MoveInstruction::execute([[maybe_unused]] Bus& bus) 
{

    /*//setting the size of the operands
    int bit = calculating_number_of_bits();

    setNbit(bit);


    //setting the size of the operands(it might not be necessary but for know we dont have a geeneic function for fethcing 
    //from memory so we have to set the size of the operands to know what to fetch from memory)

    

    getSourceOperand()->setSize(bit);
    getDestinationOperand()->setSize(bit);





    if(getDestinationOperand() && getSourceOperand())
    {
         //getting the value from the source operand
        uint64_t value = getSourceOperand()->getValue().data;

        //casting the value to the number of bits of the operand (8, 16, 32, 64) and zero extending it
        value = castingValue(value, getNbit());

        debugLog("Value: " + to_string_hex(value));

        //setting the value to the destination operand
        getDestinationOperand()->setValue(value);

    }
    else
    {
        debugLog("Error: Source or destination operand is null");
    }

    //std::cout<< "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ"<< std::endl;

    //delete sourceOperand; // delete the source operand after use
    //delete destinationOperand; // delete the destination operand after use

    //sourceOperand = nullptr; // set the pointer to null after deletion
    //destinationOperand = nullptr; // set the pointer to null after deletion

*/
}


void MoveInstruction::requestMemoryAccess([[maybe_unused]] Bus& bus, [[maybe_unused]] EventHandler& eventHandler) 
{
    //default implementation (do nothing)
}
void MoveInstruction::updateMemoryAccess([[maybe_unused]] Bus& bus, [[maybe_unused]] EventHandler& eventHandler) 
{
    //default implementation (do nothing)
}
void MoveInstruction::accessMemory([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}

void MoveInstruction::writeBack([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}
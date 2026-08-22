
#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include "instruction_code_map.hpp"
#include <memory>
#include "operands.hpp"


class Instruction
{
    public:

        //constructor
        Instruction();
        
        ~Instruction() = default;

        bool isEmpty() const; //check if the instruction is empty (no operation)
        

        uint8_t calculatingNumberOfBits(); //calculate the number of bits of the value/operand

         //setters and getters for the operands
        void setSourceOperand(std::unique_ptr<Operand> sOperand);
        Operand* getSourceOperand();
        void setDestinationOperand(std::unique_ptr<Operand> dOperand);

        Operand* getDestinationOperand();
        InstructionCore& getCore() { return core; }
        InstructionFlags& getFlags() { return flags; } 

        void setTemporaryValues(const temporaryValues& values) { tempValues = values; }

        temporaryValues getTemporaryValues()  { return tempValues; }

        temporaryValues& getTemporaryValuesRef() { return tempValues; } // Return a reference to the temporaryValues struct


    protected:
        std::unique_ptr<Operand> sourceOperand; //source operand
        std::unique_ptr<Operand> destinationOperand; //destination operand
    private:
    //parts of the instruction
        
        temporaryValues tempValues;
        InstructionCore core;
        InstructionFlags flags;
        

        




        

};




#endif // INSTRUCTION_HPP    
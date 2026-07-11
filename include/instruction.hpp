
#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "memory.hpp"
#include "addressCalculator.hpp"
#include "instruction_code_map.hpp"
#include <unordered_set>
#include <memory>
#include "operands.hpp"


class Instruction
{
    public:

        //constructor
        Instruction();
        
        ~Instruction() = default;

        bool isEmpty() const; //check if the instruction is empty (no operation)
        
        uint64_t castingValue(uint64_t value, uint8_t nbit); //cast the value to the number of bits of the operand (8, 16, 32, 64)

        uint8_t calculating_number_of_bits(); //calculate the number of bits of the value/operand

        uint64_t mask(uint8_t nbit);// return a mask for the number of bits (8, 16, 32, 64)


         //setters and getters for the operands
        void setSourceOperand(std::unique_ptr<Operand> sourceOperand);
        Operand* getSourceOperand();
        void setDestinationOperand(std::unique_ptr<Operand> destinationOperand);

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
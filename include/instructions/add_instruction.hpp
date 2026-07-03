#ifndef ADD_INSTRUCTION_HPP
#define ADD_INSTRUCTION_HPP

#include "../instruction.hpp"

//ADD instruction class
class AddInstruction : public Instruction
{
    public:
        //destructor
        ~AddInstruction() override = default;

        bool isEmpty() const override {return false;}


};

#endif // ADD_INSTRUCTION_HPP
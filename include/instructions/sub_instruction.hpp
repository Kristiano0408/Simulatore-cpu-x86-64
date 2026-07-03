#ifndef SUB_INSTRUCTION_HPP
#define SUB_INSTRUCTION_HPP

#include "../instruction.hpp"

//SUB instruction class
class SubInstruction : public Instruction
{
    public:
        //destructor
        ~SubInstruction() override = default;

        bool isEmpty() const override { return false; }

};

#endif // SUB_INSTRUCTION_HPP

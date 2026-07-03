#ifndef MOV_INSTRUCTION_HPP
#define MOV_INSTRUCTION_HPP

#include "../instruction.hpp"


class MoveInstruction : public Instruction
{
    public:

        //destructor
        ~MoveInstruction() override = default;

        bool isEmpty() const override { return false; }

        
};
#endif // MOV_INSTRUCTION_HPP

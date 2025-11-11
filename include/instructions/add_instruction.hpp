#ifndef ADD_INSTRUCTION_HPP
#define ADD_INSTRUCTION_HPP

#include "../instruction.hpp"

//ADD instruction class
class AddInstruction : public Instruction
{
    public:
        //destructor
        ~AddInstruction() override = default;

        void fetchOperands(Bus& bus) override;

        //execute the instruction
        void startExecution(Bus& bus) override;
        void updateExecution(Bus& bus) override;
        void execute(Bus& bus) override;

        void accessMemory(Bus& bus) override;

        void writeBack(Bus& bus) override;


};

#endif // ADD_INSTRUCTION_HPP
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

        void fetchOperands(Bus& bus) override;

        //execute the instruction
        void startExecution(Bus& bus) override;
        void updateExecution(Bus& bus) override;
        void execute(Bus& bus) override;

        void requestMemoryAccess(Bus& bus) override;
        void updateMemoryAccess(Bus& bus) override;
        void accessMemory(Bus& bus) override;

        void writeBack(Bus& bus) override;

};

#endif // SUB_INSTRUCTION_HPP

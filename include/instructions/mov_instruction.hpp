#ifndef MOV_INSTRUCTION_HPP
#define MOV_INSTRUCTION_HPP

#include "../instruction.hpp"


class MoveInstruction : public Instruction
{
    public:

        //destructor
        ~MoveInstruction() override = default;

        bool isEmpty() const override { return false; }

        void fetchOperands(Bus& bus) override;

        //execute the instruction
        void startExecution(Bus& bus, EventHandler& eventHandler) override;
        void updateExecution(Bus& bus, EventHandler& eventHandler) override;
        void execute(Bus& bus) override;

        void requestMemoryAccess(Bus& bus, EventHandler& eventHandler) override;
        void accessMemory(Bus& bus) override;

        void writeBack(Bus& bus) override;



        
};
#endif // MOV_INSTRUCTION_HPP

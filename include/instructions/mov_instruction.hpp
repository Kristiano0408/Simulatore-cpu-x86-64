#ifndef MOV_INSTRUCTION_HPP
#define MOV_INSTRUCTION_HPP

#include "../instruction.hpp"


class MoveInstruction : public Instruction
{
    public:

        //destructor
        ~MoveInstruction() override = default;

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
#endif // MOV_INSTRUCTION_HPP

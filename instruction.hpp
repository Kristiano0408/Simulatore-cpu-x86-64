//

#ifndef ISTRUCTION_HPP
#define ISTRUCTION_HPP

#include <cstdint>

class CPU;

class Instruction
{
    public:

        //destructor
        virtual  ~Instruction() = default;
        //execute the instruction
        virtual void execute(CPU* cpu) = 0; //Polimorfic method that will be implemented in the derived classes
};

//define the instruction classes (an iscrutction for each operation)

//add instruction
class AddInstruction : public Instruction
{
    public:
        //constructor
        AddInstruction(uint8_t dest, uint8_t src1, uint8_t src2);
        //destructor
        ~AddInstruction() override = default;
        //execute the instruction
        void execute(CPU* cpu) override;

    private:
        uint8_t dest;
        uint8_t src1;
        uint8_t src2;
};




#endif // ISTRUCTION_HPP    
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
        virtual void execute(CU* controlUnit, Memory* ram);//Polimorfic method that will be implemented in the derived classes
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
        void execute(CU* controlUnit, Memory* ram) override;

    private:
        uint8_t dest;
        uint8_t src1;
        uint8_t src2;
};


class MoveInstruction : public Instruction
{
    public:
        //constructor
        MoveInstruction();
        //destructor
        ~MoveInstruction() override = default;
        //execute the instruction
        void execute(CU* controlUnit, Memory* ram) override;

        void setS_address(int64_t address);
        void setD_address(int64_t address);
        void setValue(int64_t value);

    private:
        int64_t S_address;
        int64_t D_address;
        int64_t value;

       
};

#endif // ISTRUCTION_HPP    
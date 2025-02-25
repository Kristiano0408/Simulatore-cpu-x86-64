
#ifndef ISTRUCTION_HPP
#define ISTRUCTION_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "memory.hpp"

class CU;
class Memory;

class Instruction
{
    public:


        //destructor
        virtual  ~Instruction();
        //execute the instruction
        virtual void execute(CU* controlUnit, Memory* ram);//Polimorfic method that will be implemented in the derived classes

        virtual int64_t getValue() const = 0;

        //setters and getters for the instruction
        void setOpcode(uint32_t opcode);
        uint32_t getOpcode();
        void setPrefix(uint8_t prefix[4]);
        uint8_t* getPrefix();
        void setNumPrefixes(int numPrefixes);
        int getNumPrefixes();
        void setRex(bool rex);
        bool getRex();
        void setRexprefix(int16_t rexprefix);
        int16_t getRexprefix();



    private:
    //parts of the instruction
        uint32_t opcode;
        uint8_t prefix[4];
        int numPrefixes;
        bool rex;
        int16_t rexprefix;
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
        void setD_register(char* registerName);
        void setS_register(char* registerName);
        void setValue(int64_t value);
        int64_t getValue() const  override;

    private:
        int64_t S_address;
        char* S_register;
        char* D_register;
        int64_t value;

       
};

#endif // ISTRUCTION_HPP    
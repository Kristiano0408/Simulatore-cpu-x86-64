
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
        void setRexprefix(int8_t rexprefix);
        int8_t getRexprefix();
        void setNbit(int nbit);
        int getNbit();
        void decodeNbit(); //method to decode the number of bits of the value/operand with rex prefix or 0x66 prefix



    private:
    //parts of the instruction
        uint32_t opcode;
        uint8_t prefix[4];
        int numPrefixes;
        bool rex;
        int8_t rexprefix;
        int nbit; //number of bits of the value/operand

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

        //methods to set the values/addresses
        void setS_address(int64_t address);
        void setD_address(int64_t address);
        void setD_register(const std::string& registerName);
        void setS_register(const std::string& registerName);
        void setValue(int64_t value);
        int64_t getValue() const  override;
        
        //reset the values/addresses
        void reset();

    private:
        int64_t S_address;
        int64_t D_address;
        std::string S_register;
        std::string D_register;
        int64_t value;
        



       
};

#endif // ISTRUCTION_HPP    
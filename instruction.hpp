
#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "memory.hpp"
#include "helpers.hpp"  


struct r_m {
    uint8_t r_m : 3;
    uint8_t mod : 2;
    uint8_t reg : 3;
    uint8_t byte_r_m;
};

struct SIB {
    uint8_t base : 3;
    uint8_t index : 3;
    uint8_t scale : 2;
    uint8_t byte_sib;
};



class CU;
class Memory;

class Instruction
{
    public:
        //destructor
        virtual  ~Instruction();
        //execute the instruction
        virtual void execute(CU* controlUnit, Memory* ram);//Polimorfic method that will be implemented in the derived classes

        virtual void fetchOperands(CU* controlUnit, Memory* ram);

        void decodeNbit(); //method to decode the number of bits of the value/operand with rex prefix or 0x66 prefix

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
        
        void setHasImmediate(bool hasImmediate);
        bool getHasImmediate();

        void setHasDisplacement(bool hasDisplacement);
        bool getHasDisplacement();

        void setHasModRM(bool hasModRM);
        bool getHasModRM();

        void setHasSIB(bool hasSIB);
        bool getHasSIB();

        void setRM(r_m rm);
        r_m getRM();

        void setSIB(SIB sib);
        SIB getSIB();

        void setValue(uint64_t value);
        uint64_t getValue();

        void setDisplacement(uint64_t displacement);
        uint64_t getDisplacement();

        void setRegToReg(bool regToReg);
        bool getRegToReg();

        void setRegToMem(bool regToMem);
        bool getRegToMem();

        void setMemToReg(bool memToReg);
        bool getMemToReg();


    private:
    //parts of the instruction
        uint32_t opcode;
        uint8_t prefix[4];
        int numPrefixes;
        bool rex;
        int8_t rexprefix;
        int nbit; //number of bits of the value/operand
        bool hasImmediate;
        bool hasDisplacement;
        bool hasModRM;
        bool hasSIB;
        r_m rm;
        SIB sib;
        uint64_t value;
        uint64_t displacement;
        bool regToReg;
        bool regToMem;
        bool memToReg;

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

        void fetchOperands(CU* controlUnit, Memory* ram) override;

        //setters and getters for the operands
        void setS_address(uint64_t S_address);
        uint64_t getS_address();

        void setD_address(uint64_t D_address);
        uint64_t getD_address();

        void setS_register(const std::string& S_register);
        std::string getS_register() const;

        void setD_register(const std::string& D_register);
        std::string getD_register() const;


    private:
        uint64_t S_address;
        uint64_t D_address;

        std::string S_register;
        std::string D_register;

    
        

       
};

#endif // INSTRUCTION_HPP    
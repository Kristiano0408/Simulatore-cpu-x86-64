
#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "memory.hpp"
#include "addressCalculator.hpp"
#include "instruction_code_map.hpp"
#include <unordered_set>
#include "operands.hpp"




class CU;
class Memory;

class Instruction
{
    public:

        //constructor
        Instruction();
        //destructor
        virtual  ~Instruction();
        //execute the instruction
        virtual void execute(CU* controlUnit, Memory* ram);//Polimorfic method that will be implemented in the derived classes

        virtual void fetchOperands(CU* controlUnit, Memory* ram) = 0; //Polimorfic method that will be implemented in the derived classes

        uint64_t castingValue(uint64_t value, int nbit); //cast the value to the number of bits of the operand (8, 16, 32, 64)

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

        void setSIBdisplacement(uint32_t SIBdisplacement);
        uint32_t getSIBdisplacement();

        void setRegToReg(bool regToReg);
        bool getRegToReg();

        void setRegToMem(bool regToMem);
        bool getRegToMem();

        void setMemToReg(bool memToReg);
        bool getMemToReg();

         //setters and getters for the operands
        void setSourceOperand(Operand* sourceOperand);
        Operand* getSourceOperand();
        void setDestinationOperand(Operand* destinationOperand);
        Operand* getDestinationOperand();

    protected:
        // operands for the instruction
        Operand* sourceOperand; //source operand (register or memory)
        Operand* destinationOperand; //destination operand (register or memory)

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
        uint32_t SIBdisplacement;
        bool regToReg;
        bool regToMem;
        bool memToReg;

};

//define the instruction classes (an iscrutction for each operation)

class MoveInstruction : public Instruction
{
    public:
        //destructor
        ~MoveInstruction() override = default;



        void fetchOperands(CU* controlUnit, Memory* ram) override;

        uint64_t calculatingAddressR_M(CU* controlUnit, Memory* ram); //calculate the address for the operation R/M   

       
        //execute the instruction
        void execute(CU* controlUnit, Memory* ram) override;
        int calculating_number_of_bits(CU* controlUnit); //calculate the number of bits of the value/operand
        

    

    
        

       
};

#endif // INSTRUCTION_HPP    
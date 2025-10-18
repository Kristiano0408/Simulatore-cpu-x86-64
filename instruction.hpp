
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
#include <memory>
#include "operands.hpp"



class Bus;

class Instruction
{
    public:

        //constructor
        Instruction();
        //destructor
        virtual  ~Instruction();

        //methods for pipeline stages
        virtual void execute(Bus& bus);//Polimorfic method that will be implemented in the derived classes

        virtual void fetchOperands(Bus& bus) = 0; //Polimorfic method that will be implemented in the derived classes

        virtual void accessMemory(Bus& bus) {} //Polimorfic method for memory access (only for load/store instructions)

        virtual void writeBack(Bus& bus) {} //Polimorfic method for write-back (only for store instructions)

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
        void setSourceOperand(std::unique_ptr<Operand> sourceOperand);
        Operand* getSourceOperand();
        void setDestinationOperand(std::unique_ptr<Operand> destinationOperand);
        Operand* getDestinationOperand();

        void setAddressingMode(AddressingMode addressingMode);
        AddressingMode getAddressingMode();

        int calculating_number_of_bits(); //calculate the number of bits of the value/operand

        uint64_t mask(int nbit);// return a mask for the number of bits (8, 16, 32, 64)

    protected:

        // operands for the instruction
        std::unique_ptr<Operand> sourceOperand; //SOURCE operand
        std::unique_ptr<Operand> destinationOperand; //destination operand

        AddressingMode addressingMode; //addressing mode of the instruction

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

        //intermediate value for pipeline stages
        uint64_t intermediate_result;
        bool tempCF;
        bool tempZF;
        bool tempSF;
        bool tempOF;
        bool tempPF;
        bool tempAF;



        

};

//define the instruction classes (an instruction for each operation)

//MOV instruction class
class MoveInstruction : public Instruction
{
    public:

        //destructor
        ~MoveInstruction() override = default;
        void fetchOperands(Bus& bus) override;
        //uint64_t calculatingAddressR_M(Bus& bus); //calculate the address for the operation R/M

        //execute the instruction
        void execute(Bus& bus) override;

        void accessMemory(Bus& bus) override;

        void writeBack(Bus& bus) override;



        
};

//ADD instruction class
class AddInstruction : public Instruction
{
    public:
        //destructor
        ~AddInstruction() override = default;

        void fetchOperands(Bus& bus) override;

        //execute the instruction
        void execute(Bus& bus) override;

        void accessMemory(Bus& bus) override;

        void writeBack(Bus& bus) override;


};

//SUB instruction class
class SubInstruction : public Instruction
{
    public:
        //destructor
        ~SubInstruction() override = default;

        void fetchOperands(Bus& bus) override;

        //execute the instruction
        void execute(Bus& bus) override;

        void accessMemory(Bus& bus) override;

        void writeBack(Bus& bus) override;

};


#endif // INSTRUCTION_HPP    
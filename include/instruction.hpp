
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

template<EnumType T>
class EventHandler;



class Instruction
{
    public:

        //constructor
        Instruction();
        //destructor
        virtual  ~Instruction();

        virtual bool isEmpty() const { return false; }

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

        uint64_t getInstructionId() const { return InstructionId; }

        void setInstructionId(uint64_t id) { InstructionId = id; }


        bool isWaitingSrcOperand() const { return waitingSrcOperand; }

        void setWaitingSrcOperand(bool waiting) { waitingSrcOperand = waiting; }

        bool isWaitingDestOperand() const { return waitingDestOperand; }

        void setWaitingDestOperand(bool waiting) { waitingDestOperand = waiting; }

        void setTemporaryValues(const temporaryValues& values) { tempValues = values; }

        temporaryValues getTemporaryValues()  { return tempValues; }

        temporaryValues& getTemporaryValuesRef() { return tempValues; } // Return a reference to the temporaryValues struct

        void setType(TypeofInstruction type) { this->type = type; }
        TypeofInstruction getType() const { return type; }

    protected:

        // operands for the instruction
        std::unique_ptr<Operand> sourceOperand; //SOURCE operand
        std::unique_ptr<Operand> destinationOperand; //destination operand

        AddressingMode addressingMode; //addressing mode of the instruction

        temporaryValues tempValues;

    private:
    //parts of the instruction
        TypeofInstruction type; //type of the instruction (arithmetic, logical, control flow, etc.)
        uint64_t InstructionId; //unique id for the instruction
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
        
        bool waitingSrcOperand = false;
        bool waitingDestOperand = false;

        




        

};

//define the instruction classes (an instruction for each operation)

//empty instruction class (for smartpointer initialization)
class EmptyInstruction : public Instruction
{
    public:
        //destructor
        ~EmptyInstruction() override = default;

        bool isEmpty() const override { return true; }


};



#endif // INSTRUCTION_HPP    
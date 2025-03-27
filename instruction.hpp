
#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "memory.hpp"
#include "addressingMode.hpp"
#include "instruction_code_map.hpp"
#include <unordered_set>
#include "operands.hpp"




class CU;
class Memory;

class Instruction
{
    public:
        //destructor
        virtual  ~Instruction();
        //execute the instruction
        virtual void execute(CU* controlUnit, Memory* ram);//Polimorfic method that will be implemented in the derived classes

        virtual void fetchOperands(CU* controlUnit, Memory* ram) = 0; //Polimorfic method that will be implemented in the derived classes

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

        void setSIBdisplacement(uint32_t SIBdisplacement);
        uint32_t getSIBdisplacement();

        void setRegToReg(bool regToReg);
        bool getRegToReg();

        void setRegToMem(bool regToMem);
        bool getRegToMem();

        void setMemToReg(bool memToReg);
        bool getMemToReg();

         //setters and getters for the operands
         void setS_address(uint64_t S_address);
         uint64_t getS_address();
 
         void setD_address(uint64_t D_address);
         uint64_t getD_address();
 
         void setS_register(const std::string& S_register);
         std::string getS_register() const;
 
         void setD_register(const std::string& D_register);
         std::string getD_register() const;

    protected:
        uint64_t S_address;
        uint64_t D_address;

        std::string S_register;
        std::string D_register;

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
        //constructor
        MoveInstruction();
        //destructor
        ~MoveInstruction() override = default;


        

        void fetchOperands(CU* controlUnit, Memory* ram) override;

        uint64_t calculatingAddressR_M(CU* controlUnit, Memory* ram); //calculate the address for the operation R/M   

       
        //execute the instruction
        void execute(CU* controlUnit, Memory* ram) override;
        int calcualting_number_of_bits(CU* controlUnit); //calculate the number of bits of the value/operand
        void executeR_M(CU* controlUnit, Memory* ram, MOVType type); //execute the instruction move register to R/M or move R/M to register (88, 89, 8A, 8B)
        void executeMI(CU* controlUnit, Memory* ram); //execute the instruction move immediate to memory/register
        void executeOI(CU* controlUnit, Memory* ram); //execute the instruction move immediate to reg
        void executeFD_TD(CU* controlUnit, Memory* ram, MOVType type); //execute the instruction move from offset to Rax or move from Rax to offset
        uint64_t castingValue(uint64_t value, int nbit); //cast the value to the number of bits of the operand (8, 16, 32, 64)

    

    
        

       
};

#endif // INSTRUCTION_HPP    
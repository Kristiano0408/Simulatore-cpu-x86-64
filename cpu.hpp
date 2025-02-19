//define the CPU class 
//basic version without SIMD instructions, only 4 flags and only 64 version of registers

#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>
#include "RegisterFile.hpp"
#include "ALU.hpp"
#include "Memory.hpp"
#include "Decoder.hpp"

class CPU
{
    public:
        
        //constructor that receives a pointer to the memory
        CPU(Memory* memory); 
        ~CPU();

        //cpu cycle (fetch, decode, execute)
        void fetchInstruction();
        void decodeInstruction();
        void executeInstruction();

        //getters for the registers and ALU
        RegisterFile& getRegisters();
        ALU& getALU();

    private:
        
        // registers, ALU, memory and decoder
        RegisterFile registers; 
        ALU alu;                 
        Memory* memory;          
        Decoder decoder;         
        
};










#endif   // CPU_HPP
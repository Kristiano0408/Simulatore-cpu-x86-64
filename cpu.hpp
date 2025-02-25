//define the CPU class 
//basic version without SIMD instructions, only 4 flags and only 64 version of registers

#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>
#include "Memory.hpp"
#include "ControlUnit.hpp"

class CPU
{

    private:
        
        // registers, ALU, memory and decoder          
        Memory* memory;          
        Decoder decoder;   
        CU controlUnit;    

    public:
        
        //constructor that receives a pointer to the memory
        CPU(Memory* memory); 
        ~CPU();

        //cpu operations
        void cpuReset();
        void cpuStep();
        void cpuStart();

        //getters for the registers and ALU
        
        ALU& getALU();

        RegisterFile& getRegisters();
        
        CU& getControlUnit();

    
        
};




#endif   // CPU_HPP
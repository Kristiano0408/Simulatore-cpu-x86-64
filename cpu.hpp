//define the CPU class 
//basic version without SIMD instructions, only 4 flags and only 64 version of registers

#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>
#include "bus.hpp"
#include "controlUnit.hpp"
#include "alu.hpp"
#include "registerFile.hpp"
#include "cacheManager.hpp"


class CPU
{

    private:
        
        Bus& bus; //pointer to the bus

        CU controlUnit; //control unit of the CPU
        ALU alu; //arithmetic logic unit
        RegisterFile registers; //register file of the CPU

        uint64_t L1_cache_size = 1024;
        uint64_t L2_cache_size = 8192;
        uint64_t L3_cache_size = 65536;
        uint64_t L1_cache_assoc = 2;
        uint64_t L2_cache_assoc = 4;
        uint64_t L3_cache_assoc = 8;

        CacheManager cacheManager;

        CPUState state = CPUState::FETCH; //current state of the CPU

        InstructionInfo  current_instruction; //current instruction fetched from memory
        Instruction* decodedInstruction = nullptr; //decoded instruction


    public:

        //constructor that receives a pointer to the bus
        CPU(Bus& bus);
        ~CPU();

        //cpu operations
        void cpuReset();
        void cpuStep();
        void cpuStart();

    
        //getters for the registers and ALU 
        ALU& getALU();

        RegisterFile& getRegisters();
        
        CU& getControlUnit();

        CacheManager& getCacheManager();

};




#endif   // CPU_HPP
//define the CPU class 
//basic version without SIMD instructions, only 4 flags and only 64 version of registers

#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include "controlUnit.hpp"
#include "alu.hpp"
#include "registerFile.hpp"
#include "cache/cacheManager.hpp"
#include "pipeline.hpp"
#include "pipelineController.hpp"
#include "third_party/unordered_dense.h"

class Bus;
class PipelineEventHandler;

class CPU: public TickedDevice, public FaultDevice
{

    private:
        
        Bus& bus; //pointer to the bus

        CU controlUnit; //control unit of the CPU
        ALU alu; //arithmetic logic unit
        RegisterFile registers; //register file of the CPU

        uint64_t L1_cache_size = 1024;
        uint64_t L2_cache_size = 8192;
        uint64_t L3_cache_size = 65536;
        uint8_t L1_cache_assoc = 2;
        uint8_t L2_cache_assoc = 4;
        uint8_t L3_cache_assoc = 8;


        //instructionId counter for unique identification of instructions
        uint64_t instructionIdCounter = 0;

        CacheManager cacheManager;

        Pipeline pipeline; //pipeline of the CPU

        PipelineController pipelineController; //controller for managing pipeline events

        

       




    


    public:

        //constructor that receives a pointer to the bus
        CPU(Bus& busRef);
        ~CPU();

        //cpu operations
        void cpuReset();
        void cpuStep();
        void cpuStart();

        void execute_operation() override; //override of the pure virtual function from Device class

        //getters for the registers, ALU, CU and CacheManager, Pipeline
        ALU& getALU();

        RegisterFile& getRegisters();
        
        CU& getControlUnit();

        CacheManager& getCacheManager();

        
        Pipeline& getPipeline();

        PipelineController& getPipelineController();

        void sendCacheRequest(CacheRequest&& request, CacheLevelType cacheType); //send a cache request

        void eraseCacheResponseIfFound(uint64_t requestID); //erase a cache response from the queue

        void findCacheResponse(uint64_t requestID, MaxCPUInstructionLength& response, bool& found); //find a cache response in the queue

        ankerl::unordered_dense::map<uint64_t, MaxCPUInstructionLength> cacheResponseQueue; //map for cache responses

        void incrementInstructionIdCounter() { instructionIdCounter++; }

        uint64_t getInstructionIdCounter() const { return instructionIdCounter; }



};




#endif   // CPU_HPP
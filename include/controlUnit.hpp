#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include "memoryInterface.hpp"
#include "device.hpp"

class Bus;   // forward declaration
class CPU;   // forward declaration
class RegisterFile;   // forward declaration
class Instruction;   // forward declaration
class PipelineEventHandler; // forward declaration

class CU : public FaultDevice
{
    public:
        CU(Bus& busRef);
        ~CU();
        //RegisterFile& getRegisters();


        void startFetch(uint64_t instructionId, uint64_t& index,  PipelineEventHandler& eventHandler);
        void updateFetch(uint64_t instructionId, PipelineEventHandler& eventHandler);
        InstructionInfo fetchInstruction(uint64_t instructionID, uint64_t index, PipelineEventHandler& eventHandler);
        void decodeInstruction(InstructionInfo instruction, std::unique_ptr<Instruction>& decodedInstruction, PipelineEventHandler& eventHandler);

        
        

    private:
        Decoder decoder;
        Bus& bus; //reference to the bus
        uint64_t currentInstructionId;
        InstructionMemoryInterface instructionMemoryInterface; // Interface for instruction memory access

        

        //helpers function for making the code more readable
        void searchingSibDisplacement(const MaxCPUInstructionLength& buffer, InstructionInfo& info, uint8_t& byteCounter, r_m& rm);
        void fetchOpcode(const MaxCPUInstructionLength& buffer, uint32_t& opcode, uint8_t& byteCounter);

        void fetchPrefix(const MaxCPUInstructionLength& buffer, uint8_t prefix[4], uint8_t& numbersOfPrefix, uint8_t& byteCounter);
        void fetchREX(uint8_t byte, bool& rex, uint8_t& rexprefix, uint8_t& byteCounter);
        

};









#endif // CONTROL_UNIT_HPP
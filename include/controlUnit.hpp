#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include <array>

class Bus;   // forward declaration
class CPU;   // forward declaration
class RegisterFile;   // forward declaration
template<EnumType T>
class EventHandler; // forward declaration

class CU
{
    public:
        CU(Bus& bus);
        ~CU();
        //RegisterFile& getRegisters();


        void startFetch(uint64_t instructionId, uint64_t& index, EventHandler<EventHandlerPipelineEventType>& eventHandler);

        void updateFetch(uint64_t instructionId, EventHandler<EventHandlerPipelineEventType>& eventHandler);
        InstructionInfo fetchInstruction(uint64_t instructionID, uint64_t index);
        Instruction* decodeInstruction(InstructionInfo instruction);
        void OperandFetch(Instruction* instruction);
        void executeInstruction(Instruction* instruction);
        void memoryphase(Instruction* instruction);
        void writeBack(Instruction* instruction);

        
        

    private:
        Decoder decoder;
        Bus& bus; //reference to the bus
        uint64_t currentInstructionId;

        

        //helpers function for making the code more readable
        void searchingSIB_Displacement(const MaxCPUInstructionLength& buffer, std::vector<uint8_t>& bytes, InstructionInfo& info, int& byteCounter, r_m& rm);
        void fetchOpcode(const MaxCPUInstructionLength& buffer, uint32_t& opcode, int& byteCounter, std::vector<uint8_t>& bytes);

        void fetchPrefix(const MaxCPUInstructionLength& buffer, uint8_t prefix[4], int& numbersOfPrefix, std::vector<uint8_t>& bytes, int& byteCounter);
        void fetchREX(uint8_t byte, bool& rex, uint8_t& rexprefix, int& byteCounter, std::vector<uint8_t>& bytes);
        void fetchRemainingBytes(const MaxCPUInstructionLength& buffer, std::vector<uint8_t>& bytes, int& byteCounter, int bytesToFetch);
        

};









#endif // CONTROL_UNIT_HPP
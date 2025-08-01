#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include "decoder.hpp"
#include <array>

class Bus;   // forward declaration
class CPU;   // forward declaration


class CU
{
    public:
        CU(Bus& bus, CPU& cpu);
        ~CU();



        InstructionInfo fetchInstruction();
        //Instruction* decodeInstruction(InstructionInfo instruction);
        //void OperandFetch(Instruction* instruction);
        //void executeInstruction(Instruction* instruction);

        
        

    private:
        Decoder decoder;
        Bus& bus; //reference to the bus
        CPU& cpu; //reference to the CPU
        

        //helpers function for making the code more readable
        void searchingSIB_Displacement(std::array<uint8_t, 15>& buffer, std::vector<uint8_t>& bytes, InstructionInfo& info, int& byteCounter, r_m& rm, uint64_t index);
        void fetchOpcode(std::array<uint8_t, 15>& buffer, uint32_t& opcode, int& byteCounter, std::vector<uint8_t>& bytes);

        void fetchPrefix(std::array<uint8_t, 15>& buffer, uint8_t prefix[4], int& numbersOfPrefix, std::vector<uint8_t>& bytes, int& byteCounter);
        void fetchREX(uint8_t byte, bool& rex, uint8_t& rexprefix, int& byteCounter, std::vector<uint8_t>& bytes);
        void fetchRemainingBytes(std::array<uint8_t, 15>& buffer, std::vector<uint8_t>& bytes, int& byteCounter, int bytesToFetch);
        

};









#endif // CONTROL_UNIT_HPP
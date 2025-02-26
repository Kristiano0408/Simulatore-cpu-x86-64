#include <iostream>
#include "memory.hpp"
#include "cpu.hpp"
#include "instruction.hpp"
#include "decoder.hpp"
#include "addressingMode.hpp"
#include "controlUnit.hpp"
#include <string>
#include <vector>

#include <iomanip>
#include <cstdint>

using namespace std;

int main()
{
    Memory ram(1024);
    CPU cpu(&ram);
    cpu.cpuReset();



    vector<uint8_t> data;
    //load the program in the memory
    data= {0x89, 0x78, 0x56, 0x34, 0x12, 0x11, 0x11,0x11,0x11};

    ram.setData(data);

    

    InstructionInfo info;

    Instruction* instruction;


    info = cpu.getControlUnit().fetchInstruction();


    for (int i = 0; i < info.istruction.size(); i++)
    {
        cout << "Byte: " << hex << static_cast<int>(info.istruction[i]) << endl;
    }
    

    instruction = cpu.getControlUnit().decodeInstruction(info);

    cout << "Instruction: " <<hex<< instruction->getValue() << endl;

    cpu.getControlUnit().executeInstruction(instruction);

    cpu.getControlUnit().getRegisters().printRegisters();

    





};
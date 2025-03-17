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
    data= {0x88, 0x04, 0x25, 0x78, 0x56, 0x34, 0x12, 0X34, 0X12, 0X11, 0X11, 0X11, 0X11, 0x8B, 0x04, 0x13,  0x89, 0x78, 0x56, 0x34, 0x12, 0x11, 0x11,0x11,0x11};

    ram.setData(data);

    

    InstructionInfo info;

    Instruction* instruction;


    info = cpu.getControlUnit().fetchInstruction();


    for (int i = 0; i < info.instruction.size(); i++)
    {
        cout << "Byte: " << hex << static_cast<int>(info.instruction[i]) << endl;
    }
    

    instruction = cpu.getControlUnit().decodeInstruction(info);

    cout << "Instruction: " <<hex << static_cast<int>(instruction->getOpcode()) << endl;
    cout << "prefix: " << hex << static_cast<int>(instruction->getPrefix()[0]) << endl;
    cout << "prefix: " << hex << static_cast<int>(instruction->getPrefix()[1]) << endl;
    cout << "prefix: " << hex << static_cast<int>(instruction->getPrefix()[2]) << endl;
    cout << "prefix: " << hex << static_cast<int>(instruction->getPrefix()[3]) << endl;
    cout << "numPrefixes: " << dec << instruction->getNumPrefixes() << endl;
    cout << "rex: " << instruction->getRex() << endl;
    cout << "rexprefix: " << hex << static_cast<int>(instruction->getRexprefix()) << endl;
    cout << "nbit: " << dec << instruction->getNbit() << endl;
    cout << "hasImmediate: " << instruction->getHasImmediate() << endl;
    cout << "hasDisplacement: " << instruction->getHasDisplacement() << endl;
    cout << "hasModRM: " << instruction->getHasModRM() << endl;
    cout << "hasSIB: " << instruction->getHasSIB() << endl;
    cout << "value: " << hex << instruction->getValue() << endl;
    cout << "displacement: " << hex << instruction->getDisplacement() << endl;
    cout << "SIB displacement: " << hex << instruction->getSIBdisplacement() << endl;
    cout << "regToReg: " << instruction->getRegToReg() << endl;
    cout << "regToMem: " << instruction->getRegToMem() << endl;
    cout << "memToReg: " << instruction->getMemToReg() << endl;

    cout << "RM: " << static_cast<int>(instruction->getRM().mod) << " " << static_cast<int>(instruction->getRM().reg)<< " " << static_cast<int>(instruction->getRM().r_m) << endl;
    cout << "SIB: " << static_cast<int>(instruction->getSIB().scale) << " " << static_cast<int>(instruction->getSIB().index) << " " << static_cast<int>(instruction->getSIB().base) << endl;

    cpu.getControlUnit().executeInstruction(instruction);

    cout << instruction->getS_register() << " " << instruction->getD_register() << endl;
    cout << instruction->getS_address() << " " << instruction->getD_address() << endl;



   

    





};
#include <iostream>
#include "memory.hpp"
#include "cpu.hpp"
#include "instruction.hpp"
#include "decoder.hpp"
#include "addressCalculator.hpp"
#include "controlUnit.hpp"
#include <string>
#include <vector>

#include <iomanip>
#include <cstdint>

using namespace std;

int main()
{
    CPU cpu;

    Memory ram(1024, cpu); //create the memory with 1024 bytes

    cpu.connectMemory(&ram); //connect the memory to the CPU
    
    //cpu.cpuReset();




    vector<uint8_t> data;
    //load the program in the memory
    data= {0x48, 0x8b, 0x08, 0x24, 0x10, 0x48, 0xBF, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x48, 0x8B, 0x07, 0x41, 0xB8, 0X34, 0X12, 0X11, 0X11, 0X11, 0X11, 0x8B, 0x04, 0x13,  0x89, 0x78, 0x56, 0x34, 0x12, 0x11, 0x11,0x11,0x11};

    ram.setData(data);

    

    InstructionInfo info;

    Instruction* instruction;

    for (int i = 0; i < 1 ;i++)
    {

    

    info = cpu.getControlUnit().fetchInstruction();

    cout << "Instruction size: " << info.instruction.size() << endl;


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


    cpu.getRegisters().getReg(Register::RAX) = 0x000000000000000A;
    cpu.getRegisters().getReg(Register::R8) = 0x00000000000000000;
    
    cpu.getControlUnit().OperandFetch(instruction);
    cpu.getControlUnit().executeInstruction(instruction);

    cout << hex << instruction->getSourceOperand()->getValue() << endl;
    cout << hex << instruction->getDestinationOperand()->getValue() << endl;


    //delete the operands
    delete instruction->getSourceOperand(); // delete the source operand after use
    delete instruction->getDestinationOperand(); // delete the destination operand after use






    //delete the instruction
    //delete instruction;

    std::cout << "----------------------------------------" << std::endl;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "RAX: " << hex << cpu.getRegisters().getReg(Register::RAX).raw() << std::endl;
    std::cout << "RBX: " << hex << cpu.getRegisters().getReg(Register::RBX).raw() << std::endl;
    std::cout << "RCX: " << hex << cpu.getRegisters().getReg(Register::RCX).raw() << std::endl;
    std::cout << "RDX: " << hex << cpu.getRegisters().getReg(Register::RDX).raw() << std::endl;
    std::cout << "RSI: " << hex << cpu.getRegisters().getReg(Register::RSI).raw() << std::endl;
    std::cout << "RDI: " << hex << cpu.getRegisters().getReg(Register::RDI).raw() << std::endl;
    std::cout << "RSP: " << hex << cpu.getRegisters().getReg(Register::RSP).raw() << std::endl;
    std::cout << "RBP: " << hex << cpu.getRegisters().getReg(Register::RBP).raw() << std::endl;
    std::cout << "R8: " << hex << cpu.getRegisters().getReg(Register::R8).raw() << std::endl;
    std::cout << "R9: " << hex << cpu.getRegisters().getReg(Register::R9).raw() << std::endl;
    std::cout << "R10: " << hex << cpu.getRegisters().getReg(Register::R10).raw() << std::endl;
    std::cout << "R11: " << hex << cpu.getRegisters().getReg(Register::R11).raw() << std::endl;
    std::cout << "R12: " << hex << cpu.getRegisters().getReg(Register::R12).raw() << std::endl;
    std::cout << "R13: " << hex << cpu.getRegisters().getReg(Register::R13).raw() << std::endl;
    std::cout << "R14: " << hex << cpu.getRegisters().getReg(Register::R14).raw() << std::endl;
    std::cout << "R15: " << hex << cpu.getRegisters().getReg(Register::R15).raw() << std::endl;
    std::cout << "RIP: " << hex << cpu.getRegisters().getReg(Register::RIP).raw() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

}
    

}



   

    






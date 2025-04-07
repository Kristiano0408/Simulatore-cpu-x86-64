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

    Memory ram(50000, cpu); //create the memory with 1024 bytes

    cpu.connectMemory(&ram); //connect the memory to the CPU
    
    //cpu.cpuReset();




    vector<uint8_t> data;
    //load the program in the memory
    //data= {0x48, 0x8b, 0x43, 0x12, 0x00, 0x00, 0x00,0x8a, 0x03, 0x24, 0x10, 0x48, 0xBF, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x48, 0x8B, 0x07, 0x41, 0xB8, 0X34, 0X12, 0X11, 0X11, 0X11, 0X11, 0x8B, 0x04, 0x13,  0x89, 0x78, 0x56, 0x34, 0x12, 0x11, 0x11,0x11,0x11};

    data = {
        // Inizializza alcuni registri con valori noti
         0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,    // MOV RAX, 1
         0xC7, 0xC3, 0x02, 0x00, 0x00, 0x00,    // MOV RBX, 2
         0xC7, 0xC1, 0x03, 0x00, 0x00, 0x00,    // MOV RCX, 3
         0xC7, 0xC2, 0x04, 0x00, 0x00, 0x00,    // MOV RDX, 4
         0x41, 0xC7, 0xC0, 0x05, 0x00, 0x00, 0x00,    // MOV R8, 5
         0x41, 0xC7, 0xC1, 0x06, 0x00, 0x00, 0x00,    // MOV R9, 6
    
        // Test MOV registro, registro
        0x48, 0x89, 0xD8,                            // MOV RAX, RBX
        0x48, 0x89, 0xCB,                            // MOV RBX, RCX
        0x48, 0x89, 0xD1,                            // MOV RCX, RDX
        0x4C, 0x89, 0xC2,                            // MOV RDX, R8
        0x4D, 0x89, 0xC8,                            // MOV R8, R9
    
        // Test MOV registro, immediato
        0xC7, 0xC0, 0x78, 0x56, 0x34, 0x12,    // MOV RAX, 0x12345678
        0x66, 0xBB, 0x34, 0x12,                      // MOV BX, 0x1234
        0xB1, 0x42,                                  // MOV CL, 0x42
    
        // Prepara un'area di memoria
        0xC7, 0xC6, 0x00, 0x10, 0x00, 0x00,    // MOV RSI, 0x1000 (indirizzo base per i test di memoria)
    
        // Test MOV [memoria], registro
        0x48, 0x89, 0x06,                            // MOV [RSI], RAX
        0x48, 0x89, 0x5E, 0x08,                      // MOV [RSI+8], RBX
        0x48, 0x89, 0x4E, 0x10,                      // MOV [RSI+16], RCX
    
        // Test MOV registro, [memoria]
        0x48, 0x8B, 0x1E,                            // MOV RBX, [RSI]
        0x48, 0x8B, 0x4E, 0x08,                      // MOV RCX, [RSI+8]
        0x48, 0x8B, 0x56, 0x10,                      // MOV RDX, [RSI+16]
    
        // Test MOV [memoria], immediato
        0xC7, 0x06, 0x90, 0x78, 0x56, 0x34,          // MOV DWORD PTR [RSI], 0x34567890
        0x66, 0xC7, 0x46, 0x04, 0xCD, 0xAB,          // MOV WORD PTR [RSI+4], 0xABCD
        0xC6, 0x46, 0x06, 0xEF,                      // MOV BYTE PTR [RSI+6], 0xEF
    
        // Test MOV con SIB
        0x48, 0x8B, 0x04, 0x0E,                      // MOV RAX, [RSI+RCX*1]
        0x48, 0x8B, 0x1C, 0x4E,                      // MOV RBX, [RSI+RCX*2]
        0x48, 0x8B, 0x14, 0x8E,                      // MOV RDX, [RSI+RCX*4]
        0x48, 0x8B, 0x3C, 0xCE                       // MOV RDI, [RSI+RCX*8]
    };

    ram.setData(data);

    

    InstructionInfo info;

    Instruction* instruction;

    for (int i = 0; i < 28; i++) {
        info = cpu.getControlUnit().fetchInstruction();
        instruction = cpu.getControlUnit().decodeInstruction(info);
        cpu.getControlUnit().OperandFetch(instruction);
        cpu.getControlUnit().executeInstruction(instruction);
        
        // Stampa lo stato dei registri dopo ogni istruzione
        std::cout << "Dopo l'istruzione " << i+1 << ":" << std::endl;
        std::cout << "RAX: " << std::hex << cpu.getRegisters().getReg(Register::RAX).raw() << std::endl;
        std::cout << "RBX: " << std::hex << cpu.getRegisters().getReg(Register::RBX).raw() << std::endl;
        std::cout << "RCX: " << std::hex << cpu.getRegisters().getReg(Register::RCX).raw() << std::endl;
        std::cout << "RDX: " << std::hex << cpu.getRegisters().getReg(Register::RDX).raw() << std::endl;
        std::cout << "RSI: " << std::hex << cpu.getRegisters().getReg(Register::RSI).raw() << std::endl;
        std::cout << "RDI: " << std::hex << cpu.getRegisters().getReg(Register::RDI).raw() << std::endl;
        std::cout << "RSP: " << std::hex << cpu.getRegisters().getReg(Register::RSP).raw() << std::endl;
        std::cout << "RBP: " << std::hex << cpu.getRegisters().getReg(Register::RBP).raw() << std::endl;
        std::cout << "R8: " << std::hex << cpu.getRegisters().getReg(Register::R8).raw() << std::endl;
        std::cout << "R9: " << std::hex << cpu.getRegisters().getReg(Register::R9).raw() << std::endl;
        std::cout << "R10: " << std::hex << cpu.getRegisters().getReg(Register::R10).raw() << std::endl;
        std::cout << "R11: " << std::hex << cpu.getRegisters().getReg(Register::R11).raw() << std::endl;
        std::cout << "R12: " << std::hex << cpu.getRegisters().getReg(Register::R12).raw() << std::endl;
        std::cout << "R13: " << std::hex << cpu.getRegisters().getReg(Register::R13).raw() << std::endl;
        std::cout << "R14: " << std::hex << cpu.getRegisters().getReg(Register::R14).raw() << std::endl;
        std::cout << "R15: " << std::hex << cpu.getRegisters().getReg(Register::R15).raw() << std::endl;
        std::cout << "RIP: " << std::hex << cpu.getRegisters().getReg(Register::RIP).raw() << std::endl;
        
        std::cout << "----------------------------------------" << std::endl;
    }




    /*for (int i = 0; i < 1 ;i++)
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
    cpu.getRegisters().getReg(Register::RBX) = 0x0000000000000000;
    
    cpu.getControlUnit().OperandFetch(instruction);
    cpu.getControlUnit().executeInstruction(instruction);

    cout << hex <<instruction->getSourceOperand()->getValue() << dec << endl;

    cout << hex << instruction->getDestinationOperand()->getValue() <<dec << endl;


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

}*/
    

}



   

    






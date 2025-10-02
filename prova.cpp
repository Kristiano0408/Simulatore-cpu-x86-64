#include <iostream>
#include "memory.hpp"
#include "cpu.hpp"
#include "instruction.hpp"
#include "decoder.hpp"
#include "addressCalculator.hpp"
#include "controlUnit.hpp"
#include "bus.hpp"
#include "registerFile.hpp"

#include <string>
#include <vector>

#include <iomanip>
#include <cstdint>

using namespace std;

int main()
{

    Bus bus; // Create a bus instance
    


    std::vector<uint8_t> data= {
        /*
        // Inizializza alcuni registri con valori noti
         0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,    // MOV RAX, 1
         0xC7, 0xC3, 0x02, 0x00, 0x00, 0x00,    // MOV RBX, 2
         0xC7, 0xC1, 0x03, 0x00, 0x00, 0x00,    // MOV RCX, 3
         0xC7, 0xC2, 0x04, 0x00, 0x00, 0x00,    // MOV RDX, 4
         0x41, 0xC7, 0xC0, 0x05, 0x00, 0x00, 0x00,    // MOV R8, 5
         0x41, 0xC7, 0xC1, 0x06, 0x00, 0x00, 0x00,    // MOV R9, 6
    
        // Test MOV registro, registro
        0x48, 0x89, 0xD8,                            // MOV RAX, RBX //rax=2
        0x48, 0x89, 0xCB,                            // MOV RBX, RCX // rbx=3
        0x48, 0x89, 0xD1,                            // MOV RCX, RDX //rcx=4
        0x4C, 0x89, 0xC2,                            // MOV RDX, R8  //rdx=5
        0x4D, 0x89, 0xC8,                            // MOV R8, R9   //r8=6
    
        // Test MOV registro, immediato
        0xC7, 0xC0, 0x78, 0x56, 0x34, 0x12,    // MOV RAX, 0x12345678 //rax=0x12345678
        0x66, 0xBB, 0x34, 0x12,                      // MOV BX, 0x1234 //rbx=0x1234
        0xB1, 0x42,                                  // MOV CL, 0x42 //rcx=0x42

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
       */
        /*
        0xC7, 0xC6, 0x00, 0x10, 0x00, 0x00,    // MOV RSI, 0x1000

        0x04, 0x05,                                  // ADD AL, imm8  (opcode 0x04) / ADD AX, imm16? esempio base
        0x05, 0x78, 0x56, 0x34, 0x12,                // ADD EAX, imm32 (opcode 0x05)
        0x80, 0x06, 0x01,                            // ADD BYTE PTR [RSI], imm8  ; aggiunge 1 al byte in [RSI]
        0x81, 0x06, 0x02,0x00,0x00,0x00,             // ADD DWORD PTR [RSI], imm32 ; aggiunge 2 a [RSI]
        0x83, 0x06, 0x03,                            // ADD DWORD PTR [RSI], imm8  ; aggiunge 3 a [RSI]
        0x00, 0xC3,                                  // ADD BL, AL  (MR)
        0x01, 0xD8,                                  // ADD EAX, EBX (MR)
        0x02, 0xC0,                                  // ADD AL, AL (RM)
        0x03, 0xC2,                                  // ADD EAX, EDX (RM)
        0x03, 0x04, 0x0E,                            // ADD EAX, [RSI+RCX*1]
        0x03, 0x14, 0x8E,                            // ADD EDX, [RSI+RCX*4]
        */
       // 0x2C : SUB AL, imm8
        0x2C, 0x03,         // sub al, 0x03

        // 0x2D : SUB EAX, imm32
        0x2D, 0x05,0x00,0x00,0x00, // sub eax, 5

        // 0x80 /5 : SUB r/m8, imm8 (r/m8 = BL)
        0x80, 0xEB, 0x01,   // sub bl, 0x01   ; ModR/M = EB => r/m8=BL, /5=sub

        // 0x81 /5 : SUB r/m32, imm32 (r/m32 = ECX)
        0x81, 0xE9, 0xFF,0xFF,0xFF,0xFF, // sub ecx, 0xFFFFFFFF

        // 0x83 /5 : SUB r/m32, imm8 (sign-extended, r/m32 = EDX)
        0x83, 0xEA, 0x02,   // sub edx, 0x02

        // 0x28 : SUB r/m8, r8 (AL - BL)
        0x28, 0xD8,         // sub al, bl  ; ModR/M = D8 => AL, BL

        // 0x29 : SUB r/m32, r32 (EAX - EBX)
        0x29, 0xD8,         // sub eax, ebx

        // 0x2A : SUB r8, r/m8 (BL - AL)
        0x2A, 0xD8,         // sub bl, al

        // 0x2B : SUB r32, r/m32 (ebx - eax)
        0x2B, 0xD8          // sub ebx, eax


    };

    bus.getMemory().setData(data); // Set the data in memory

    std::vector<uint8_t> memoryData = bus.getMemory().getData();

    for (size_t i = 0; i < memoryData.size(); i++) {
        cout << "Memory[" << i << "]: " << hex << static_cast<int>(memoryData[i]) << endl;
    }

    InstructionInfo info;

    Instruction* instruction;

    for (int i = 0; i < 9; i++) {


        auto& cu = bus.getCPU().getControlUnit();

        info = cu.fetchInstruction();
        std::cout << "Instruction size: " << info.instruction.size() << std::endl;
        for (size_t j = 0; j < info.instruction.size(); j++) {
            cout << "Byte: " << hex << static_cast<int>(info.instruction[j]) << endl;
        }
        instruction = bus.getCPU().getControlUnit().decodeInstruction(info);

        cout << "Instruction: " << hex << static_cast<int>(instruction->getOpcode()) << endl;
        cout << "Prefix: " << hex << static_cast<int>(instruction->getPrefix()[0]) << endl;
        cout << "Prefix: " << hex << static_cast<int>(instruction->getPrefix()[1]) << endl;
        cout << "Prefix: " << hex << static_cast<int>(instruction->getPrefix()[2]) << endl;
        cout << "Prefix: " << hex << static_cast<int>(instruction->getPrefix()[3]) << endl;
        cout << "Opcode: " << hex << static_cast<int>(instruction->getOpcode()) << endl;
        
        bus.getCPU().getControlUnit().OperandFetch(instruction);

        cout << "Fetched operands for Move Instruction" << endl;
        cout << "Source Operand: " << hex << instruction->getSourceOperand()->getValue().data << endl;
        cout << "Source Operand Size: " << instruction->getSourceOperand()->getSize() << endl;
        cout << "Destination Operand: " << hex << instruction->getDestinationOperand()->getValue().data << endl;
        cout << "Destination Operand Size: " << instruction->getDestinationOperand()->getSize() << endl;

        // Esegui l'istruzione
        
        bus.getCPU().getControlUnit().executeInstruction(instruction);

        // Stampa lo stato dei registri dopo ogni istruzione
        std::cout << "Dopo l'istruzione " << i+1 << ":" << std::endl;
        std::cout << "RAX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RAX).raw() << std::endl;
        std::cout << "RBX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RBX).raw() << std::endl;
        std::cout << "RCX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RCX).raw() << std::endl;
        std::cout << "RDX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RDX).raw() << std::endl;
        std::cout << "RSI: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RSI).raw() << std::endl;
        std::cout << "RDI: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RDI).raw() << std::endl;
        std::cout << "RSP: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RSP).raw() << std::endl;
        std::cout << "RBP: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RBP).raw() << std::endl;
        std::cout << "R8: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R8).raw() << std::endl;
        std::cout << "R9: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R9).raw() << std::endl;
        std::cout << "R10: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R10).raw() << std::endl;
        std::cout << "R11: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R11).raw() << std::endl;
        std::cout << "R12: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R12).raw() << std::endl;
        std::cout << "R13: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R13).raw() << std::endl;
        std::cout << "R14: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R14).raw() << std::endl;
        std::cout << "R15: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R15).raw() << std::endl;
        std::cout << "RIP: " << std::dec << bus.getCPU().getRegisters().getReg(Register::RIP).raw() << std::endl;

        //stampa dei flag
        std::cout << "CF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::CF) << std::endl;
        std::cout << "PF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::PF) << std::endl;
        std::cout << "AF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::AF) << std::endl;
        std::cout << "ZF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::ZF) << std::endl;
        std::cout << "SF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::SF) << std::endl;
        std::cout << "OF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::OF) << std::endl;
        

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



   

    






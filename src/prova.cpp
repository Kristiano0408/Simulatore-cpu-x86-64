#include "addressCalculator.hpp"
#include "bus.hpp"
#include "controlUnit.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include <iostream>

#include <string>
#include <vector>

#include <cstdint>

#include <termios.h>
#include <unistd.h>

static void setNonCanonical(bool enable)
{
    static termios oldt;
    static bool saved = false;
    termios newt;

    if (enable)
    {
        tcgetattr(STDIN_FILENO, &oldt);
        saved = true;
        newt = oldt;
        newt.c_lflag &= (tcflag_t)(~(ICANON | ECHO)); // disattiva buffering e echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    else if (saved)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}


using namespace std;

int main()
{

    setNonCanonical(true); // Abilita modalità non canonica per l'input

    DEBUG_LOG(debugLog("inizializzazione bus"));
    Bus bus; // Create a bus instance
    DEBUG_LOG(debugLog("inizializzazione cpu"));
#ifndef PERF

    std::vector<uint8_t> data = {

    // =========================================================================
    // MOV - immediate -> register
    // =========================================================================

    // MOV AL, 0x11
    0xB0, 0x11,

    // MOV CL, 0x22
    0xB1, 0x22,

    // MOV R8, 0x12345678
    0x41, 0xB8,
    0x78, 0x56, 0x34, 0x12,
    0x00, 0x00, 0x00, 0x00,

    // =========================================================================
    // MOV - register -> register
    // =========================================================================

    // MOV RAX, RBX
    0x48, 0x89, 0xD8,

    // MOV RBX, RAX
    0x48, 0x89, 0xC3,

    // =========================================================================
    // MOV - register -> memory
    // =========================================================================

    // MOV [RSI], RAX
    0x48, 0x89, 0x06,

    // =========================================================================
    // MOV - memory -> register
    // =========================================================================

    // MOV RBX, [RSI]
    0x48, 0x8B, 0x1E,

    // =========================================================================
    // ADD - immediate
    // =========================================================================

    // ADD AL, 5
    0x04, 0x05,

    // ADD EAX, 3
    0x05,
    0x03, 0x00, 0x00, 0x00,

    // ADD RAX, 0x10
    0x48, 0x05,
    0x10, 0x00, 0x00, 0x00,

    // =========================================================================
    // ADD - register -> register
    // =========================================================================

    // ADD RAX, RBX
    0x48, 0x01, 0xD8,

    // =========================================================================
    // SUB - immediate
    // =========================================================================

    // SUB AL, 3
    0x2C, 0x03,

    // SUB EAX, 1
    0x2D,
    0x01, 0x00, 0x00, 0x00,

    // SUB RAX, 2
    0x48, 0x2D,
    0x02, 0x00, 0x00, 0x00,

    // =========================================================================
    // SUB - register -> register
    // =========================================================================

    // SUB RAX, RBX
    0x48, 0x29, 0xD8,

    // =========================================================================
    // ADD - memory
    // =========================================================================

    // ADD BYTE [RSI], 1
    0x80, 0x04, 0x01,

    // =========================================================================
    // SUB - memory
    // =========================================================================

    // SUB DWORD [RSI], 2
    0x83, 0x2C, 0x02

    };

    bus.getMemory().setData(data); // Set the data in memory

    std::vector<uint8_t> memoryData = bus.getMemory().getData();
    for (size_t i = 0; i < memoryData.size(); i++)
    {
        cout << "Memory[" << i << "]: " << hex << static_cast<int>(memoryData[i]) << '\n';
    }

    int i = 0;
    while (true)
    {
        printf("Press space to execute a clock cycle, or 'q' to quit...\n");
        char c = (char)getchar();
        if (c == 'q')
        {
            break; // Esci dal ciclo se l'utente preme 'q'
        }
        if (c == ' ')
        {
            cout << "---- Clock Cycle " << i + 1 << " ----" << '\n';

            bus.tick();
            i++;

            // Stampa lo stato dei registri
            std::cout << "RAX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RAX).raw() << '\n';
            std::cout << "RBX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RBX).raw() << '\n';
            std::cout << "RCX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RCX).raw() << '\n';
            std::cout << "RDX: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RDX).raw() << '\n';
            std::cout << "RSI: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RSI).raw() << '\n';
            std::cout << "RDI: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RDI).raw() << '\n';
            std::cout << "RSP: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RSP).raw() << '\n';
            std::cout << "RBP: " << std::hex << bus.getCPU().getRegisters().getReg(Register::RBP).raw() << '\n';
            std::cout << "R8: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R8).raw() << '\n';
            std::cout << "R9: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R9).raw() << '\n';
            std::cout << "R10: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R10).raw() << '\n';
            std::cout << "R11: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R11).raw() << '\n';
            std::cout << "R12: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R12).raw() << '\n';
            std::cout << "R13: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R13).raw() << '\n';
            std::cout << "R14: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R14).raw() << '\n';
            std::cout << "R15: " << std::hex << bus.getCPU().getRegisters().getReg(Register::R15).raw() << '\n';
            std::cout << "RIP: " << std::dec << bus.getCPU().getRegisters().getReg(Register::RIP).raw() << '\n';

            // stampa dei flag
            std::cout << "CF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::CF) << '\n';
            std::cout << "PF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::PF) << '\n';
            std::cout << "AF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::AF) << '\n';
            std::cout << "ZF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::ZF) << '\n';
            std::cout << "SF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::SF) << '\n';
            std::cout << "OF: " << bus.getCPU().getRegisters().getFlags().getFlag(Flagbit::OF) << '\n';
        }
    }
#else
    constexpr int programSize = 1024 * 1024;

    std::vector<uint8_t> memoryData(programSize);

    for (size_t i = 0; i < programSize; i += 6)
    {
        memoryData[i] = 0x2C;
        memoryData[i + 1] = 0x03;

        memoryData[i + 2] = 0x28;
        memoryData[i + 3] = 0xD8;

        memoryData[i + 4] = 0x2A;
        memoryData[i + 5] = 0xD8;
    }

    bus.getMemory().setData(memoryData);
    for (uint64_t i = 0; i < programSize / 2; i++)
    {
        bus.tick();
    }
#endif

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
    cout << "SIB: " << static_cast<int>(instruction->getSIB().scale) << " " << static_cast<int>(instruction->getSIB().index) << " " << static_cast<int>(instruction->getSIB().base)
<< endl;


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

    return 0;
}

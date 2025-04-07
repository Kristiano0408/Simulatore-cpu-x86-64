#include "registerFile.hpp"
#include <string>
#include <iostream>

RegisterFile::RegisterFile()
{
    // Initialize all registers to 0
   for (auto& reg : GPregisters)
   {
         reg = std::make_unique<Reg>();

   }

   //inizialized the flags register
    flags = std::make_unique<FlagReg>();
}

RegisterFile::~RegisterFile()
{
    //nothing to do here
}







void RegisterFile::reset()
{
    // Reset all registers to 0
    RAX = 0;
    RBX = 0;
    RCX = 0;
    RDX = 0;
    RSI = 0;
    RDI = 0;
    RSP = 0;
    RBP = 0;
    R8 = 0;
    R9 = 0;
    R10 = 0;
    R11 = 0;
    R12 = 0;
    R13 = 0;
    R14 = 0;
    R15 = 0;
    RIP = 0;

    // Reset all flags to false
    for (int i = 0; i < 64; i++)
    {
        RFLAGS[i] = false;
    }
    

}


void RegisterFile::printRegisters()
{
    std::cout << "RAX: " << RAX << std::endl;
    std::cout << "RBX: " << RBX << std::endl;
    std::cout << "RCX: " << RCX << std::endl;
    std::cout << "RDX: " << RDX << std::endl;
    std::cout << "RSI: " << RSI << std::endl;
    std::cout << "RDI: " << RDI << std::endl;
    std::cout << "R8: " << R8 << std::endl;
    std::cout << "R9: " << R9 << std::endl;
    std::cout << "R10: " << R10 << std::endl;
    std::cout << "R11: " << R11 << std::endl;
    std::cout << "R12: " << R12 << std::endl;
    std::cout << "R13: " << R13 << std::endl;
    std::cout << "R14: " << R14 << std::endl;
    std::cout << "R15: " << R15 << std::endl;
    std::cout << "RSP: " << RSP << std::endl;
    std::cout << "RBP: " << RBP << std::endl;
    std::cout << "RIP: " << RIP << std::endl;
    

}



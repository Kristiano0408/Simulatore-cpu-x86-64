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


Reg& RegisterFile::getReg(Register reg)
{
    //returns the register at the index of the enum value
    return *GPregisters[static_cast<int>(reg)];
}


FlagReg& RegisterFile::getFlags()
{
    //returns the flags register
    return *flags;
}




void RegisterFile::reset()
{
    //resets all general purpose register to zero
    for (auto& reg : GPregisters)
    {
        reg->raw() = 0; //set the raw value of the register to 0
    }

    //resets the flags register to zero
    flags->raw() = 0;




}


void RegisterFile::printRegisters()
{
    std::cout << "RAX: " << (*GPregisters[static_cast<int>(Register::RAX)]).raw() << std::endl;
    std::cout << "RBX: " << (*GPregisters[static_cast<int>(Register::RBX)]).raw() << std::endl;
    std::cout << "RCX: " << (*GPregisters[static_cast<int>(Register::RCX)]).raw() << std::endl;
    std::cout << "RDX: " << (*GPregisters[static_cast<int>(Register::RDX)]).raw() << std::endl;
    std::cout << "RSI: " << (*GPregisters[static_cast<int>(Register::RSI)]).raw() << std::endl;
    std::cout << "RDI: " << (*GPregisters[static_cast<int>(Register::RDI)]).raw() << std::endl;
    std::cout << "R8: " << (*GPregisters[static_cast<int>(Register::R8)]).raw() << std::endl;
    std::cout << "R9: " << (*GPregisters[static_cast<int>(Register::R9)]).raw() << std::endl;
    std::cout << "R10: " << (*GPregisters[static_cast<int>(Register::R10)]).raw() << std::endl;
    std::cout << "R11: " << (*GPregisters[static_cast<int>(Register::R11)]).raw() << std::endl;
    std::cout << "R12: " << (*GPregisters[static_cast<int>(Register::R12)]).raw() << std::endl;
    std::cout << "R13: " << (*GPregisters[static_cast<int>(Register::R13)]).raw() << std::endl;
    std::cout << "R14: " << (*GPregisters[static_cast<int>(Register::R14)]).raw() << std::endl;
    std::cout << "R15: " << (*GPregisters[static_cast<int>(Register::R15)]).raw() << std::endl;
    std::cout << "RSP: " << (*GPregisters[static_cast<int>(Register::RSP)]).raw() << std::endl;
    std::cout << "RBP: " << (*GPregisters[static_cast<int>(Register::RBP)]).raw() << std::endl;
    std::cout << "RIP: " << (*GPregisters[static_cast<int>(Register::RIP)]).raw() << std::endl;
    

}



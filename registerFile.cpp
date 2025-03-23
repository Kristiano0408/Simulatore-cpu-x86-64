#include "registerFile.hpp"
#include <string>
#include <iostream>

RegisterFile::RegisterFile()
{
    //initialize the registers
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
    
    for (int i = 0; i < 64; i++)
    {
        RFLAGS[i] = false;
    }
}

RegisterFile::~RegisterFile()
{
    //nothing to do here
}


void RegisterFile::setRegisterValue(const std::string& name, uint64_t value)
{
    if(name == "RAX")
    {
        RAX = value;
    }
    else if(name == "RBX")
    {
        RBX = value;
    }
    else if(name == "RCX")
    {
        RCX = value;
    }
    else if(name == "RDX")
    {
        RDX = value;
    }
    else if(name == "RSI")
    {
        RSI = value;
    }
    else if(name == "RDI")
    {
        RDI = value;
    }
    else if(name == "R8")
    {
        R8 = value;
    }
    else if(name == "R9")
    {
        R9 = value;
    }
    else if(name == "R10")
    {
        R10 = value;
    }
    else if(name == "R11")
    {
        R11 = value;
    }
    else if(name == "R12")
    {
        R12 = value;
    }
    else if(name == "R13")
    {
        R13 = value;
    }
    else if(name == "R14")
    {
        R14 = value;
    }
    else if(name == "R15")
    {
        R15 = value;
    }
  
        

   
    
}

uint64_t RegisterFile::getRegisterValue(const std::string& name)
{
    if(name == "RAX")
    {
        return RAX;
    }
    else if(name == "RBX")
    {
        return RBX;
    }
    else if(name == "RCX")
    {
        return RCX;
    }
    else if(name == "RDX")
    {
        return RDX;
    }
    else if(name == "RSI")
    {
        return RSI;
    }
    else if(name == "RDI")
    {
        return RDI;
    }
    else if(name == "R8")
    {
        return R8;
    }
    else if(name == "R9")
    {
        return R9;
    }
    else if(name == "R10")
    {
        return R10;
    }
    else if(name == "R11")
    {
        return R11;
    }
    else if(name == "R12")
    {
        return R12;
    }
    else if(name == "R13")
    {
        return R13;
    }
    else if(name == "R14")
    {
        return R14;
    }
    else if(name == "R15")
    {
        return R15;
    }

    return 0;
  
        
    
}

void RegisterFile::setRSP(uint64_t value)
{
    RSP = value;
}

uint64_t RegisterFile::getRSP()
{
    return RSP;
}

void RegisterFile::setRIP(uint64_t value)
{
    RIP = value;
}

uint64_t RegisterFile::getRIP()
{
    return RIP;
}

void RegisterFile::setRBP(uint64_t value)
{
    RBP = value;
}

uint64_t RegisterFile::getRBP()
{
    return RBP;
}


void RegisterFile::setZF(bool value)
{
    
}

bool RegisterFile::getZF()
{
    
}

void RegisterFile::setSF(bool value)
{
    
}

bool RegisterFile::getSF()
{
    
}

void RegisterFile::setOF(bool value)
{
    
}

bool RegisterFile::getOF()
{
    
}


void RegisterFile::setCF(bool value)
{
    
}

bool RegisterFile::getCF()
{
    
}

void RegisterFile::reset()
{
    
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



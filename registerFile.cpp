#include "registerFile.hpp"
#include <string>

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


void RegisterFile::setRegisterValue(const std::string& name, int64_t value)
{
   
    
}

int64_t RegisterFile::getRegisterValue(const std::string& name)
{
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




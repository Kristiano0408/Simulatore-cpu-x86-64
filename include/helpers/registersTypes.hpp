#ifndef REGISTER_TYPES_HPP
#define REGISTER_TYPES_HPP


enum class Register : uint8_t
{
    RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15,
    RIP, DummyRegister
};


enum class Flagbit : uint8_t {

    CF = 0, //carry flag
    PF = 2, //parity flag
    AF = 4, //auxiliary carry flag
    ZF = 6, //zero flag
    SF = 7, //sign flag
    OF = 11 //overflow flag
};








#endif // REGISTER_TYPES_HPP
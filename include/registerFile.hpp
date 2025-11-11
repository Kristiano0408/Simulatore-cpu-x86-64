//for managing the cpu registers

// registerFile.hpp
#ifndef REGISTERFILE_HPP
#define REGISTERFILE_HPP
#include <cstdint>
#include <string>
#include <memory>
#include <array>

enum class Register {
    RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15,
    RIP, DummyRegister
};


enum Flagbit{

    CF = 0, //carry flag
    PF = 2, //parity flag
    AF = 4, //auxiliary carry flag
    ZF = 6, //zero flag
    SF = 7, //sign flag
    OF = 11 //overflow flag
};


class Reg{

    public:
        Reg() : value(0) {};
        Reg(uint64_t val) : value(val) {};

        operator uint64_t() const { return value; } //conversion operator to uint64_t

        Reg& operator=(uint64_t val) { value = val; return *this; } //assignment operator (you can assign a uint64_t to a Reg object)

        uint64_t& raw() { return value; } //get the raw value of the register

        const uint64_t& raw() const { return value; } //get the raw value of the register (const version)


    private:
        uint64_t value;
        



};


class FlagReg : public Reg {
    public:
        FlagReg() : Reg() {};

        bool getFlag(Flagbit flag) const {
            return (raw() >> static_cast<int>(flag)) & 1; //get the flag bit
        }

        void setFlag(Flagbit flag, bool value) {
            if (value) {
                raw() |= (1 << static_cast<int>(flag)); //set the flag bit
            } else {
                raw() &= ~(1 << static_cast<int>(flag)); //clear the flag bit
            }
        }

        

};



class RegisterFile {
    // Implementation of the RegisterFile class

    public:
        RegisterFile();
        ~RegisterFile();

        //access to general purpose registers
        Reg& getReg(Register reg);

        //access to rflags register
        FlagReg& getFlags();

        //reset the registers
        void reset();

        void printRegisters();


    private:

        std::array<std::unique_ptr<Reg>, 17> GPregisters; //array of unique_ptrs to Reg objects
        std::unique_ptr<FlagReg> flags; //unique_ptr to FlagReg object

        //in the future we can add more registers like simmd, xmm, etc.

        

};
#endif // REGISTERFILE_HPP

//for managing the cpu registers

// registerFile.hpp
#ifndef REGISTERFILE_HPP
#define REGISTERFILE_HPP
#include <cstdint>
#include <array>
#include "helpers/registersTypes.hpp"


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
        FlagReg() : Reg(0) {} //initialize the register to 0

        bool getFlag(Flagbit flag) const { return (bool)(raw() >> static_cast<int>(flag) & 1); } //get the flag bit

        void setFlag(Flagbit flag, bool v) {
            if (v) {
                raw() |= (1ULL << static_cast<int>(flag)); //set the flag bit
            } else {
                raw() &= ~(1ULL << static_cast<int>(flag)); //clear the flag bit
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

        std::array<Reg,17> GPregisters; //array of unique_ptrs to Reg objects
        FlagReg flags; //unique_ptr to FlagReg object

        //in the future we can add more registers like simmd, xmm, etc.

        

};
#endif // REGISTERFILE_HPP

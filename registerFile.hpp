//for managing the cpu registers

// registerFile.hpp
#ifndef REGISTERFILE_HPP
#define REGISTERFILE_HPP
#include <cstdint>
#include <string>

class RegisterFile {
    // Implementation of the RegisterFile class

    public:
        RegisterFile();
        ~RegisterFile();

        //getters and setters for the registers
        void setRegisterValue(const std::string& name, uint64_t value);
        uint64_t getRegisterValue(const std::string& name);
        uint64_t& getRegister(const std::string& name);

        //getters and setters for the special registers
        void setRSP(uint64_t value);
        uint64_t getRSP();

        void setRIP(uint64_t value);
        uint64_t getRIP();

        void setRBP(uint64_t value);
        uint64_t getRBP();

        uint64_t* getRSPPointer();
        
        uint64_t* getRBPPointer();


        //getters and setters for the flags
        void setZF(bool value);
        bool getZF();

        void setSF(bool value);
        bool getSF();

        void setOF(bool value);
        bool getOF();

        void setCF(bool value);
        bool getCF();

        //reset the registers
        void reset();

        void printRegisters();


    private:

        //registers(olnly 64 bit version)
        uint64_t RAX; 
        uint64_t RBX;
        uint64_t RCX;
        uint64_t RDX;
        uint64_t RSI;
        uint64_t RDI;
        uint64_t RSP;
        uint64_t RBP;
        uint64_t R8;
        uint64_t R9;
        uint64_t R10;
        uint64_t R11;
        uint64_t R12;
        uint64_t R13;
        uint64_t R14;
        uint64_t R15;
        uint64_t RIP;
        uint64_t dummyRegister = 0; //dummy register for preventing errors

        //flags
        bool RFLAGS[64];

};
#endif // REGISTERFILE_HPP

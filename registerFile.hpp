//for managing the cpu registers

// registerFile.hpp
#ifndef REGISTERFILE_HPP
#define REGISTERFILE_HPP
#include <cstdint>

class RegisterFile {
    // Implementation of the RegisterFile class

    public:
        RegisterFile();
        ~RegisterFile();

        //getters and setters for the registers
        void setRegisterValue(const std::string& name, int64_t value);
        int64_t getRegisterValue(const std::string& name);

        //getters and setters for the special registers
        void setRSP(uint64_t value);
        uint64_t getRSP();

        void setRIP(uint64_t value);
        uint64_t getRIP();

        void setRBP(uint64_t value);
        uint64_t getRBP();

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


    private:

        //registers(olnly 64 bit version)
        int64_t RAX; 
        int64_t RBX;
        int64_t RCX;
        int64_t RDX;
        int64_t RSI;
        int64_t RDI;
        uint64_t RSP;
        uint64_t RBP;
        int64_t R8;
        int64_t R9;
        int64_t R10;
        int64_t R11;
        int64_t R12;
        int64_t R13;
        int64_t R14;
        int64_t R15;
        uint64_t RIP;

        //flags
        bool RFLAGS[64];

};
#endif // REGISTERFILE_HPP

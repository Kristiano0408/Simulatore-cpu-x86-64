//for managing the ALU and arithmetic operations

// alu.hpp
#ifndef ALU_HPP
#define ALU_HPP
#include <cstdint>

class ALU {
    // Implemenattion of the ALU class

    public:
        ALU();
        ~ALU();

        //arithmetic operations
        int64_t add(int64_t a, int64_t b);
        int64_t sub(int64_t a, int64_t b);
        int64_t mul(int64_t a, int64_t b);
        //etc 

        
};

#endif // ALU_HPP

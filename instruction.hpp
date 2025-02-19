//

#ifndef ISTRUCTION_HPP
#define ISTRUCTION_HPP

#include <cstdint>

class CPU;

class Instruction
{
    public:

        //destructor
        virtual  ~Instruction() = default;
        //execute the instruction
        virtual void execute(CPU* cpu) = 0; //Polimorfic method that will be implemented in the derived classes
};



#endif // ISTRUCTION_HPP    
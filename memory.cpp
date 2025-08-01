#include "memory.hpp"
#include "cpu.hpp"
#include <cmath>
#include "bus.hpp"
//ricordarsi di convertire indirizzi dec per accedere al vettore e in binario per utilizzare l'indirizzo

//ricordasri controllo offset e size per evitare buffer overflow

//Constructor
Memory::Memory(size_t size, Bus& bus) : size(size), RSP(bus.getCPU().getRegisters().getReg(Register::RSP).raw()),
                                         RBP(bus.getCPU().getRegisters().getReg(Register::RBP).raw()) {


    data.resize(size, 0); //initialize the memory with 0

    size_stack = size / 4; //initialize the stack size to 1/4 of the memory size

    RSP = size - 1; //initialize the stack pointer to the end of the memory
    RBP = size - 1; //initialize the base pointer to the end of the memory


};


//get the size of the memory
size_t Memory::getSize() const
{
    return size;
};

//get the data of the memory
const std::vector<uint8_t>& Memory::getData() const
{
    return data;
};

//set the data of the memory
void Memory::setData(const std::vector<uint8_t>& data)
{
    this->data = data;
};

//clear the memory
void Memory::clear()
{
    data.clear();
    data.resize(size, 0);
};

void Memory::setStackPointer(uint64_t value)
{
    RSP = value;
};

uint64_t Memory::getStackPointer() const
{
    return RSP;
};

void Memory::setBasePointer(uint64_t value)
{
    RBP = value;
};

uint64_t Memory::getBasePointer() const
{
    return RBP;
};

void Memory::push(uint64_t value)
{
    //controllo overflow
    if (RSP - 8 < size - size_stack) //check if the stack pointer is out of bounds
    {
        std::cerr << "Stack overflow!" << std::endl; //print error message
        return; //return from the function
    }


    RSP -= 8; //decrement the stack pointer
    writeGeneric<uint64_t>(RSP, value); //write the value to the stack
};

uint64_t Memory::pop()
{
    //controllo underflow
    if (RSP > size - 8) //check if the stack pointer is out of bounds
    {
        std::cerr << "Stack underflow!" << std::endl; //print error message
        return 0; //return 0 from the function
    }


    uint64_t value = readGeneric<uint64_t>(RSP); //read the value from the stack
    RSP += 8; //increment the stack pointer
    return value; //return the value
};
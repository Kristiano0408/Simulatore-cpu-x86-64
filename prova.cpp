#include <iostream>
#include "memory.hpp"

using namespace std;

int main()
{
    Memory ram(1024); //create a memory with 1024 bytes

    //write some values in the memory
    ram.writeByte(0, 0x12);
    uint8_t value = ram.readByte(0);
    cout << "Value at address 0: " << hex << static_cast<int>(value) << endl;
    


};
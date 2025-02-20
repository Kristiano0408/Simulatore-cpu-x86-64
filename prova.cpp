#include <iostream>
#include "memory.hpp"
#include <cstdint>

using namespace std;

int main()
{
    Memory ram(1024); //create a memory with 1024 bytes

    //write some values in the memory
    ram.writeWord(0, 0x1234);
    uint16_t value = ram.readWord(0);
    cout << "Value at address 0: " << hex << static_cast<int>(value) << endl;

    ram.writeDWord(2, 0x12345678);
    uint32_t value2 = ram.readDWord(2);
    cout << "Value at address 2: " << hex << static_cast<int>(value2) << endl;



};
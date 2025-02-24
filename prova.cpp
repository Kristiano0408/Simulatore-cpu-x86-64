#include <iostream>
#include "memory.hpp"
#include "cpu.hpp"
#include "instruction.hpp"
#include "decoder.hpp"
#include "addressingMode.hpp"
#include "controlUnit.hpp"
#include <string>
#include <vector>

#include <iomanip>
#include <cstdint>

using namespace std;

int main()
{
    Memory ram(1024);
    CPU cpu(&ram);
    cpu.cpuReset();

    vector<uint8_t> data;
    //load the program in the memory
    data= { 0x48, 0xB8, 0x78, 0x56, 0x34, 0x12};

    ram.setData(data);


    cpu.getControlUnit().fetchInstruction();

    





};
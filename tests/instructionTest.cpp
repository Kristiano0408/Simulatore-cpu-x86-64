#include "../include/instruction.hpp"
#include <array>
int main() {
    // Create an instance of the Instruction class

   InstructionInfo info{};
   std::cout<< sizeof(info)<<std::endl;
   std::cout<< alignof(info)<<std::endl;


}

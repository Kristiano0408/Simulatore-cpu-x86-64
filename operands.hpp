// opcode.hpp
#ifndef OPERANDS_HPP
#define OPERANDS_HPP


#include <cstdint>
#include "helpers.hpp"


class CU;  // Forward declaration of CU
class Memory;  // Forward declaration of Memory
class Instruction;  // Forward declaration OF Instruction

namespace operandFetch {
    void fetchRM(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchMR(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchFD(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchTD(Instruction* i, CU* controlUnit, Memory* ram);
    void fetchOI(Instruction* i, CU* controlUnit, Memory* ram, uint32_t opcode);
    void fetchMI(Instruction* i, CU* controlUnit, Memory* ram);
    uint64_t calculatingAddressR_M(Instruction* i, CU* controlUnit, Memory* ram);
}

#endif

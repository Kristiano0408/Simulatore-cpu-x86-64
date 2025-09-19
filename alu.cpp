#include "alu.hpp"
#include "cpu.hpp"
#include "registerFile.hpp"

ALU::ALU(Bus& bus) : bus(bus)
{
    //nothing to do here
}

ALU::~ALU()
{
    //nothing to do here
}

uint64_t ALU::add(uint64_t dest, uint64_t src)
{
    uint64_t res = dest + src;
     //UPDATING FLAGS
    FlagReg& flags = bus.getCPU().getRegisters().getFlags();

    //Zero Flag
    flags.setFlag(Flagbit::ZF, (res == 0));

    //Carry Flag
    flags.setFlag(Flagbit::CF, (res < dest));

    //parity flag(set if the number of set bits in the least significant byte is even)
    flags.setFlag(Flagbit::PF, !__builtin_parity(res & 0xFF));

    //Auxiliary Carry Flag(set if there is a carry from bit 3 to bit 4)
    flags.setFlag(Flagbit::AF, ((dest & 0xF) + (src & 0xF)) > 0xF);

    //Sign Flag(set if the result is negative)
    flags.setFlag(Flagbit::SF, (static_cast<int64_t>(res) < 0));

    //Overflow Flag(set if there is a signed overflow)
    flags.setFlag(Flagbit::OF, (((static_cast<int64_t>(dest) < 0) == (static_cast<int64_t>(src) < 0)) && ((static_cast<int64_t>(res) < 0) != (static_cast<int64_t>(dest) < 0))));

    return res;
}

uint64_t ALU::sub(uint64_t dest, uint64_t src)
{
    uint64_t res = dest - src;
     //UPDATING FLAGS
    FlagReg& flags = bus.getCPU().getRegisters().getFlags();

    //Zero Flag
    flags.setFlag(Flagbit::ZF, (res == 0));

    //Carry Flag
    flags.setFlag(Flagbit::CF, (dest < src));

    //parity flag(set if the number of set bits in the least significant byte is even)
    flags.setFlag(Flagbit::PF, !__builtin_parity(res & 0xFF));

    //Auxiliary Carry Flag(set if there is a borrow from bit 4 to bit 3)
    flags.setFlag(Flagbit::AF, ((dest & 0xF) + (src & 0xF)) > 0xF);

    //Sign Flag(set if the result is negative)
    flags.setFlag(Flagbit::SF, (static_cast<int64_t>(res) < 0));

    //Overflow Flag(set if there is a signed overflow)
    flags.setFlag(Flagbit::OF, (((static_cast<int64_t>(dest) < 0) != (static_cast<int64_t>(src) < 0)) && ((static_cast<int64_t>(res) < 0) != (static_cast<int64_t>(dest) < 0))));

    return res;
}
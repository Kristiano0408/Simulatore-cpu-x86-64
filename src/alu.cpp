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


void ALU::executeOperation(temporaryValues& tempValues, typeofInstruction type)
{
    auto getIntegralValue = [](const auto& v) -> uint64_t {
        return std::visit([](auto&& value) -> uint64_t {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_integral_v<T>)
                return static_cast<uint64_t>(value);
            else
                throw std::runtime_error("ALU: operando non integrale");
        }, v);
    };

    uint64_t dest = getIntegralValue(tempValues.destValue);
    uint64_t src  = getIntegralValue(tempValues.srcValue);
    uint64_t tmp  = 0;

    // Esegui le operazioni normalmente
    switch (type) {
        case typeofInstruction::ADD:
            tmp = add(dest, src);
            tempValues.CF = (tmp < dest);
            tempValues.AF = ((dest & 0xF) + (src & 0xF)) > 0xF;
            tempValues.OF = ((((int64_t)dest < 0)== ((int64_t)src < 0)) &&
                             (((int64_t)tmp < 0) != ((int64_t)dest < 0)));
            break;

        case typeofInstruction::SUB:
            tmp = sub(dest, src);
            tempValues.CF = (dest < src);
            tempValues.AF = ((dest & 0xF) - (src & 0xF)) & 0x10;
            tempValues.OF = ((((int64_t)dest < 0) != ((int64_t)src < 0)) &&
                             (((int64_t)tmp < 0) != ((int64_t)dest < 0)));
            break;

        default:
            throw std::runtime_error("ALU: operazione non supportata");
    }

    tempValues.ZF = (tmp == 0);
    tempValues.SF = ((int64_t)tmp < 0);
    tempValues.PF = !__builtin_parity(tmp & 0xFF);
    tempValues.resultValue = tmp;
}


uint64_t ALU::sub(uint64_t dest, uint64_t src)
{
    return dest - src;
}


uint64_t ALU::add(uint64_t dest, uint64_t src)
{
    return dest + src;
}
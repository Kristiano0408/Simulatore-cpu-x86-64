#ifndef MASK_AND_CONST_HPP
#define MASK_AND_CONST_HPP

#include <cstdint>

//defined constants
inline constexpr size_t CACHE_LINE_SIZE = 64; // Size of a cache line in bytes


namespace RexMask
{
    inline constexpr uint8_t REX_W = 0x08; // REX.W bit
    inline constexpr uint8_t REX_R = 0x04; // REX.R bit
    inline constexpr uint8_t REX_X = 0x02; // REX.X bit
    inline constexpr uint8_t REX_B = 0x01; // REX.B bit

}// namepsace RexMask

namespace ModRMMask
{
    inline constexpr uint8_t MOD_MASK = 0b11000000; // Mask for the MOD field (bits 6-7)
    inline constexpr uint8_t REG_MASK = 0b00111000; // Mask for the REG field (bits 3-5)
    inline constexpr uint8_t RM_MASK  = 0b00000111; // Mask for the R/M field (bits 0-2)

    inline constexpr uint8_t MOD_SHIFT = 6;
    inline constexpr uint8_t REG_SHIFT = 3;
    inline constexpr uint8_t RM_SHIFT  = 0;

    [[nodiscard]] inline constexpr uint8_t extractMod(uint8_t byte) 
    {
        return (byte & MOD_MASK) >> MOD_SHIFT;
    }

    [[nodiscard]] inline constexpr uint8_t extractReg(uint8_t byte) 
    {
        return (byte & REG_MASK) >> REG_SHIFT;
    }

    [[nodiscard]] inline constexpr uint8_t extractRM(uint8_t byte) 
    {
        return (byte & RM_MASK) >> RM_SHIFT;
    }

} // namespace ModRMMask


namespace SIBMask
{
    inline constexpr uint8_t SCALE_MASK = 0b11000000; // Mask for the SCALE field (bits 6-7)
    inline constexpr uint8_t INDEX_MASK = 0b00111000; // Mask for the INDEX field (bits 3-5)
    inline constexpr uint8_t BASE_MASK  = 0b00000111; // Mask for the BASE field (bits 0-2)


    [[nodiscard]] inline constexpr uint8_t extractScale(uint8_t byte) 
    {
        return (byte & SCALE_MASK) >> 6;
    }

    [[nodiscard]] inline constexpr uint8_t extractIndex(uint8_t byte) 
    {
        return (byte & INDEX_MASK) >> 3;
    }

    [[nodiscard]] inline constexpr uint8_t extractBase(uint8_t byte) 
    {
        return (byte & BASE_MASK);
    }

} // namespace SIBMask


namespace LegacyPrefixMask
{
    inline constexpr uint8_t OPERAND_SIZE_OVERRIDE = 0x66; // Operand-size override prefix
    inline constexpr uint8_t ADDRESS_SIZE_OVERRIDE = 0x67; // Address-size override prefix
    inline constexpr uint8_t REPNE_PREFIX          = 0xF2; // REPNE/REPNZ prefix
    inline constexpr uint8_t REP_PREFIX            = 0xF3; // REP/REPE/REPZ prefix
    inline constexpr uint8_t LOCK_PREFIX           = 0xF0; // LOCK prefix

} // namespace LegacyPrefixMask


#endif // MASK_AND_CONST_HPP
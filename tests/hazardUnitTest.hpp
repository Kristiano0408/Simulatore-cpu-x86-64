#ifndef HAZARD_UNIT_TEST_HPP
#define HAZARD_UNIT_TEST_HPP

#include "hazardUnit.hpp"
#include "bus.hpp"
#include "gtest/gtest.h"

#include <cstdint>

struct HazardUnitBufferTestCase
{
    std::vector<uint8_t> program;
    uint64_t expectedInstructionId;
    TypeofInstruction expectedType;
    InstructionFlagsInfo expectedFlags;
    DecodedRegistersInfo expectedDecodedRegisters;
    std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)> expectedOperands;
    StageType stalledStage;
    bool hasInstructionID{};
    bool hasType{};
    bool hasFlags{};
    bool hasDecodedRegisters{};
    bool hasOperands{};

};

class HazardUnitTest : public ::testing::TestWithParam<HazardUnitBufferTestCase>
{
    protected:
        Bus bus;
        void SetUp() override;
           
};







#endif // HAZARD_UNIT_TEST_HPP
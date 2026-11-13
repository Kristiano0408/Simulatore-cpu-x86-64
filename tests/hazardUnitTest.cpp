#include "hazardUnitTest.hpp"
#include "bus.hpp"
#include "helpers/debug.hpp"
#include "gtest/gtest.h"


std::vector<HazardUnitBufferTestCase> buildHazardUnitBufferTests()
{
    std::vector<HazardUnitBufferTestCase> tests{};

    //--------------------------------------------------------------------------------
    // Test case 1: fetch stage, decode stalled
    //--------------------------------------------------------------------------------
    HazardUnitBufferTestCase test1{};
    test1.program = {0x48, 0x89, 0xD8}; // MOV RAX, RBX
    test1.expectedInstructionId = 1;
    test1.stalledStage = StageType::DECODE;
    test1.hasInstructionID = true;

    tests.push_back(test1);

    //--------------------------------------------------------------------------------
    // Test case 2: decode stage, operand_fetch stalled
    //--------------------------------------------------------------------------------
    HazardUnitBufferTestCase test2{};
    test2.program = {0x48, 0x89, 0xD8}; // MOV RAX, RBX
    test2.expectedInstructionId = 1;
    test2.stalledStage = StageType::OPERAND_FETCH;
    test2.hasInstructionID = true;
    test2.hasType = true;
    test2.expectedType = TypeofInstruction::MOV;
    test2.hasFlags = true;
    test2.expectedFlags = {.flagsNeeded = flagNeeded(TypeofInstruction::MOV), .flagsAffected = flagAffected(TypeofInstruction::MOV)};
    test2.hasDecodedRegisters = true;
    test2.expectedDecodedRegisters = DecodedRegistersInfo(Register::RAX, Register::RBX, Register::DummyRegister, Register::DummyRegister, false);

    tests.push_back(test2);

    //--------------------------------------------------------------------------------
    // Test case 3: operand_fetch stage (register), execute stalled
    //--------------------------------------------------------------------------------
    HazardUnitBufferTestCase test3{};
    test3.program = {0x48, 0x89, 0xD8}; // MOV RAX, RBX
    test3.expectedInstructionId = 1;
    test3.stalledStage = StageType::EXECUTE;
    test3.hasInstructionID = true;
    test3.hasType = true;
    test3.expectedType = TypeofInstruction::MOV;
    test3.hasFlags = true;
    test3.expectedFlags = {.flagsNeeded = flagNeeded(TypeofInstruction::MOV), .flagsAffected = flagAffected(TypeofInstruction::MOV)};
    test3.hasDecodedRegisters = true;
    test3.expectedDecodedRegisters = DecodedRegistersInfo(Register::RAX, Register::RBX, Register::DummyRegister, Register::DummyRegister, false);
    test3.hasOperands = true;
    test3.expectedOperands = {{
        OperandInfo{.role = OperandRole::SRC, .type = OperandType::REGISTER, .needRead = true, .needWrite = false, .operandRef = Register::RBX},
        OperandInfo{.role = OperandRole::DEST, .type = OperandType::REGISTER, .needRead = false, .needWrite = true, .operandRef = Register::RAX},
        OperandInfo{.role = OperandRole::AUX, .type = OperandType::NONE, .needRead = false, .needWrite = false, .operandRef = Register::DummyRegister}
    }};

    tests.push_back(test3);

    //--------------------------------------------------------------------------------
    // Test case 4: operand_fetch stage (memory), execute stalled
    // MOV RDI, [RSI] -> 48 8B 3E ; RSI = 0x2000
    // OF risolve l'indirizzo ma NON fissa la size dell'operando -> len 0
    //--------------------------------------------------------------------------------
    HazardUnitBufferTestCase test4{};
    test4.program = {0x48, 0x8B, 0x3E}; // MOV RDI, [RSI]
    test4.expectedInstructionId = 1;
    test4.stalledStage = StageType::EXECUTE;
    test4.hasInstructionID = true;
    test4.hasType = true;
    test4.expectedType = TypeofInstruction::MOV;
    test4.hasFlags = true;
    test4.expectedFlags = {.flagsNeeded = flagNeeded(TypeofInstruction::MOV), .flagsAffected = flagAffected(TypeofInstruction::MOV)};
    test4.hasDecodedRegisters = true;
    test4.expectedDecodedRegisters = DecodedRegistersInfo(Register::RSI, Register::RDI, Register::DummyRegister, Register::DummyRegister, false);
    test4.hasOperands = true;
    test4.expectedOperands = {{
        OperandInfo{.role = OperandRole::SRC, .type = OperandType::MEMORY, .needRead = true, .needWrite = false, .operandRef = OperandInfo::MemoryReference{.address = 0x2000, .accessLenght = 0}},
        OperandInfo{.role = OperandRole::DEST, .type = OperandType::REGISTER, .needRead = false, .needWrite = true, .operandRef = Register::RDI},
        OperandInfo{.role = OperandRole::AUX, .type = OperandType::NONE, .needRead = false, .needWrite = false, .operandRef = Register::DummyRegister}
    }};

    tests.push_back(test4);

    //--------------------------------------------------------------------------------
    // Test case 5: execute stage, memory stalled (register)
    //--------------------------------------------------------------------------------
    HazardUnitBufferTestCase test5{};
    test5.program = {0x48, 0x89, 0xD8}; // MOV RAX, RBX
    test5.expectedInstructionId = 1;
    test5.stalledStage = StageType::MEMORY;
    test5.hasInstructionID = true;
    test5.hasType = true;
    test5.expectedType = TypeofInstruction::MOV;
    test5.hasFlags = true;
    test5.expectedFlags = {.flagsNeeded = flagNeeded(TypeofInstruction::MOV), .flagsAffected = flagAffected(TypeofInstruction::MOV)};
    test5.hasDecodedRegisters = true;
    test5.expectedDecodedRegisters = DecodedRegistersInfo(Register::RAX, Register::RBX, Register::DummyRegister, Register::DummyRegister, false);
    test5.hasOperands = true;
    test5.expectedOperands = {{
        OperandInfo{.role = OperandRole::SRC, .type = OperandType::REGISTER, .needRead = true, .needWrite = false, .operandRef = Register::RBX},
        OperandInfo{.role = OperandRole::DEST, .type = OperandType::REGISTER, .needRead = false, .needWrite = true, .operandRef = Register::RAX},
        OperandInfo{.role = OperandRole::AUX, .type = OperandType::NONE, .needRead = false, .needWrite = false, .operandRef = Register::DummyRegister}
    }};

    tests.push_back(test5);



    //--------------------------------------------------------------------------------
    // Test case 6: memory stage, write_back stalled (memoria)
    // MOV RDI, [RSI] -> 48 8B 3E ; RSI = 0x2000
    // in MEMORY l'operando SRC ha indirizzo risolto e size fissa (64 bit -> len 8)
    //--------------------------------------------------------------------------------
    HazardUnitBufferTestCase test6{};
    test6.program = {0x48, 0x8B, 0x3E}; // MOV RDI, [RSI]
    test6.expectedInstructionId = 1;
    test6.stalledStage = StageType::WRITE_BACK;
    test6.hasInstructionID = true;
    test6.hasType = true;
    test6.expectedType = TypeofInstruction::MOV;
    test6.hasFlags = true;
    test6.expectedFlags = {.flagsNeeded = flagNeeded(TypeofInstruction::MOV), .flagsAffected = flagAffected(TypeofInstruction::MOV)};
    test6.hasDecodedRegisters = true;
    test6.expectedDecodedRegisters = DecodedRegistersInfo(Register::RSI, Register::RDI, Register::DummyRegister, Register::DummyRegister, false);
    test6.hasOperands = true;
    test6.expectedOperands = {{
        OperandInfo{.role = OperandRole::SRC, .type = OperandType::MEMORY, .needRead = true, .needWrite = false, .operandRef = OperandInfo::MemoryReference{.address = 0x2000, .accessLenght = 8}},
        OperandInfo{.role = OperandRole::DEST, .type = OperandType::REGISTER, .needRead = false, .needWrite = true, .operandRef = Register::RDI},
        OperandInfo{.role = OperandRole::AUX, .type = OperandType::NONE, .needRead = false, .needWrite = false, .operandRef = Register::DummyRegister}
    }};

    tests.push_back(test6);

    return tests;
}





void HazardUnitTest::SetUp()
{
    const auto& test = GetParam();

    // Load the program into memory
    bus.getMemory().setDataPartial(test.program, 0x0000);

    // RSI = 0x2000 serve ai casi con operando memoria [RSI]
    bus.getCPU().getRegisters().getReg(Register::RSI).raw() = 0x2000;


    //--------------------------------------------------------------------------------
    // Configure pipeline stall
    //--------------------------------------------------------------------------------

    Pipeline& pipeline = bus.getCPU().getPipeline();

    switch (test.stalledStage)
    {
    case StageType::DECODE:
        pipeline.getDecodeStage().setStatus(StageStatus::STALLED);
        break;
    case StageType::OPERAND_FETCH:
        pipeline.getOperandFetchStage().setStatus(StageStatus::STALLED);
        break;
    case StageType::EXECUTE:
        pipeline.getExecuteStage().setStatus(StageStatus::STALLED);
        break;
    case StageType::MEMORY:
        pipeline.getMemoryStage().setStatus(StageStatus::STALLED);
        break;
    case StageType::WRITE_BACK:
        pipeline.getWriteBackStage().setStatus(StageStatus::STALLED);
        break;
    
    default:
        break;
    }

    constexpr size_t MAX_CYCLES = 20;

    for (size_t c = 0; c < MAX_CYCLES; ++c)
    {
        DEBUG_LOG(debugLog("Cycle " + std::to_string(c)));
        bus.tick();
    }

    bus.getCPU().getCacheManager().flushAllCaches();

}



TEST_P(HazardUnitTest, HazardUnitBufferTest)
{
    const auto& test = GetParam();

    HazardUnit& hazardUnit = bus.getCPU().getHazardUnit();
    StageType currentStage = static_cast<StageType>(static_cast<uint8_t>(test.stalledStage) - 1);
    PipelineStageStatus& stageInfo = hazardUnit.getStageInstructionInfo(static_cast<uint8_t>(currentStage));

    SCOPED_TRACE("Test for stage: " + toStringStageType(currentStage));

    if(test.hasInstructionID)
    {
        EXPECT_EQ(stageInfo.instructionInfo.instructionID, test.expectedInstructionId);
    }
    if(test.hasType)
    {
        EXPECT_EQ(stageInfo.instructionInfo.typeofInstruction, test.expectedType);
    }
    if(test.hasFlags)
    {
        EXPECT_EQ(stageInfo.instructionInfo.flagsInfo.flagsNeeded, test.expectedFlags.flagsNeeded);
        EXPECT_EQ(stageInfo.instructionInfo.flagsInfo.flagsAffected, test.expectedFlags.flagsAffected);
    }
    if(test.hasDecodedRegisters)
    {
        EXPECT_EQ(stageInfo.instructionInfo.decodedRegisters.baseReg, test.expectedDecodedRegisters.baseReg);
        EXPECT_EQ(stageInfo.instructionInfo.decodedRegisters.indexReg, test.expectedDecodedRegisters.indexReg);
        EXPECT_EQ(stageInfo.instructionInfo.decodedRegisters.rmReg, test.expectedDecodedRegisters.rmReg);
        EXPECT_EQ(stageInfo.instructionInfo.decodedRegisters.regReg, test.expectedDecodedRegisters.regReg);
        EXPECT_EQ(stageInfo.instructionInfo.decodedRegisters.needRSPimplicitly, test.expectedDecodedRegisters.needRSPimplicitly);
    }
    if(test.hasOperands)
    {
        for (size_t i = 0; i < static_cast<size_t>(OperandRole::COUNT); ++i)
        {
            const OperandInfo& expectedOperand = test.expectedOperands[i];
            const OperandInfo& actualOperand = stageInfo.instructionInfo.operands[i];

            EXPECT_EQ(actualOperand.role, expectedOperand.role);
            EXPECT_EQ(actualOperand.type, expectedOperand.type);
            EXPECT_EQ(actualOperand.needRead, expectedOperand.needRead);
            EXPECT_EQ(actualOperand.needWrite, expectedOperand.needWrite);

            if (actualOperand.type == OperandType::REGISTER)
            {
                EXPECT_EQ(std::get<Register>(actualOperand.operandRef), std::get<Register>(expectedOperand.operandRef));
            }
            else if (actualOperand.type == OperandType::MEMORY)
            {
                const auto& actualMemRef = std::get<OperandInfo::MemoryReference>(actualOperand.operandRef);
                const auto& expectedMemRef = std::get<OperandInfo::MemoryReference>(expectedOperand.operandRef);

                EXPECT_EQ(actualMemRef.address, expectedMemRef.address);
                EXPECT_EQ(actualMemRef.accessLenght, expectedMemRef.accessLenght);
            }
        }
    }

}

INSTANTIATE_TEST_SUITE_P(HazardUnitBufferTests, HazardUnitTest, ::testing::ValuesIn(buildHazardUnitBufferTests()),
    [](const ::testing::TestParamInfo<HazardUnitBufferTestCase>& paramInfo) {
        std::string name = "TestCase_" + std::to_string(paramInfo.index) + "_" + toStringStageType(static_cast<StageType>(static_cast<uint8_t>(paramInfo.param.stalledStage)- 1));
        return name;
    }
);

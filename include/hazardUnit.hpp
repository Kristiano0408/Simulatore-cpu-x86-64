#ifndef HAZARDUNIT_HPP
#define HAZARDUNIT_HPP

#include "pipelineController.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "instruction.hpp"
#include "helpers/operandTypes.hpp"
#include <bitset>
#include <variant>

struct StageStatusInfo
{
    StageStatus status;
    bool stalledGUI;
};

enum class OperandRole : uint8_t
{
    SRC = 0,
    DEST = 1,
    AUX = 2,
    COUNT = 3 //we dont need NONE, there are always 3 operands in every instruction, they could be OperandType::NONE insted 

};



struct OperandInfo
{
    struct MemoryReference 
    {
        uint64_t address{};
        uint64_t accessLenght{};
        Register register1{Register::DummyRegister};  //an address could be calculated from a register,                                                           
        Register register2{Register::DummyRegister};  //   so we need to store the register used for the address calculation
                                                      // max 2 registers are used for address calculation, e.g. [RAX + RBX + 4] or [RAX + 4]

    };

    OperandRole role{OperandRole::COUNT};
    OperandType type{OperandType::NONE};
    bool needRead{false};
    bool needWrite{false};
    
    std::variant<Register, MemoryReference> operandRef;
   
};

constexpr size_t FLAG_COUNT = 12; //least significant 12 bits of RFLAGS register are used for flags, the rest are reserved

using FlagsMask = std::bitset<FLAG_COUNT>;

struct InstructionFlagsInfo
{
    FlagsMask flagsNeeded{};
    FlagsMask flagsAffected{};
};

struct StageInstructionInfo
{
    uint64_t instructionID;
    TypeofInstruction typeofInstruction;
    std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)> operands;
    InstructionFlagsInfo flagsInfo;
};

struct PipelineStageStatus
{
    StageStatusInfo stageStatusinfo;
    StageInstructionInfo instructionInfo;
    

};



class HazardUnit : public TickedDevice, public FaultDevice
{
    public:
        HazardUnit(Pipeline& pipeline);
        ~HazardUnit();
        void execute_operation() override; // Override the execute_operation function to handle hazard detection and resolution
        PipelineController& getPipelineController() {return pipelineController;} // Function to get the PipelineController instance
        Pipeline& getPipeline() {return pipeline;} // Function to get the Pipeline instance

        inline PipelineEventHandler* getEventHandler() { return &eventHandler; }

        void setupEvents();

        static void onFetchCompleteWrapper(void* context);
        static void onDecodeCompleteWrapper(void* context);
        static void onOperandFetchCompleteWrapper(void* context);
        static void onExecuteCompleteWrapper(void* context);
        static void onMemoryStageCompleteWrapper(void* context);
        static void onWriteBackCompleteWrapper(void* context);

        static void onWaitingMemoryWrapperMemoryStage(void* context);
        static void onWaitingMemoryWrapperExecute(void* context);
        static void onWaitingMemoryWrapperFetch(void* context);

        static void onMemoryDoneWrapperFetch(void* context);
        static void onMemoryDoneWrapperExecute(void* context);
        static void onMemoryDoneWrapperMemoryStage(void* context);


    private:
        Pipeline& pipeline; // Reference to the pipeline to access its stages and buffers
        PipelineController pipelineController; // Pipeline controller to manage the pipeline stages and buffers
        PipelineEventHandler eventHandler;
        std::array<PipelineStageStatus, 6> stageInstructionsInfo; // Array to store instruction information for each pipeline stage




};


inline FlagsMask flagNeeded(TypeofInstruction instruction)
{
   switch (instruction)
    {
        case TypeofInstruction::MOV:
        case TypeofInstruction::ADD:
        case TypeofInstruction::SUB:
        case TypeofInstruction::LEA:
        case TypeofInstruction::INC:
        case TypeofInstruction::DEC:
        case TypeofInstruction::NEG:
        case TypeofInstruction::CMP:
        case TypeofInstruction::MUL:
        case TypeofInstruction::IMUL:
        case TypeofInstruction::DIV:
        case TypeofInstruction::IDIV:
        case TypeofInstruction::MOVZX:
        case TypeofInstruction::MOVSX:
        case TypeofInstruction::MOVSXD:
        case TypeofInstruction::PUSH:
        case TypeofInstruction::POP:
        case TypeofInstruction::AND:
        case TypeofInstruction::OR:
        case TypeofInstruction::XOR:
        case TypeofInstruction::NOT:
        case TypeofInstruction::TEST:
        case TypeofInstruction::SALSHL:
        case TypeofInstruction::SAR:
        case TypeofInstruction::SHR:
        case TypeofInstruction::ROL:
        case TypeofInstruction::ROR:
        case TypeofInstruction::JMP:
            return FlagsMask();
        case TypeofInstruction::ADC:
        case TypeofInstruction::SBB:
        case TypeofInstruction::RCL:
        case TypeofInstruction::RCR:
            return FlagsMask().set(static_cast<size_t>(Flagbit::CF));
        default:
            return FlagsMask(); // No flags needed for other instructions
    }
}

inline FlagsMask flagAffected(TypeofInstruction instruction)
{
    switch (instruction)
    {
    case TypeofInstruction::MOV:
    case TypeofInstruction::LEA:
    case TypeofInstruction::DIV:
    case TypeofInstruction::IDIV:
    case TypeofInstruction::MOVZX:
    case TypeofInstruction::MOVSX:
    case TypeofInstruction::MOVSXD:
    case TypeofInstruction::PUSH:
    case TypeofInstruction::POP:
    case TypeofInstruction::NOT:
    case TypeofInstruction::JMP:
        return FlagsMask(); // No flags affected for MOV and LEA
    case TypeofInstruction::ADD:
    case TypeofInstruction::SUB:
    case TypeofInstruction::ADC:
    case TypeofInstruction::SBB:
    case TypeofInstruction::NEG:
    case TypeofInstruction::CMP:
        return FlagsMask().set(static_cast<size_t>(Flagbit::CF))
                          .set(static_cast<size_t>(Flagbit::PF))
                          .set(static_cast<size_t>(Flagbit::AF))
                          .set(static_cast<size_t>(Flagbit::ZF))
                          .set(static_cast<size_t>(Flagbit::SF))
                          .set(static_cast<size_t>(Flagbit::OF));
    case TypeofInstruction::INC:
    case TypeofInstruction::DEC:
        return FlagsMask().set(static_cast<size_t>(Flagbit::PF))
                          .set(static_cast<size_t>(Flagbit::AF))
                          .set(static_cast<size_t>(Flagbit::ZF))
                          .set(static_cast<size_t>(Flagbit::SF))
                          .set(static_cast<size_t>(Flagbit::OF));

    case TypeofInstruction::MUL:
    case TypeofInstruction::IMUL:
    case TypeofInstruction::RCL:
    case TypeofInstruction::RCR:
    case TypeofInstruction::ROL:
    case TypeofInstruction::ROR:
        return FlagsMask().set(static_cast<size_t>(Flagbit::CF))
                          .set(static_cast<size_t>(Flagbit::OF));
    case TypeofInstruction::AND:
    case TypeofInstruction::OR:
    case TypeofInstruction::XOR:
    case TypeofInstruction::TEST:
    case TypeofInstruction::SALSHL:
    case TypeofInstruction::SAR:
    case TypeofInstruction::SHR:
        return FlagsMask().set(static_cast<size_t>(Flagbit::PF))
                          .set(static_cast<size_t>(Flagbit::ZF))
                          .set(static_cast<size_t>(Flagbit::SF))
                          .set(static_cast<size_t>(Flagbit::CF))
                          .set(static_cast<size_t>(Flagbit::OF));
                        
    
    default:
        return FlagsMask(); // No flags affected for other instructions
    }
    
}

const std::array<InstructionFlagsInfo, static_cast<size_t>(TypeofInstruction::COUNT)> instructionFlagsInfo = {{
    {flagNeeded(TypeofInstruction::MOV), flagAffected(TypeofInstruction::MOV)},
    {flagNeeded(TypeofInstruction::ADD), flagAffected(TypeofInstruction::ADD)},
    {flagNeeded(TypeofInstruction::SUB), flagAffected(TypeofInstruction::SUB)},
    {flagNeeded(TypeofInstruction::LEA), flagAffected(TypeofInstruction::LEA)},
    {flagNeeded(TypeofInstruction::ADC), flagAffected(TypeofInstruction::ADC)},
    {flagNeeded(TypeofInstruction::SBB), flagAffected(TypeofInstruction::SBB)},
    {flagNeeded(TypeofInstruction::INC), flagAffected(TypeofInstruction::INC)},
    {flagNeeded(TypeofInstruction::DEC), flagAffected(TypeofInstruction::DEC)},
    {flagNeeded(TypeofInstruction::NEG), flagAffected(TypeofInstruction::NEG)},
    {flagNeeded(TypeofInstruction::CMP), flagAffected(TypeofInstruction::CMP)},
    {flagNeeded(TypeofInstruction::MUL), flagAffected(TypeofInstruction::MUL)},
    {flagNeeded(TypeofInstruction::IMUL), flagAffected(TypeofInstruction::IMUL)},
    {flagNeeded(TypeofInstruction::DIV), flagAffected(TypeofInstruction::DIV)},
    {flagNeeded(TypeofInstruction::IDIV), flagAffected(TypeofInstruction::IDIV)},
    {flagNeeded(TypeofInstruction::MOVZX), flagAffected(TypeofInstruction::MOVZX)},
    {flagNeeded(TypeofInstruction::MOVSX), flagAffected(TypeofInstruction::MOVSX)},
    {flagNeeded(TypeofInstruction::MOVSXD), flagAffected(TypeofInstruction::MOVSXD)},
    {flagNeeded(TypeofInstruction::PUSH), flagAffected(TypeofInstruction::PUSH)},
    {flagNeeded(TypeofInstruction::POP), flagAffected(TypeofInstruction::POP)},
    {flagNeeded(TypeofInstruction::AND), flagAffected(TypeofInstruction::AND)},
    {flagNeeded(TypeofInstruction::OR), flagAffected(TypeofInstruction::OR)},
    {flagNeeded(TypeofInstruction::XOR), flagAffected(TypeofInstruction::XOR)},
    {flagNeeded(TypeofInstruction::NOT), flagAffected(TypeofInstruction::NOT)},
    {flagNeeded(TypeofInstruction::TEST), flagAffected(TypeofInstruction::TEST)},
    {flagNeeded(TypeofInstruction::SALSHL), flagAffected(TypeofInstruction::SALSHL)},
    {flagNeeded(TypeofInstruction::SAR), flagAffected(TypeofInstruction::SAR)},
    {flagNeeded(TypeofInstruction::SHR), flagAffected(TypeofInstruction::SHR)},
    {flagNeeded(TypeofInstruction::RCL), flagAffected(TypeofInstruction::RCL)},
    {flagNeeded(TypeofInstruction::RCR), flagAffected(TypeofInstruction::RCR)},
    {flagNeeded(TypeofInstruction::ROL), flagAffected(TypeofInstruction::ROL)},
    {flagNeeded(TypeofInstruction::ROR), flagAffected(TypeofInstruction::ROR)},
    {flagNeeded(TypeofInstruction::JMP), flagAffected(TypeofInstruction::JMP)}

}};





#endif // HAZARDUNIT_HPP
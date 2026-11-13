#include "hazardUnit.hpp"

HazardUnit::HazardUnit(Pipeline& pipelineRef) : pipeline(pipelineRef), pipelineController(pipelineRef){

    setupEvents(); // Set up the events for the HazardUnit
};

HazardUnit::~HazardUnit() {};


void HazardUnit::setupEvents()
{
    // Register events for stage completions
    eventHandler.setContext(this); // Set the context for the event handler to this PipelineController instance
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::FETCH_COMPLETE, &HazardUnit::onFetchCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::DECODE_COMPLETE, &HazardUnit::onDecodeCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::OPERAND_FETCH_COMPLETE, &HazardUnit::onOperandFetchCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::EXECUTE_COMPLETE, &HazardUnit::onExecuteCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_COMPLETE, &HazardUnit::onMemoryStageCompleteWrapper);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::WRITE_BACK_COMPLETE, &HazardUnit::onWriteBackCompleteWrapper);

    // Register events for memory waiting and completion
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING, &HazardUnit::onWaitingMemoryWrapperMemoryStage);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE, &HazardUnit::onMemoryDoneWrapperMemoryStage);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_EXECUTE, &HazardUnit::onWaitingMemoryWrapperExecute);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_EXECUTE, &HazardUnit::onMemoryDoneWrapperExecute);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_FETCH, &HazardUnit::onWaitingMemoryWrapperFetch);
    eventHandler.registerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_FETCH, &HazardUnit::onMemoryDoneWrapperFetch);

}


void HazardUnit::onFetchCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onFetchComplete();
}

void HazardUnit::onDecodeCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onDecodeComplete();
}

void HazardUnit::onOperandFetchCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onOperandFetchComplete();
}

void HazardUnit::onExecuteCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onExecuteComplete();
}

void HazardUnit::onMemoryStageCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryStageComplete();
}

void HazardUnit::onWriteBackCompleteWrapper(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    WriteBackStage& writeBackStage = hazardUnit->getPipeline().getWriteBackStage();
    Instruction* instr = writeBackStage.peekInstruction();
    if(instr != nullptr && !instr->isEmpty() && instr->getCore().type == TypeofInstruction::JMP)
    {
        std::printf("HazardUnit: Flushing pipeline due to JMP instruction with ID %llu\n", instr->getCore().InstructionId);
        hazardUnit->flushPipeline(instr->getCore().InstructionId);
        hazardUnit->getEraseInstructionQueuedCallbackCPU()(instr->getCore().InstructionId, hazardUnit->getEraseInstructionQueuedCallbackCPUCtx());
        hazardUnit->getEraseInstructionQueuedCallbackCache()(instr->getCore().InstructionId, hazardUnit->getEraseInstructionQueuedCallbackCacheCtx());
        hazardUnit->pipeline.eraseExecuteEngineQueue(instr->getCore().InstructionId);
    }
    hazardUnit->getPipelineController().onWriteBackComplete();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HazardUnit::onWaitingMemoryWrapperMemoryStage(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWaitingMemory(StageType::MEMORY);
}

void HazardUnit::onWaitingMemoryWrapperFetch(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWaitingMemory(StageType::FETCH);
}

void HazardUnit::onWaitingMemoryWrapperExecute(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onWaitingMemory(StageType::EXECUTE);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HazardUnit::onMemoryDoneWrapperFetch(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().\2(StageType::FETCH);
}

void HazardUnit::onMemoryDoneWrapperExecute(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryDone(StageType::EXECUTE);
}

void HazardUnit::onMemoryDoneWrapperMemoryStage(void* context)
{
    HazardUnit* hazardUnit = static_cast<HazardUnit*>(context);
    hazardUnit->getPipelineController().onMemoryDone(StageType::MEMORY);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HazardUnit::flushPipeline(uint64_t instructionId)
{
    DEBUG_LOG(debugLog("HazardUnit: Flushing pipeline starting from instruction ID " + std::to_string(instructionId)));
    
    //flushing fetch stage
    FetchStage& fetchStage = pipeline.getFetchStage();
    const InstructionInFlightInfo& inflightInfo = inflightInstructionInfoCallback(inflightInstructionInfoCtx);
    bool isFetching = inflightInfo.isFetching;
    uint64_t inflightInstructionId = inflightInfo.instructionId;
    if(fetchStage.getCurrentInstructionInfo().instructionId > instructionId || (isFetching && inflightInstructionId > instructionId))
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Fetch stage for instruction ID " + std::to_string(instructionId)));
        fetchStage.flushStage();
        std::printf("HazardUnit: Fetch stage flushed for instruction ID %llu\n", instructionId);
        std::printf("HazardUnit: Fetch stage status after flush: %s\n", toStringStageStatus(fetchStage.getStatus()).c_str());
    }
    //flushing fetch-decode buffer
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.getFetchDecodeBuffer();
    if(fetchDecodeBuffer.valid && fetchDecodeBuffer.instructionInfo.instructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Fetch-Decode buffer for instruction ID " + std::to_string(instructionId)));   
        fetchDecodeBuffer.flush();
    }

    //flushing decode stage
    DecodeStage& decodeStage = pipeline.getDecodeStage();
    if((decodeStage.peekInstruction() != nullptr && decodeStage.peekInstruction()->getCore().InstructionId > instructionId) || (decodeStage.getInstructionToDecodeRef().instructionId > instructionId))
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Decode stage for instruction ID " + std::to_string(instructionId)));
        decodeStage.flushStage();
    }
    //flushing decode-operand fetch buffer
    DecodeOperandFetchBuffer& decodeOperandFetchBuffer = pipeline.getDecodeOperandFetchBuffer();
    if(decodeOperandFetchBuffer.valid && decodeOperandFetchBuffer.peekInstruction() != nullptr && decodeOperandFetchBuffer.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Decode-Operand Fetch buffer for instruction ID " + std::to_string(instructionId)));
        decodeOperandFetchBuffer.flush();
    }

    //flushing operand fetch stage
    OperandFetchStage& operandFetchStage = pipeline.getOperandFetchStage();
    if(operandFetchStage.peekInstruction() != nullptr && operandFetchStage.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Operand Fetch stage for instruction ID " + std::to_string(instructionId)));
        operandFetchStage.flushStage();
    }
    //flushing operand fetch-execute buffer
    OperandFetchExecuteBuffer& operandFetchExecuteBuffer = pipeline.getOperandFetchExecuteBuffer();
    if(operandFetchExecuteBuffer.valid && operandFetchExecuteBuffer.peekInstruction() != nullptr && operandFetchExecuteBuffer.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Operand Fetch-Execute buffer for instruction ID " + std::to_string(instructionId)));
        operandFetchExecuteBuffer.flush();
    }

    //flushing execute stage
    ExecuteStage& executeStage = pipeline.getExecuteStage();
    if(executeStage.peekInstruction() != nullptr && executeStage.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Execute stage for instruction ID " + std::to_string(instructionId)));
        executeStage.flushStage();
    }
    //Flushing execute-memory buffer
    ExecuteMemoryBuffer& executeMemoryBuffer = pipeline.getExecuteMemoryBuffer();
    if(executeMemoryBuffer.valid && executeMemoryBuffer.peekInstruction() != nullptr && executeMemoryBuffer.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Execute-Memory buffer for instruction ID " + std::to_string(instructionId)));
        executeMemoryBuffer.flush();
    }

    //flushing memory stage
    MemoryStage& memoryStage = pipeline.getMemoryStage();
    if(memoryStage.peekInstruction() != nullptr && memoryStage.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Memory stage for instruction ID " + std::to_string(instructionId)));
        memoryStage.flushStage();
    }
    //flushing memory-write back buffer
    MemoryWriteBackBuffer& memoryWriteBackBuffer = pipeline.getMemoryWriteBackBuffer();
    if(memoryWriteBackBuffer.valid && memoryWriteBackBuffer.peekInstruction() != nullptr && memoryWriteBackBuffer.peekInstruction()->getCore().InstructionId > instructionId)
    {
        DEBUG_LOG(debugLog("HazardUnit: Flushing Memory-Write Back buffer for instruction ID " + std::to_string(instructionId)));
        memoryWriteBackBuffer.flush();  
    }
    


    
}




void HazardUnit::updateInstructionOperands(Instruction* instr,std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)>& operands)
{
    if(instr == nullptr || instr->isEmpty())
    {
        for(auto& operand : operands)
        {
            operand = OperandInfo();
        }
        return;
    }

    if(instr->getSourceOperand() != nullptr)
    {
        Operand* srcOperand = instr->getSourceOperand();
        operands[static_cast<size_t>(OperandRole::SRC)].role = OperandRole::SRC;
        operands[static_cast<size_t>(OperandRole::SRC)].type = srcOperand->getType();
        operands[static_cast<size_t>(OperandRole::SRC)].needRead = true;
        operands[static_cast<size_t>(OperandRole::SRC)].needWrite = false;

        if(srcOperand->getType() == OperandType::REGISTER)
        {
            RegOperand* regOperand = static_cast<RegOperand*>(srcOperand);
            Register reg = static_cast<Register>(regOperand->getRegName());
            operands[static_cast<size_t>(OperandRole::SRC)].operandRef = reg;
        }
        else if(srcOperand->getType() == OperandType::MEMORY)
        {
            MemOperand* memOperand = static_cast<MemOperand*>(srcOperand);
            OperandInfo::MemoryReference memRef;
            memRef.address = memOperand->getAddress();
            memRef.accessLenght = srcOperand->getSize() / 8; // Convert size from bits to bytes
            operands[static_cast<size_t>(OperandRole::SRC)].operandRef = memRef;
        }
    }

    if(instr->getDestinationOperand() != nullptr)
    {
        Operand* destOperand = instr->getDestinationOperand();
        operands[static_cast<size_t>(OperandRole::DEST)].role = OperandRole::DEST;
        operands[static_cast<size_t>(OperandRole::DEST)].type = destOperand->getType();
        operands[static_cast<size_t>(OperandRole::DEST)].needRead = instr->getFlags().destNeedRead;
        operands[static_cast<size_t>(OperandRole::DEST)].needWrite = true;

        if(destOperand->getType() == OperandType::REGISTER)
        {
            RegOperand* regOperand = static_cast<RegOperand*>(destOperand);
            Register reg = static_cast<Register>(regOperand->getRegName());
            operands[static_cast<size_t>(OperandRole::DEST)].operandRef = reg;
        }
        else if(destOperand->getType() == OperandType::MEMORY)
        {
            MemOperand* memOperand = static_cast<MemOperand*>(destOperand);
            OperandInfo::MemoryReference memRef;
            memRef.address = memOperand->getAddress();
            memRef.accessLenght = destOperand->getSize() / 8; // Convert size from bits to bytes
            operands[static_cast<size_t>(OperandRole::DEST)].operandRef = memRef;
        }
    }

    if(instr->getAuxOperand() != nullptr)
    {
        Operand* auxOperand = instr->getAuxOperand();
        operands[static_cast<size_t>(OperandRole::AUX)].role = OperandRole::AUX;
        operands[static_cast<size_t>(OperandRole::AUX)].type = auxOperand->getType();
        operands[static_cast<size_t>(OperandRole::AUX)].needRead = instr->getFlags().auxNeedRead;
        operands[static_cast<size_t>(OperandRole::AUX)].needWrite = instr->getFlags().auxNeedWrite;

        if(auxOperand->getType() == OperandType::REGISTER)
        {
            RegOperand* regOperand = static_cast<RegOperand*>(auxOperand);
            Register reg = static_cast<Register>(regOperand->getRegName());
            operands[static_cast<size_t>(OperandRole::AUX)].operandRef = reg;
        }
    
    }

    return;
   
}








void HazardUnit::updateFetchStageStatus()
{
    
    PipelineStageStatus& fetchStageInfo = stageInstructionsInfo[static_cast<size_t>(StageType::FETCH)];
    uint64_t& instructionId = fetchStageInfo.instructionInfo.instructionID;
    //get the data from the pipeline
    FetchStage& fetchStage = pipeline.getFetchStage();
    FetchDecodeBuffer& fetchDecodeBuffer = pipeline.getFetchDecodeBuffer();

   
    if(fetchDecodeBuffer.valid)
    {
        DEBUG_LOG(debugLog("Fetch stage info updated from fetch-decode buffer"));
        instructionId = fetchDecodeBuffer.instructionInfo.instructionId;
    }
    else if (fetchStage.getCurrentInstructionInfoRef().totalLength > 0)
    {   
        DEBUG_LOG(debugLog("Fetch stage info updated from fetch stage"));
        //extremly rare case, almost impossible, but for security reasons 
        instructionId = fetchStage.getCurrentInstructionInfoRef().instructionId;
    }

    return;

}

void HazardUnit::updateDecodeStageStatus()
{
    PipelineStageStatus& decodeStageInfo = stageInstructionsInfo[static_cast<size_t>(StageType::DECODE)];
    uint64_t& instructionId = decodeStageInfo.instructionInfo.instructionID;
    TypeofInstruction& type = decodeStageInfo.instructionInfo.typeofInstruction;
    InstructionFlagsInfo& flags = decodeStageInfo.instructionInfo.flagsInfo;
    DecodedRegistersInfo& decodedRegisters = decodeStageInfo.instructionInfo.decodedRegisters;




    DecodeStage& decodeStage = pipeline.getDecodeStage();
    DecodeOperandFetchBuffer& decodeOperandFetchBuffer = pipeline.getDecodeOperandFetchBuffer();

   
    if(decodeOperandFetchBuffer.valid)
    {
        Instruction* instr = decodeOperandFetchBuffer.decodedInstruction.get();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
    }
    else if (decodeStage.peekInstruction() != nullptr)
    {
        InstructionCore& core = decodeStage.peekInstruction()->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
    }
    else
    {
        instructionId = 0;
        type = TypeofInstruction::COUNT;
        flags = {.flagsNeeded = FlagsMask(), .flagsAffected = FlagsMask()};
        decodedRegisters = DecodedRegistersInfo();

    }

    return;
    
}

void HazardUnit::updateOperandFetchStageStatus()
{
    PipelineStageStatus& operandFetchStageInfo = stageInstructionsInfo[static_cast<size_t>(StageType::OPERAND_FETCH)];
    uint64_t& instructionId = operandFetchStageInfo.instructionInfo.instructionID;
    TypeofInstruction& type = operandFetchStageInfo.instructionInfo.typeofInstruction;
    InstructionFlagsInfo& flags = operandFetchStageInfo.instructionInfo.flagsInfo;
    DecodedRegistersInfo& decodedRegisters = operandFetchStageInfo.instructionInfo.decodedRegisters;
    std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)>& operands = operandFetchStageInfo.instructionInfo.operands;




    OperandFetchStage& operandFetchStage = pipeline.getOperandFetchStage();
    OperandFetchExecuteBuffer& operandFetchExecuteBuffer = pipeline.getOperandFetchExecuteBuffer();

   
    if(operandFetchExecuteBuffer.valid)
    {
        Instruction* instr = operandFetchExecuteBuffer.instructionWithOperands.get();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else if (operandFetchStage.peekInstruction() != nullptr)
    {
        Instruction* instr = operandFetchStage.peekInstruction();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else
    {
        instructionId = 0;
        type = TypeofInstruction::COUNT;
        flags = {.flagsNeeded = FlagsMask(), .flagsAffected = FlagsMask()};
        decodedRegisters = DecodedRegistersInfo();
        updateInstructionOperands(nullptr, operands);

    }

    return;
    



}

void HazardUnit::updateExecuteStageStatus()
{
    PipelineStageStatus& executeStageInfo = stageInstructionsInfo[static_cast<size_t>(StageType::EXECUTE)];
    uint64_t& instructionId = executeStageInfo.instructionInfo.instructionID;
    TypeofInstruction& type = executeStageInfo.instructionInfo.typeofInstruction;
    InstructionFlagsInfo& flags = executeStageInfo.instructionInfo.flagsInfo;
    DecodedRegistersInfo& decodedRegisters = executeStageInfo.instructionInfo.decodedRegisters;
    std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)>& operands = executeStageInfo.instructionInfo.operands;

    ExecuteStage& executeStage = pipeline.getExecuteStage();
    ExecuteMemoryBuffer& executeMemoryBuffer = pipeline.getExecuteMemoryBuffer();

   
    if(executeMemoryBuffer.valid)
    {
        Instruction* instr = executeMemoryBuffer.executedInstruction.get();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else if (executeStage.peekInstruction() != nullptr)
    {
        Instruction* instr = executeStage.peekInstruction();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else
    {
        instructionId = 0;
        type = TypeofInstruction::COUNT;
        flags = {.flagsNeeded = FlagsMask(), .flagsAffected = FlagsMask()};
        decodedRegisters = DecodedRegistersInfo();
        updateInstructionOperands(nullptr, operands);

    }

    return;

}


void HazardUnit::updateMemoryStageStatus()
{
    PipelineStageStatus& memoryStageInfo = stageInstructionsInfo[static_cast<size_t>(StageType::MEMORY)];
    uint64_t& instructionId = memoryStageInfo.instructionInfo.instructionID;
    TypeofInstruction& type = memoryStageInfo.instructionInfo.typeofInstruction;
    InstructionFlagsInfo& flags = memoryStageInfo.instructionInfo.flagsInfo;
    DecodedRegistersInfo& decodedRegisters = memoryStageInfo.instructionInfo.decodedRegisters;
    std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)>& operands = memoryStageInfo.instructionInfo.operands;

    MemoryStage& memoryStage = pipeline.getMemoryStage();
    MemoryWriteBackBuffer& memoryWriteBackBuffer = pipeline.getMemoryWriteBackBuffer();

   
    if(memoryWriteBackBuffer.valid)
    {
        Instruction* instr = memoryWriteBackBuffer.memoryAccessedInstruction.get();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else if (memoryStage.peekInstruction() != nullptr)
    {
        Instruction* instr = memoryStage.peekInstruction();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else
    {
        instructionId = 0;
        type = TypeofInstruction::COUNT;
        flags = {.flagsNeeded = FlagsMask(), .flagsAffected = FlagsMask()};
        decodedRegisters = DecodedRegistersInfo();
        updateInstructionOperands(nullptr, operands);

    }

    return;

}

void HazardUnit::updateWriteBackStageStatus()
{
    PipelineStageStatus& writeBackStageInfo = stageInstructionsInfo[static_cast<size_t>(StageType::WRITE_BACK)];
    uint64_t& instructionId = writeBackStageInfo.instructionInfo.instructionID;
    TypeofInstruction& type = writeBackStageInfo.instructionInfo.typeofInstruction;
    InstructionFlagsInfo& flags = writeBackStageInfo.instructionInfo.flagsInfo;
    DecodedRegistersInfo& decodedRegisters = writeBackStageInfo.instructionInfo.decodedRegisters;
    std::array<OperandInfo, static_cast<size_t>(OperandRole::COUNT)>& operands = writeBackStageInfo.instructionInfo.operands;

    WriteBackStage& writeBackStage = pipeline.getWriteBackStage();

   
    if(writeBackStage.peekInstruction() != nullptr)
    {
        
        Instruction* instr = writeBackStage.peekInstruction();
        InstructionCore& core = instr->getCore();
        instructionId = core.InstructionId;
        type = core.type;
        flags = instructionFlagsInfo[static_cast<size_t>(type)];
        decodedRegisters = DecodedRegistersInfo(core.rmReg, core.regReg, core.indexReg, core.baseReg, core.needRSPimplicitly);
        updateInstructionOperands(instr, operands);
    }
    else
    {
        instructionId = 0;
        type = TypeofInstruction::COUNT;
        flags = {.flagsNeeded = FlagsMask(), .flagsAffected = FlagsMask()};
        decodedRegisters = DecodedRegistersInfo();
        updateInstructionOperands(nullptr, operands);

    }

    return;

}


void  HazardUnit::execute_operation() {

    updateWriteBackStageStatus(); // Update the status of the write back stage // we need this info for detecting data hazards and control hazards
    updateMemoryStageStatus(); // Update the status of the memory stage // we need this info for detecting data hazards and control hazards
    updateExecuteStageStatus(); // Update the status of the execute stage // we need this info for detecting data hazards and control hazards
    updateOperandFetchStageStatus(); // Update the status of the operand fetch stage // we needhis info to block reg and memory access in execution 
    updateDecodeStageStatus(); // Update the status of the decode stage // we nedd his info to block OF for adress calculation 
    updateFetchStageStatus(); // Update the status of the fetch stage // we need this info dor detecting control hazards( id > idJMP)*/
    
    
    
  
   



    /*if(detectAdressCalculationHazard()); // Detect address calculation hazards(confronting the instruction in decode stage or buffer  with the instruction OF,EX,WB to see if they are using the same registers)
    {
        pipeline.getDecodeOperandFetchBuffer().stalled = true;
    }
    if(detectRegisterHazard())
    {
        pipeline.getOperandFetchExecuteBuffer().stalled = true;
    }
    if(detectMemoryHazard())
    {
        pipeline.getOperandFetchExecuteBuffer().stalled = true;
    }*/
    // Implement hazard detection and resolution logic here
    // This function will be called when the HazardUnit is ticked and the ticks_needed is reached
    DEBUG_LOG(debugLog("HazardUnit executing operation for hazard detection and resolution..."));
    // Example: Check for data hazards, control hazards, etc., and take appropriate actions
}
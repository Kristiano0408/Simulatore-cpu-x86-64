#include "operandEngine.hpp"
#include "cache/cacheManager.hpp"
#include "cpu.hpp"
#include "eventHandler.hpp"
#include "eventLog.hpp"
#include "instruction.hpp"

void OperandEngine::sendReadRequest(Instruction* instruction, Operand* srcOperand, Operand* destOperand)
{
    readQueue.push({instruction, srcOperand, destOperand});
}

void OperandEngine::sendWriteRequest(Instruction* instruction, Operand* operand, uint64_t value)
{
    writeQueue.push({instruction, operand, value});
}

void OperandEngine::execute_operation()
{
    // Process write operations
    if (!writeQueue.empty())
    {
        OperandContextWrite context = writeQueue.front();
        writeOperand(context.instruction, context.operand, context.value);
    }

    // Process read operations
    if (!readQueue.empty())
    {
        OperandContextRead context = readQueue.front();
        temporaryValues& tempValues = context.instruction->getTemporaryValuesRef();

        if (!tempValues.isSrcValueReady)
            readOperand(context.instruction, context.srcOperand, WhichOperand::SOURCE);

        if (!tempValues.isDestValueReady)
            readOperand(context.instruction, context.destOperand, WhichOperand::DESTINATION);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define readValue(type, getValueFunction, resultVariable)                                                              \
    DEBUG_LOG(debugLog("Reading from " + std::string(type) + " operand."));                                            \
    (resultVariable) = getValueFunction;                                                                               \
    if ((resultVariable).status == OperandStatus::OK && instruction->getTemporaryValuesRef().isSrcValueReady &&        \
        instruction->getTemporaryValuesRef().isDestValueReady)                                                         \
    {                                                                                                                  \
        DEBUG_LOG(debugLog("OperandEngine: " + std::string(type) + " read completed for instruction with ID " +        \
                           std::to_string(instruction->getCore().InstructionId)));                                     \
        executeEngineEventHandler.triggerExecuteEngineEvent(                                                           \
            EventHandlerExecuteEngineEventType::OPERAND_COMPLETE_READ_EXECUTION);                                      \
        readQueue.pop(); /* Remove the completed read operation from the queue */                                      \
    }

void OperandEngine::readOperand(Instruction* instruction, Operand* operand, WhichOperand whichoperand)
{
    DEBUG_LOG(debugLog("OperandEngine: Reading operand of type " +
                       std::to_string(static_cast<uint8_t>(operand->getType())) + " for instruction with ID " +
                       std::to_string(instruction->getCore().InstructionId)));
    OperandResult result;
    switch (operand->getType())
    {
        case OperandType::REGISTER:
            readValue("register", getRegisterValue(instruction, operand, whichoperand), result);
            return;
        case OperandType::MEMORY:
            readValue(
                "memory",
                getMemoryValue(instruction, operand, executeEngineEventHandler.getContext(),
                               executeEngineEventHandler.getCallback(EventHandlerExecuteEngineEventType::MEMORY_DONE),
                               whichoperand),
                result);
            return;
        case OperandType::IMMEDIATE:
            readValue("immediate", getImmediateValue(instruction, operand, whichoperand), result);
            return;
        default:
            return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define writeValue(type, setValueFunction, resultVariable, callbackType)                                               \
    DEBUG_LOG(debugLog("Writing to " + std::string(type) + " operand."));                                              \
    (resultVariable) = setValueFunction;                                                                               \
    if ((resultVariable).status == OperandStatus::OK)                                                                  \
    {                                                                                                                  \
        DEBUG_LOG(debugLog("OperandEngine: " + std::string(type) + " write completed for instruction with ID " +       \
                           std::to_string(instruction->getCore().InstructionId)));                                     \
        executeEngineEventHandler.triggerExecuteEngineEvent(callbackType);                                             \
        writeQueue.pop(); /* Remove the completed write operation from the queue */                                    \
    }

void OperandEngine::writeOperand(Instruction* instruction, Operand* operand, uint64_t value)
{
    DEBUG_LOG(debugLog("OperandEngine: Writing value " + std::to_string(value) + " to operand of type " +
                       std::to_string(static_cast<uint8_t>(operand->getType())) + " for instruction with ID " +
                       std::to_string(instruction->getCore().InstructionId)));
    OperandResult result;
    switch (operand->getType())
    {
        case OperandType::REGISTER:
            writeValue("register", setRegisterValue(operand, value), result,
                       EventHandlerExecuteEngineEventType::OPERAND_COMPLETE_WRITE_WRITEBACK);
            return;
        case OperandType::MEMORY:
            writeValue(
                "memory",
                setMemoryValue(instruction, operand, value, executeEngineEventHandler.getContext(),
                               executeEngineEventHandler.getCallback(EventHandlerExecuteEngineEventType::MEMORY_DONE)),
                result, EventHandlerExecuteEngineEventType::OPERAND_COMPLETE_WRITE_MEMORY);
            return;
        default:
            return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OperandResult OperandEngine::setRegisterValue(Operand* operand, uint64_t value)
{
    DEBUG_LOG(debugLog("OperandEngine: Setting register value to " + std::to_string(value) + " for operand of type " +
                       std::to_string(static_cast<uint8_t>(operand->getType()))));
    auto* regOperand = static_cast<RegOperand*>(operand);

    Result result;
    uint8_t bitCount = operand->getSize();
    const uint64_t mask = (bitCount == 64) ? ~0ULL : ((1ULL << bitCount) - 1);

    // Applica la maschera coerente alla size del tipo
    const uint64_t valueMasked = value & mask;

    uint64_t& regValue = regOperand->getReg();
    if (bitCount == 32)
        regValue = valueMasked; // For 32-bit registers, overwrite the entire register
    else
        regValue = (regValue & ~mask) | valueMasked; // Altrimenti, sovrascrivi l'intero registro

    result.success = true;
    result.errorInfo.source = ComponentType::OPERAND;
    result.errorInfo.event = EventType::OPERAND_SET_VALUE;
    result.errorInfo.error = ErrorType::NONE;

    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), valueMasked);

    return {.status=OperandStatus::OK, .value=0};
}

OperandResult OperandEngine::getRegisterValue(Instruction* instruction, Operand* operand, WhichOperand whichoperand)
{
    auto* regOperand = static_cast<RegOperand*>(operand);
    Result result;
    uint64_t value{};
    uint64_t mask{};

    if (regOperand->getSize() == 64)
        mask = 0xFFFFFFFFFFFFFFFF;
    else
        mask = ((1ULL << (regOperand->getSize())) - 1);

    value = regOperand->getReg() & mask;

    result.success = true;
    result.errorInfo.source = ComponentType::OPERAND;
    result.errorInfo.event = EventType::OPERAND_GET_VALUE;
    result.errorInfo.error = ErrorType::NONE;
    switch (whichoperand)
    {
        case WhichOperand::SOURCE:
            instruction->getTemporaryValuesRef().srcValue = value;
            instruction->getTemporaryValuesRef().isSrcValueReady = true;
            break;
        case WhichOperand::DESTINATION:
            instruction->getTemporaryValuesRef().destValue = value;
            instruction->getTemporaryValuesRef().isDestValueReady = true;
            break;
    }
    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);

    return {.status=OperandStatus::OK, .value=value};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OperandResult OperandEngine::setImmediateValue(Operand* operand, uint64_t value)
{
    auto* immOperand = static_cast<ImmediateOperand*>(operand);
    Result result;

    immOperand->setValue(value);

    result.success = true;
    result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::OPERAND_SET_VALUE, .error=ErrorType::NONE};
    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);

    return {.status=OperandStatus::OK, .value=0};
}

OperandResult OperandEngine::getImmediateValue(Instruction* instruction, Operand* operand, WhichOperand whichoperand)
{
    auto* immOperand = static_cast<ImmediateOperand*>(operand);

    Result result;
    result.success = true;
    result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::OPERAND_GET_VALUE, .error=ErrorType::NONE};
    switch (whichoperand)
    {
        case WhichOperand::SOURCE:
            instruction->getTemporaryValuesRef().srcValue = immOperand->getValue();
            instruction->getTemporaryValuesRef().isSrcValueReady = true;
            break;
        case WhichOperand::DESTINATION:
            instruction->getTemporaryValuesRef().destValue = immOperand->getValue();
            instruction->getTemporaryValuesRef().isDestValueReady = true;
            break;
    }

    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), immOperand->getValue());
    return {.status=OperandStatus::OK, .value=immOperand->getValue()};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OperandResult OperandEngine::getMemoryValue(Instruction* instruction, Operand* operand, void* callbackContext,
                                            void (*callback)(void* context), WhichOperand whichoperand)
{
    InstructionCore& core = instruction->getCore();
    MemOperand* memOperand = static_cast<MemOperand*>(operand);

    Result result;
    if (memOperand->getSize() == 0)
    {
        result.success = false;
        result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::ERROR, .error=ErrorType::INVALID_SIZE};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), uint64_t{0});
        return {.status=OperandStatus::ERROR, .value=0};
    }

    // extracting value first from cache, then from memory if necessary
    uint64_t value{};

    if (!memOperand->isReadRequestSent())
    {
        // sending the read request to the cache manager

        DEBUG_LOG(debugLog("MemOperand: Sending read request to address " + to_string_hex(memOperand->getAddress()) +
                           " with size " + std::to_string(memOperand->getSize()) + " bytes."));
        memOperand->setReadRequestSent(true);
        TypeofData dataTypeSize;
        switch (memOperand->getSize())
        {
            case 8:
                dataTypeSize = TypeofData::UINT8_T;
                break;
            case 16:
                dataTypeSize = TypeofData::UINT16_T;
                break;
            case 32:
                dataTypeSize = TypeofData::UINT32_T;
                break;
            case 64:
                dataTypeSize = TypeofData::UINT64_T;
                break;
            default:
                dataTypeSize = TypeofData::UNKNOWN;
                break;
        }
        CacheRequest request(RequestType::READ, dataTypeSize, dataMemoryInterface.type, memOperand->getAddress(),
                             MaxCPUInstructionLength{}, false, core.InstructionId, callbackContext, callback);
        cacheManager.enqueRequest(std::move(request), dataMemoryInterface.type);

        result.success = false;
        result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::ERROR, .error=ErrorType::WAITING_MEMORY};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), uint64_t{0});
        return {.status=OperandStatus::WAITING_MEMORY, .value=0};
    }
    
            // request already sent, waiting for completion

        DEBUG_LOG(debugLog("MemOperand: Read request already sent to address " +
                           to_string_hex(memOperand->getAddress()) + ". Waiting for completion."));

        MaxCPUInstructionLength response;
        bool found = false;
        // checking if the request is completed
        cpu.findCacheResponse(core.InstructionId, response, found);

        if (found)
        {
            // request completed
            DEBUG_LOG(debugLog("MemOperand: Read request completed for address " +
                               to_string_hex(memOperand->getAddress()) + "."));

            // extracting the result
            uint8_t numBytes = memOperand->getSize() / 8; // Convert size in bits to bytes
            std::memcpy(&value, response.data(), numBytes);

            cpu.eraseCacheResponseIfFound(core.InstructionId);
            memOperand->setReadRequestSent(false); // resetting the flag for future requests

            result.success = true;
            result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::OPERAND_GET_VALUE, .error=ErrorType::NONE};
            EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);
            switch (whichoperand)
            {
                case WhichOperand::SOURCE:
                    instruction->getTemporaryValuesRef().srcValue = value;
                    instruction->getTemporaryValuesRef().isSrcValueReady = true;
                    break;
                case WhichOperand::DESTINATION:
                    instruction->getTemporaryValuesRef().destValue = value;
                    instruction->getTemporaryValuesRef().isDestValueReady = true;
                    break;
            }

            return {.status=OperandStatus::OK, .value=value};
        }
        
                    // request not completed dhdhhdhd
            DEBUG_LOG(debugLog("MemOperand: Read request not completed for address " +
                               to_string_hex(memOperand->getAddress()) + "."));
            return {.status=OperandStatus::WAITING_MEMORY, .value=0};
       
   
}

OperandResult OperandEngine::setMemoryValue(Instruction* instruction, Operand* operand, uint64_t value,
                                            void* callbackContext, void (*callback)(void* context))
{
    InstructionCore& core = instruction->getCore();
    auto* memOperand = static_cast<MemOperand*>(operand);

    Result result;

    if (memOperand->getSize() == 0)
    {
        result.success = false;
        result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::ERROR, .error=ErrorType::INVALID_SIZE};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);
        return {.status=OperandStatus::ERROR, .value=0};
    }

    TypeofData dataTypeSize = TypeofData::UNKNOWN;
    MaxCPUInstructionLength out{};                // Buffer per i dati da scrivere, dimensione massima di 16 byte
    uint8_t numBytes = memOperand->getSize() / 8; // Convert size in bits to bytes
    std::memcpy(out.data(), &value, numBytes);    // Copia i dati in out, rispettando la size dell'operando

    switch (memOperand->getSize())
    {
        case 1:
            dataTypeSize = TypeofData::UINT8_T;
            break;
        case 2:
            dataTypeSize = TypeofData::UINT16_T;
            break;
        case 4:
            dataTypeSize = TypeofData::UINT32_T;
            break;
        case 8:
            dataTypeSize = TypeofData::UINT64_T;
            break;
        default:
            dataTypeSize = TypeofData::UNKNOWN;
            break;
    }

    if (!memOperand->isWriteRequestSent())
    {
        // sending the write request to the cache manager

        // debugLog("MemOperand: Sending write request to address " + to_string_hex(this->address) + " with value " +
        // to_string_hex(v) + " and size " + std::to_string(this->size) + " bytes.");
        memOperand->setWriteRequestSent(true);

        CacheRequest request(RequestType::WRITE, dataTypeSize, dataMemoryInterface.type, memOperand->getAddress(), out,
                             false, core.InstructionId, callbackContext, callback);
        cacheManager.enqueRequest(std::move(request), dataMemoryInterface.type);
        result.success = false;
        result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::ERROR, .error=ErrorType::WAITING_MEMORY};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), uint64_t{0});
        return {.status=OperandStatus::WAITING_MEMORY, .value=0};
    }
    
            // request already sent, waiting for completion

        DEBUG_LOG(debugLog("MemOperand: Write request already sent to address " +
                           to_string_hex(memOperand->getAddress()) + ". Waiting for completion."));

        // checking if the request is completed
        MaxCPUInstructionLength response;
        bool found = false;

        cpu.findCacheResponse(core.InstructionId, response, found);

        if (found)
        {
            // request completed
            DEBUG_LOG(debugLog("MemOperand: Write request completed for address " +
                               to_string_hex(memOperand->getAddress()) + "."));

            // extracting the result
            cpu.eraseCacheResponseIfFound(core.InstructionId);
            memOperand->setWriteRequestSent(false); // resetting the flag for future requests

            result.success = true;
            result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::OPERAND_SET_VALUE, .error=ErrorType::NONE};
            EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);

            return {.status=OperandStatus::OK, .value=0};
        }
        
                    // request not completed
            DEBUG_LOG(debugLog("MemOperand: Write request not completed for address " +
                               to_string_hex(memOperand->getAddress()) + "."));
            result.success = false;
            result.errorInfo = {.source=ComponentType::OPERAND, .event=EventType::ERROR, .error=ErrorType::WAITING_MEMORY};
            EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);
            return {.status=OperandStatus::WAITING_MEMORY, .value=0};
       
   
}

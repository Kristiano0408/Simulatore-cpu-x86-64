#include "operandEngine.hpp"
#include "instruction.hpp"
#include "cacheManager.hpp"
#include "eventLog.hpp"
#include "cpu.hpp"

OperandResult OperandEngine::readOperand(Instruction* instruction, Operand* operand, void* context, void(*callback)(void* context))
{
    OperandResult result;
    switch (operand->getType()) {
        case OperandType::REGISTER:
        DEBUG_LOG(debugLog("Reading from register operand."));
            result = getRegisterValue(operand);
            return result;
        case OperandType::MEMORY:
        DEBUG_LOG(debugLog("Reading from memory operand."));
            result = getMemoryValue(instruction, operand, context, callback);
            return result;
        case OperandType::IMMEDIATE:
        DEBUG_LOG(debugLog("Reading from immediate operand."));
            result = getImmediateValue(operand);
            return result;
        default:
            return {OperandStatus::ERROR, 0};
    }
}

OperandResult OperandEngine::writeOperand(Instruction* instruction, Operand* operand, uint64_t value, void* context, void(*callback)(void* context))
{
    OperandResult result;
    switch (operand->getType()) {
        case OperandType::REGISTER:
            result = setRegisterValue(operand, value);
            return result;
        case OperandType::MEMORY:
            result = setMemoryValue(instruction, operand, value, context, callback);
            return result;
        case OperandType::IMMEDIATE:
            result = setImmediateValue(operand, value);
            return result;
        default:
            return {OperandStatus::ERROR, 0};
    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OperandResult OperandEngine::setRegisterValue(Operand* operand, uint64_t value) 
{
    RegOperand* regOperand = static_cast<RegOperand*>(operand);

    Result result;
    uint64_t bitCount = operand->getSize();
    const uint64_t mask = (bitCount == 64) ? ~0ULL : ((1ULL << bitCount) - 1);

    // Applica la maschera coerente alla size del tipo
    const uint64_t valueMasked = value & mask;

    regOperand->getReg() = (regOperand->getReg() & ~mask) | valueMasked;


    result.success = true;
    result.errorInfo.source = ComponentType::OPERAND;
    result.errorInfo.event = EventType::OPERAND_SET_VALUE;
    result.errorInfo.error = ErrorType::NONE;

    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), valueMasked);

    return {OperandStatus::OK, 0};
    
}

OperandResult OperandEngine::getRegisterValue(Operand* operand) 
{
    RegOperand* regOperand = static_cast<RegOperand*>(operand);
    Result result;
    uint64_t value {};
    int64_t mask {};

    if(regOperand->getSize() == 64)
        mask = 0xFFFFFFFFFFFFFFFF;
    else
        mask = ((1ULL << (regOperand->getSize())) - 1);

    value = regOperand->getReg() & mask;

    result.success = true;
    result.errorInfo.source = ComponentType::OPERAND;
    result.errorInfo.event = EventType::OPERAND_GET_VALUE;
    result.errorInfo.error = ErrorType::NONE;
    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);


    return {OperandStatus::OK, value};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


OperandResult OperandEngine::setImmediateValue(Operand* operand, uint64_t value) 
{
    ImmediateOperand* immOperand = static_cast<ImmediateOperand*>(operand);
    Result result;


    immOperand->setValue(value);

    result.success = true;
    result.errorInfo = {ComponentType::OPERAND, EventType::OPERAND_SET_VALUE, ErrorType::NONE};
    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);

    return {OperandStatus::OK, 0}; 
}


OperandResult OperandEngine::getImmediateValue(Operand* operand) 
{
    ImmediateOperand* immOperand = static_cast<ImmediateOperand*>(operand);
  
    Result result;
    result.success = true;
    result.errorInfo = {ComponentType::OPERAND, EventType::OPERAND_GET_VALUE, ErrorType::NONE };
    EventLog::getInstance().pushOperandDataLogEntry(std::move(result), immOperand->getValue());
    return {OperandStatus::OK, immOperand->getValue()};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OperandResult OperandEngine::getMemoryValue(Instruction* instruction, Operand* operand, void* callbackContext, void(*callback)(void* context))
{
    InstructionCore& core = instruction->getCore();
    MemOperand* memOperand = static_cast<MemOperand*>(operand);

    Result result;
    if (memOperand->getSize() == 0)
    {
        result.success = false;
        result.errorInfo = {ComponentType::OPERAND, EventType::ERROR, ErrorType::INVALID_SIZE};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), uint64_t{0});
        return {OperandStatus::ERROR, 0};
    }

    //extracting value first from cache, then from memory if necessary
    uint64_t value {};

    if(!memOperand->isReadRequestSent())
    {
        //sending the read request to the cache manager

        DEBUG_LOG(debugLog("MemOperand: Sending read request to address " + to_string_hex(memOperand->getAddress()) + " with size " + std::to_string(memOperand->getSize()) + " bytes."));
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

        cacheManager.enqueRequest(CacheRequest(RequestType::READ, dataTypeSize, memOperand->getAddress(),MaxCPUInstructionLength{}, false, core.InstructionId, callbackContext, callback));

        result.success = false;
        result.errorInfo = {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), uint64_t{0});
        return {OperandStatus::WAITING_MEMORY, 0};
    }
    else
    {
        //request already sent, waiting for completion

        DEBUG_LOG(debugLog("MemOperand: Read request already sent to address " + to_string_hex(memOperand->getAddress()) + ". Waiting for completion."));

        MaxCPUInstructionLength response;
        bool found = false;
        //checking if the request is completed
        cpu.findCacheResponse(core.InstructionId, response, found);

        if (found)
        {
            //request completed
            DEBUG_LOG(debugLog("MemOperand: Read request completed for address " + to_string_hex(memOperand->getAddress()) + "."));
            
            //extracting the result
            std::memcpy(&value, response.data(), memOperand->getSize());
            
            cpu.eraseCacheResponseIfFound(core.InstructionId);
            memOperand->setReadRequestSent(false); //resetting the flag for future requests

            result.success = true;
            result.errorInfo = {ComponentType::OPERAND, EventType::OPERAND_GET_VALUE, ErrorType::NONE};
            EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);

            return {OperandStatus::OK, value};
        }
        else
        {
            //request not completed dhdhhdhd
            DEBUG_LOG(debugLog("MemOperand: Read request not completed for address " + to_string_hex(memOperand->getAddress()) + "."));
            return {OperandStatus::WAITING_MEMORY, 0};
        }
    }

}

OperandResult OperandEngine::setMemoryValue(Instruction* instruction, Operand* operand, uint64_t value, void* callbackContext, void(*callback)(void* context))
{
    InstructionCore& core = instruction->getCore();
    MemOperand* memOperand = static_cast<MemOperand*>(operand);

    Result result;


    if (memOperand->getSize() == 0)
    {
        result.success = false;
        result.errorInfo = {ComponentType::OPERAND, EventType::ERROR, ErrorType::INVALID_SIZE};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);
        return {OperandStatus::ERROR, 0};
    }

    TypeofData dataTypeSize = TypeofData::UNKNOWN;
    MaxCPUInstructionLength out{}; // Buffer per i dati da scrivere, dimensione massima di 16 byte
    std::memcpy(out.data(), &value, memOperand->getSize()); // Copia i dati in out, rispettando la size dell'operando
    
    switch(memOperand->getSize())
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

    if(!memOperand->isWriteRequestSent())
    {
        //sending the write request to the cache manager

        //debugLog("MemOperand: Sending write request to address " + to_string_hex(this->address) + " with value " + to_string_hex(v) + " and size " + std::to_string(this->size) + " bytes.");
        memOperand->setWriteRequestSent(true);

        cacheManager.enqueRequest(CacheRequest(RequestType::WRITE, dataTypeSize, memOperand->getAddress(), out, false, core.InstructionId, callbackContext, callback));
        result.success = false;
        result.errorInfo = {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY};
        EventLog::getInstance().pushOperandDataLogEntry(std::move(result), uint64_t{0});
        return {OperandStatus::WAITING_MEMORY, 0};
    }
    else
    {
        //request already sent, waiting for completion

        DEBUG_LOG(debugLog("MemOperand: Write request already sent to address " + to_string_hex(memOperand->getAddress()) + ". Waiting for completion."));

        //checking if the request is completed
        MaxCPUInstructionLength response;
        bool found = false;

        cpu.findCacheResponse(core.InstructionId, response, found);



        if (found)
        {
            //request completed
            DEBUG_LOG(debugLog("MemOperand: Write request completed for address " + to_string_hex(memOperand->getAddress()) + "."));
            
            //extracting the result
            cpu.eraseCacheResponseIfFound(core.InstructionId);
            memOperand->setWriteRequestSent(false); //resetting the flag for future requests

            result.success = true;
            result.errorInfo = {ComponentType::OPERAND, EventType::OPERAND_SET_VALUE, ErrorType::NONE};
            EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);


            return {OperandStatus::OK, 0};
        }
        else
        {
            //request not completed
            DEBUG_LOG(debugLog("MemOperand: Write request not completed for address " + to_string_hex(memOperand->getAddress()) + "."));
            result.success = false;
            result.errorInfo = {ComponentType::OPERAND, EventType::ERROR, ErrorType::WAITING_MEMORY};
            EventLog::getInstance().pushOperandDataLogEntry(std::move(result), value);
            return {OperandStatus::WAITING_MEMORY, 0};
        }

    }
}


#include "controlUnit.hpp"
#include "../include/eventHandler.hpp"
#include "../include/instruction.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include "registerFile.hpp"

CU::CU(Bus& busRef) : bus(busRef)
{
    DEBUG_LOG(debugLog("Control Unit created"));
}

CU::~CU()
{
}

void CU::startFetch(uint64_t instructionId, uint64_t& index, PipelineEventHandler& eventHandler)
{
    // Implementation of instruction fetching using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    DEBUG_LOG(debugLog("Starting instruction fetch..."));

    RegisterFile& cpuRegisters = bus.getCPU().getRegisters(); // get the registers of the CPU temporarily
    // take the value of istruction register
    index = cpuRegisters.getReg(Register::RIP).raw();
    // fetching the instruction from cache or memory

    DEBUG_LOG(debugLog("Fetching instruction at address: " + to_string_hex(index)));

    auto cacheRequest = CacheRequest();
    cacheRequest.type = RequestType::READ;
    cacheRequest.address = index;
    cacheRequest.dataType = TypeofData::ARRAY_16B;
    cacheRequest.requestID = instructionId;
    cacheRequest.callback = eventHandler.getCallback(EventHandlerPipelineEventType::MEMORY_DONE_FETCH);
    cacheRequest.callbackContext = eventHandler.getContext();
    cacheRequest.typeofL1 = instructionMemoryInterface.type; // Set the cache level type for instruction memory interface

    bus.getCPU().getCacheManager().enqueRequest(std::move(cacheRequest), instructionMemoryInterface.type);
    // Trigger an event to notify that a cache request has been sent
    eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_WAITING_FETCH);

    DEBUG_LOG(debugLog("Cache request sent for instruction fetch."));
}

void CU::updateFetch(uint64_t instructionId, PipelineEventHandler& eventHandler)
{
    // Implementation of updating fetch stage using the bus
    // This is a placeholder implementation and should be replaced with actual logic
    DEBUG_LOG(debugLog("Updating fetch stage..."));

    bool found = false;
    MaxCPUInstructionLength buffer;
    bus.getCPU().findCacheResponse(instructionId, buffer, found);

    if (found)
    {
        DEBUG_LOG(debugLog("memory/cache response found for instruction ID: " + std::to_string(instructionId)));
        // Trigger an event to notify that the instruction has been fetched
        eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::MEMORY_DONE_FETCH);
    }
    else
    {
        DEBUG_LOG(debugLog("No cache response found for instruction ID: " + std::to_string(instructionId)));
    }
}

// method for fethcing the instruction from the ram
InstructionInfo CU::fetchInstruction(uint64_t instructionId, uint64_t index, PipelineEventHandler& eventHandler)
{

    DEBUG_LOG(debugLog("Fetching instruction from memory..."));

    // Read a line from the cache buffer_responseQueue
    MaxCPUInstructionLength buffer;
    bool found = false;
    bus.getCPU().findCacheResponse(instructionId, buffer, found);

    bus.getCPU().eraseCacheResponseIfFound(instructionId); // Remove the response from the queue after processing

    DEBUG_LOG(debugLog("Searching for cache response for instruction ID: " + std::to_string(instructionId)));

    for (uint8_t i = 0; i < 16; i++)
    {
        DEBUG_LOG(debugLog("Byte " + std::to_string(i) + ": " + to_string_hex(static_cast<int>(buffer[i]))));
    }

    // extracting the instruction from the buffer

    uint8_t byteCounter{}; // counter of the byte (for IR)

    // fetch the prefix
    uint8_t prefix[4]{};   // prefix of the instruction
    uint8_t numbersOfPrefix{}; // number of prefix
    fetchPrefix(buffer, prefix, numbersOfPrefix, byteCounter);

    // fetch the REX prefix
    bool rex = false;     // rex prefix flag
    uint8_t rexprefix{0}; // rex prefix
    fetchREX(static_cast<uint8_t>(buffer[byteCounter]), rex, rexprefix, byteCounter);

    // fetch the opcode
    uint32_t opcode; // opcode of the instruction
    fetchOpcode(buffer, opcode, byteCounter);

    // decode the opcode and get the length of the instruction
    InstructionInfo info = decoder.lenghtOfInstruction(opcode, prefix, numbersOfPrefix, rex, rexprefix);

    // settinmg the id of the instruction for further tracking for helping in debugging and cache management
    info.instructionId = instructionId;

    // searching for the sib and displacement
    if (info.hasModRM)
    {
        std::byte byteRM = buffer[byteCounter]; // fetch the byte from the buffer
        byteCounter++;                          // increment the byte counter
        r_m rm = decoder.decodeRM(byteRM);

        DEBUG_LOG(debugLog("Mod: " + to_string_hex(static_cast<int>(rm.mod))));
        DEBUG_LOG(debugLog("Reg: " + to_string_hex(static_cast<int>(rm.reg))));
        DEBUG_LOG(debugLog("R/M: " + to_string_hex(static_cast<int>(rm.r_m))));

        searchingSibDisplacement(buffer, info, byteCounter, rm);
    }

    DEBUG_LOG(debugLog("Opcode: " + to_string_hex(info.opcode)));
    DEBUG_LOG(debugLog("Prefix Count: " + std::to_string(info.prefixCount)));
    DEBUG_LOG(debugLog("Total Length: " + std::to_string(info.totalLength)));
    DEBUG_LOG(debugLog("Opcode Length: " + std::to_string(info.opcodeLength)));
    DEBUG_LOG(debugLog("Additional Bytes: " + std::to_string(info.additionalBytes)));
    DEBUG_LOG(debugLog("Description: " + std::string(info.description)));
    DEBUG_LOG(debugLog("Number of Operands: " + std::to_string(info.numOperands)));
    DEBUG_LOG(debugLog("Operand Length: " + std::to_string(info.operandLength)));

    std::memcpy(info.instruction.data(), buffer.data(), info.totalLength); // Copy the instruction bytes into the info structure

    DEBUG_LOG(debugLog("Instruction: "));

    for (size_t i = 0; i < info.totalLength; i++)
    {
        DEBUG_LOG(debugLog("Byte: " + to_string_hex(static_cast<int>(info.instruction[i]))));
    }

    // set the RIP to the next instruction
    bus.getCPU().getRegisters().getReg(Register::RIP) = index + static_cast<uint64_t>(info.totalLength);

    DEBUG_LOG(debugLog("IR: " + to_string_hex(bus.getCPU().getRegisters().getReg(Register::RIP).raw())));

    eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::FETCH_COMPLETE);

    return info;
}

// method for decoding the instruction
void CU::decodeInstruction(InstructionInfo instruction, std::unique_ptr<Instruction>& decodedInstruction, PipelineEventHandler& eventHandler)
{

    decodedInstruction = decoder.decodeInstruction(instruction);
    DEBUG_LOG(debugLog("Decoded instruction type: " + toStringTypeofInstruction(decodedInstruction.get()->getCore().type)));
    eventHandler.triggerPipelineEvent(EventHandlerPipelineEventType::DECODE_COMPLETE);
    DEBUG_LOG(debugLog("Instruction decoded and DECODE_COMPLETE event triggered."));
}

// helpers function for making the code more readable

// function for searching the SIB and displacement
void CU::searchingSibDisplacement(const MaxCPUInstructionLength& buffer, InstructionInfo& info, uint8_t& byteCounter, r_m& rm)
{

    uint8_t byte;

    if (rm.mod == 0b11)
    {
        // nothing to do because is a reg-reg instruction
    }
    else
    {
        // if mod=00,01,10,
        if (rm.r_m == 0b100)
        {
            // if the r/m is 100, there is the SIB byte
            byte = static_cast<uint8_t>(buffer[byteCounter]); // fetch the byte from the buffer
            byteCounter++;                                    // increment the byte counter
            DEBUG_LOG(debugLog("SIB: " + to_string_hex(static_cast<int>(byte))));
            info.hasSIB = true;
            info.totalLength += 1;
            info.additionalBytes += 1;

            if ((byte & 0b111) == 0b101 && rm.mod == 0b00)
            {
                // the base of th SIB indicates a displacement of 32 bit
                byteCounter += 4; // increment the byte counter
                info.totalLength += 4;
                info.additionalBytes += 4;
            }
        }
        if (rm.mod == 0b01)
        {
            byteCounter++; // increment the byte counter
            info.hasDisplacement = true;
            info.totalLength += 1;
            info.additionalBytes += 1;
        }

        if (rm.mod == 0b10)
        {
            byteCounter += 4; // increment the byte counter
            info.totalLength += 4;
            info.additionalBytes += 4;
            info.hasDisplacement = true;
        }

        if (rm.mod == 0b00 and rm.r_m == 0b101)
        {
            byteCounter += 4; // increment the byte counter
            info.totalLength += 4;
            info.additionalBytes += 4;
            info.hasDisplacement = true;
        }
    }
}

// function for fetching the opcode
void CU::fetchOpcode(const MaxCPUInstructionLength& buffer, uint32_t& opcode, uint8_t& byteCounter)
{
    uint8_t byte; // byte fetched from buffer

    byte = static_cast<uint8_t>(buffer[byteCounter]); // fetch the byte from the buffer
    byteCounter++;                                    // increment the byte counter

    // checking if the opcode has two or three bytes
    if (byte == 0x0F)
    {
        // the opcode has two bytes
        opcode = static_cast<uint32_t>(byte);             // set the opcode
        byte = static_cast<uint8_t>(buffer[byteCounter]); // fetch the next byte
        byteCounter++;                                    // increment the byte counter

        if (byte == 0x38 || byte == 0x3A)
        {
            // the opcode has three bytes
            opcode = (opcode << 8) | static_cast<uint32_t>(byte);
            byte = static_cast<uint8_t>(buffer[byteCounter]); // fetch the next byte
            byteCounter++;                                    // increment the byte counter
            opcode = (opcode << 8) | static_cast<uint32_t>(byte);
        }
        else
        {
            // the opcode has two bytes
            opcode = (opcode << 8) | static_cast<uint32_t>(byte);
        }
    }

    else
    {
        // the opcode has one byte
        opcode = static_cast<uint32_t>(byte);
    }

    // checking if the opcode  is part of a group of instructions (the opcode is followed by a ModRM byte)
    if (opcode == 0x80 || opcode == 0x81 || opcode == 0x83)
    {
        // the opcode is part of a group of instructions
        // the real opcode is in the ModRM byte
        // the lenght of the opcode is 1 byte
        DEBUG_LOG(debugLog("Opcode is part of a group of instructions"));

        // fething the ModRM byte
        byte = static_cast<uint8_t>(buffer[byteCounter]); // fetch the byte from the buffer (DONT INCREMENT THE BYTE COUNTER, ONLY READ THE BYTE)

        uint8_t reg = (byte >> 3) & 0b111; // extract the reg field from the ModRM byte

        // setting the real opcode
        opcode = (opcode << 8) | static_cast<uint32_t>(reg);

        DEBUG_LOG(debugLog("Real Opcode: " + to_string_hex(opcode)));

        // the opcode is still 1 byte, this script just modify the "opcode" variable to avoid mistakes in the decoding phase using it has a key for the map
    }
}

// function for fetching the prefix
void CU::fetchPrefix(const MaxCPUInstructionLength& buffer, uint8_t prefix[4], uint8_t& numbersOfPrefix, uint8_t& byteCounter)
{
    uint8_t i = 0; // index for the buffer

    while (numbersOfPrefix < 4)
    {
        if (isPrefix(static_cast<uint8_t>(buffer[i])))
        {
            prefix[numbersOfPrefix] = static_cast<uint8_t>(buffer[i]); // set the prefix
            numbersOfPrefix++;                                         // increment the number of prefix
            i++;                                                       // increment the index of the buffer
        }
        else
        {
            break; // exit the loop if there is no more prefix
        }
    }

    // increment the byte counter
    byteCounter += numbersOfPrefix; // increment the byte counter by the number of prefix
}

void CU::fetchREX(uint8_t byte, bool& rex, uint8_t& rexprefix, uint8_t& byteCounter)
{
    // fetch the REX prefix

    // searcing the rex prefix
    if ((byte & 0xF0) == 0x40)
    {
        rex = true;
        rexprefix = byte;
        byteCounter++; // increment the byte counter
    }
}

/*1. Istruzioni di Movimento di Dati
Istruzione	Opcode	Lunghezza Opcode	Descrizione
MOV r/m8, imm8	0xC6 /0	1 byte	Sposta un valore immediato in un registro/memoria a 8 bit.
MOV r/m16, imm16	0xC7 /0	1 byte	Sposta un valore immediato in un registro/memoria a 16 bit.
MOV r/m32, imm32	0xC7 /0	1 byte	Sposta un valore immediato in un registro/memoria a 32 bit.
MOV r/m64, imm32	0xC7 /0	1 byte	Sposta un valore immediato in un registro/memoria a 64 bit (con estensione del segno).
MOV r8, r/m8	0x8A /r	1 byte	Sposta un valore da un registro/memoria a 8 bit in un registro.
MOV r16, r/m16	0x8B /r	1 byte	Sposta un valore da un registro/memoria a 16 bit in un registro.
MOV r32, r/m32	0x8B /r	1 byte	Sposta un valore da un registro/memoria a 32 bit in un registro.
MOV r64, r/m64	0x8B /r	1 byte	Sposta un valore da un registro/memoria a 64 bit in un registro.
2. Istruzioni Aritmetiche
Istruzione	Opcode	Lunghezza Opcode	Descrizione
ADD r/m8, imm8	0x80 /0	1 byte	Somma un valore immediato a un registro/memoria a 8 bit.
ADD r/m16, imm16	0x81 /0	1 byte	Somma un valore immediato a un registro/memoria a 16 bit.
ADD r/m32, imm32	0x81 /0	1 byte	Somma un valore immediato a un registro/memoria a 32 bit.
ADD r/m64, imm32	0x81 /0	1 byte	Somma un valore immediato a un registro/memoria a 64 bit (con estensione del segno).
SUB r/m8, imm8	0x80 /5	1 byte	Sottrae un valore immediato da un registro/memoria a 8 bit.
SUB r/m16, imm16	0x81 /5	1 byte	Sottrae un valore immediato da un registro/memoria a 16 bit.
SUB r/m32, imm32	0x81 /5	1 byte	Sottrae un valore immediato da un registro/memoria a 32 bit.
SUB r/m64, imm32	0x81 /5	1 byte	Sottrae un valore immediato da un registro/memoria a 64 bit (con estensione del segno).
IMUL r32, r/m32	0x0F 0xAF	2 byte	Moltiplicazione con segno tra registri a 32 bit.
3. Istruzioni Logiche
Istruzione	Opcode	Lunghezza Opcode	Descrizione
AND r/m8, imm8	0x80 /4	1 byte	Esegue un'operazione AND tra un registro/memoria a 8 bit e un valore immediato.
AND r/m16, imm16	0x81 /4	1 byte	Esegue un'operazione AND tra un registro/memoria a 16 bit e un valore immediato.
AND r/m32, imm32	0x81 /4	1 byte	Esegue un'operazione AND tra un registro/memoria a 32 bit e un valore immediato.
OR r/m8, imm8	0x80 /1	1 byte	Esegue un'operazione OR tra un registro/memoria a 8 bit e un valore immediato.
OR r/m16, imm16	0x81 /1	1 byte	Esegue un'operazione OR tra un registro/memoria a 16 bit e un valore immediato.
OR r/m32, imm32	0x81 /1	1 byte	Esegue un'operazione OR tra un registro/memoria a 32 bit e un valore immediato.
XOR r/m8, imm8	0x80 /6	1 byte	Esegue un'operazione XOR tra un registro/memoria a 8 bit e un valore immediato.
XOR r/m16, imm16	0x81 /6	1 byte	Esegue un'operazione XOR tra un registro/memoria a 16 bit e un valore immediato.
XOR r/m32, imm32	0x81 /6	1 byte	Esegue un'operazione XOR tra un registro/memoria a 32 bit e un valore immediato.
4. Istruzioni di Controllo del Flusso
Istruzione	Opcode	Lunghezza Opcode	Descrizione
JMP rel8	0xEB	1 byte	Salto relativo a 8 bit.
JMP rel32	0xE9	1 byte	Salto relativo a 32 bit.
CALL rel32	0xE8	1 byte	Chiamata di funzione relativa a 32 bit.
RET	0xC3	1 byte	Ritorno da una funzione.
CMP r/m8, imm8	0x80 /7	1 byte	Confronta un registro/memoria a 8 bit con un valore immediato.
CMP r/m16, imm16	0x81 /7	1 byte	Confronta un registro/memoria a 16 bit con un valore immediato.
CMP r/m32, imm32	0x81 /7	1 byte	Confronta un registro/memoria a 32 bit con un valore immediato.
5. Istruzioni di Stack
Istruzione	Opcode	Lunghezza Opcode	Descrizione
PUSH r/m16	0xFF /6	1 byte	Inserisce un valore a 16 bit nello stack.
PUSH r/m32	0xFF /6	1 byte	Inserisce un valore a 32 bit nello stack.
PUSH r/m64	0xFF /6	1 byte	Inserisce un valore a 64 bit nello stack.
POP r/m16	0x8F /0	1 byte	Rimuove un valore a 16 bit dallo stack.
POP r/m32	0x8F /0	1 byte	Rimuove un valore a 32 bit dallo stack.
POP r/m64	0x8F /0	1 byte	Rimuove un valore a 64 bit dallo stack.
Prefissi Comuni
I prefissi modificano il comportamento delle istruzioni e possono essere combinati in vari modi. Ecco i principali:

Prefisso	Byte	Descrizione
Operand Size Override	0x66	Cambia la dimensione degli operandi (es. da 32-bit a 16-bit).
Address Size Override	0x67	Cambia la dimensione degli indirizzi (es. da 32-bit a 16-bit).
REP/REPE/REPNE	0xF2, 0xF3	Ripete un'istruzione di stringa (es. MOVSB, STOSB).
LOCK	0xF0	Rende l'istruzione atomica (es. LOCK XCHG).*/

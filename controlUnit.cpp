#include "controlUnit.hpp"
#include <iostream>
#include "bus.hpp"
#include "cpu.hpp"
#include "registerFile.hpp"

CU::CU(Bus& bus, CPU& cpu) : bus(bus), cpu(cpu)
{
    //nothing to do here

}

CU::~CU()
{
}

RegisterFile& CU::getRegisters()
{
    return cpu.getRegisters();
}

//method for fethcing the instruction from the ram
InstructionInfo CU::fetchInstruction()
{
    RegisterFile& cpuRegisters = cpu.getRegisters();  // get the registers of the CPU temporarily
    
    //take the value of istruction register
    uint64_t index = cpuRegisters.getReg(Register::RIP).raw();
    
    //control the index of the instruction register
    if (index + 15 > bus.getMemory().getSize()) {
        throw std::out_of_range("Attempted to fetch instruction beyond memory bounds.");
    }

    std::array<uint8_t, 15> buffer {0}; //buffer for the instruction

    //fetch the instruction from the memory
    memcpy(buffer.data(), &bus.getMemory().getData()[index], 15);


    std::vector<uint8_t> Instructionbytes; //bytes of the instruction()
    int byteCounter {0}; //counter of the byte (for IR)

    //fetch the prefix
    uint8_t prefix[4] {0}; //prefix of the instruction
    int numbersOfPrefix {0}; //number of prefix
    fetchPrefix(buffer, prefix, numbersOfPrefix, Instructionbytes, byteCounter);

    


    //fetch the REX prefix
    bool rex = false; //rex prefix flag
    uint8_t rexprefix{0}; //rex prefix
    fetchREX(buffer[byteCounter], rex, rexprefix, byteCounter, Instructionbytes);

    

    //fetch the opcode
    uint32_t opcode; //opcode of the instruction
    fetchOpcode(buffer, opcode, byteCounter, Instructionbytes);


    //decode the opcode and get the length of the instruction
    InstructionInfo info = decoder.LenghtOfInstruction(opcode, prefix, numbersOfPrefix, rex, rexprefix);


    //searching for the sib and displacement
    if (info.hasModRM)
    {
        uint8_t byteRM = buffer[byteCounter]; //fetch the byte from the buffer
        byteCounter++; //increment the byte counter
        r_m rm = decoder.decodeRM(byteRM);

        std::cout << "Mod: " << std::hex << static_cast<int>(rm.mod) << std::endl;
        std::cout << "Reg: " << std::hex << static_cast<int>(rm.reg) << std::endl;
        std::cout << "R/M: " << std::hex << static_cast<int>(rm.r_m) << std::endl;

        Instructionbytes.push_back(byteRM); //add the byte to the instruction bytes

        searchingSIB_Displacement(buffer, Instructionbytes, info, byteCounter, rm, index);

        

       
    }


    //calculate the number of bytes to be fetched
    int bytesToFetch = static_cast<int>(info.totalLength) - byteCounter;



    //fetch the remaining bytes (the immediate value or the rest of the instruction) from the buffer
    fetchRemainingBytes(buffer, Instructionbytes, byteCounter, bytesToFetch);

    

    std::cout << "Opcode: " << std::hex << info.opcode << std::endl;
    std::cout << "Prefix Count: " << info.prefixCount << std::endl;
    std::cout << "Total Length: " << info.totalLength << std::endl;
    std::cout << "Opcode Length: " << info.opcodeLength << std::endl;
    std::cout << "Additional Bytes: " << info.additionalBytes << std::endl;
    std::cout << "Description: " << info.description << std::endl;
    std::cout << "Number of Operands: " << info.numOperands << std::endl;
    std::cout << "Operand Length: " << info.operandLength << std::endl;

    //load the bytes in the struct of the instruction
    info.instruction = Instructionbytes;

    std::cout << "Instruction: " << std::endl;

    for (int i = 0; i < info.instruction.size(); i++)
    {
        std::cout << "Byte: " << std::hex << static_cast<int>(info.instruction[i]) << std::endl;
    }

    //set the RIP to the next instruction
    cpuRegisters.getReg(Register::RIP) = index + static_cast<uint64_t>(info.totalLength);

    std::cout << "IR: " << std::hex << cpuRegisters.getReg(Register::RIP).raw() << std::endl;

    return info;

}


//method for decoding the instruction
Instruction* CU::decodeInstruction(InstructionInfo instruction)
{


   return decoder.decodeInstruction(instruction, this);
   




}



void CU::OperandFetch(Instruction* instruction)
{   
    instruction->fetchOperands(this, bus.getMemory());
 

}


//method for executing the instruction
void CU::executeInstruction(Instruction* instruction)
{

    std::cout << "executeInstruction" << std::endl;
    instruction->execute(this, bus.getMemory());

    //delete the instruction
    //delete instruction;

}


//helpers function for making the code more readable

void CU::fetchRemainingBytes(std::array<uint8_t, 15>& buffer, std::vector<uint8_t>& bytes, int& byteCounter, int bytesToFetch)
{
    //fetch the remaining bytes (the immediate value or the rest of the instruction) from the buffer
    for (int i = 0; i < bytesToFetch; i++)
    {
        uint8_t byte = buffer[byteCounter]; //fetch the byte from the buffer
        byteCounter++; //increment the byte counter
        //std::cout << "Byte: " << std::hex << static_cast<int>(byte) << std::endl;
        bytes.push_back(byte);
    }

    
}

//function for searching the SIB and displacement
void CU::searchingSIB_Displacement(std::array<uint8_t, 15>& buffer, std::vector<uint8_t>& bytes, InstructionInfo& info, int& byteCounter, r_m& rm, uint64_t index) {
    
    
    uint8_t byte;

    if (rm.mod == 0b11)
    {
        //nothing to do because is a reg-reg instruction
    }
    else
    {
        //if mod=00,01,10,
        if(rm.r_m == 0b100)
        {
                //if the r/m is 100, there is the SIB byte
            byte = buffer[byteCounter]; //fetch the byte from the buffer
            byteCounter++; //increment the byte counter
            bytes.push_back(byte);
            std::cout << "SIB: " << std::hex << static_cast<int>(byte) << std::endl;
            info.hasSIB = true;
            info.totalLength += 1;
            info.additionalBytes += 1;


            if ((byte & 0b111) == 0b101 && rm.mod == 0b00)
            {
                //the base of th SIB indicates a displacement of 32 bit


                for (int i = 0; i < 4; i++)
                {
                    byte = buffer[byteCounter]; //fetch the byte from the buffer
                    byteCounter++; //increment the byte counter
                    std::cout << "Displacement: " << std::hex << static_cast<int>(byte) << std::endl;
                    bytes.push_back(byte);
                    
                }

                info.totalLength += 4;
                info.additionalBytes += 4;
            }

           
        }
        if (rm.mod == 0b01)
        {
            //there is a displacement of 8 bit
            byte = buffer[byteCounter]; //fetch the byte from the buffer
            byteCounter++; //increment the byte counter
            bytes.push_back(byte);
            info.hasDisplacement = true;
            info.totalLength += 1;
            info.additionalBytes += 1;
            std::cout << "Displacement: " << std::hex << static_cast<int>(byte) << std::endl;
        }

        if (rm.mod == 0b10)
        {
            //there is a displacement of 32 bit
            for (int i = 0; i < 4; i++)
            {
                byte = buffer[byteCounter]; //fetch the byte from the buffer
                byteCounter++; //increment the byte counter
                std::cout << "Displacement: " << std::hex << static_cast<int>(byte) << std::endl;
                bytes.push_back(byte);
            }
            info.totalLength += 4;
            info.additionalBytes += 4;
            info.hasDisplacement = true;
        }


        if (rm.mod == 0b00 and rm.r_m == 0b101)
        {
            //there is a displacement of 32 bit
            for (int i = 0; i < 4; i++)
            {
                byte = buffer[byteCounter]; //fetch the byte from the buffer
                byteCounter++; //increment the byte counter
                std::cout << "Displacement: " << std::hex << static_cast<int>(byte) << std::endl;
                bytes.push_back(byte);
                
            }
            info.totalLength += 4;
            info.additionalBytes += 4;
            info.hasDisplacement = true;
        }


    }


}

//function for fetching the opcode
void CU::fetchOpcode(std::array<uint8_t, 15>& buffer, uint32_t& opcode, int& byteCounter, std::vector<uint8_t>& bytes)
{
    uint8_t byte; //byte fetched from buffer

    byte = buffer[byteCounter]; //fetch the byte from the buffer
    byteCounter++; //increment the byte counter

    
    if (byte == 0x0F)
    {
                //the opcode has two bytes
                opcode = static_cast<uint32_t>(byte); //set the opcode
                bytes.push_back(byte);
                byte = buffer[byteCounter]; //fetch the next byte
                byteCounter++; //increment the byte counter

                if (byte == 0x38 || byte == 0x3A)
                {
                    //the opcode has three bytes
                    opcode = (opcode << 8) | static_cast<uint32_t>(byte);
                    bytes.push_back(byte);
                    byte = buffer[byteCounter]; //fetch the next byte
                    byteCounter++; //increment the byte counter
                    opcode = (opcode << 8) | static_cast<uint32_t>(byte);
                    bytes.push_back(byte);
                }
                else
                {
                    //the opcode has two bytes
                    opcode = (opcode << 8) | static_cast<uint32_t>(byte);
                    bytes.push_back(byte);
                }

    }

    else
    {
        //the opcode has one byte
        opcode = static_cast<uint32_t>(byte);
        bytes.push_back(byte);
    }


}

//function for fetching the prefix
void CU::fetchPrefix(std::array<uint8_t, 15>& buffer, uint8_t prefix[4], int& numbersOfPrefix, std::vector<uint8_t>& bytes, int& byteCounter)
{
    int i = 0; //index for the buffer

    while(numbersOfPrefix < 4)
    {
        if(isPrefix(buffer[i]))
        {
            prefix[numbersOfPrefix] = buffer[i]; //set the prefix
            bytes.push_back(buffer[i]); //add the prefix to the bytes vector
            numbersOfPrefix++; //increment the number of prefix
            i++; //increment the index of the buffer
        }
        else
        {
            break; //exit the loop if there is no more prefix
        }
    }

    //increment the byte counter
    byteCounter += numbersOfPrefix; //increment the byte counter by the number of prefix
}

void CU::fetchREX(uint8_t byte, bool& rex, uint8_t& rexprefix, int& byteCounter, std::vector<uint8_t>& bytes)
{
    //fetch the REX prefix
    
    //searcing the rex prefix
    if ((byte & 0xF0) == 0x40) 
    { 
        rex = true;
        bytes.push_back(byte);
        rexprefix = byte;
        byteCounter++; //increment the byte counter
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

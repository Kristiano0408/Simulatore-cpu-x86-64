#include "controlUnit.hpp"
#include <iostream>



CU::CU(Memory* memory): memory(memory), decoder(), /*addressingMode(),*/registers(), alu()
{
    //nothing to do here
    
}

CU::~CU()
{
    delete decoder;
    //delete addressingMode;
}

RegisterFile& CU::getRegisters()
{
    return registers;
}

ALU& CU::getALU()
{
    return alu;
}


InstructionInfo CU::fetchInstruction()
{
    uint64_t index; //index of the instruction
    uint8_t prefix[4]; //prefix of the instruction
    int numbersOfPrefix = 0; //number of prefix
    uint8_t opcode; //opcode of the instruction
    std::vector<uint8_t> bytes; //bytes of the instruction()
    uint8_t byte; //byte fetched from the memory
    int byteCounter = 0; //counter of the byte (for IR)
    bool rex = false; //rex prefix flag
    uint8_t rexprefix; //rex prefix

    //take the value of istruction register
    index = registers.getRIP();

    std::vector<uint8_t> data = memory->getData();

    //stampa il contenuto della memoria
    for (int i = 0; i < data.size(); i++)
    {
        std::cout << "Address: " << i << " Value: " << std::hex << static_cast<int>(data[i]) << std::endl;
    }




    //fetch whit a while loop one byte at time for serching the prefix
    while (numbersOfPrefix < 4)
    {

        byte=memory->readByte(index + static_cast<uint64_t>(byteCounter));   

        //if the byte is a prefix
        if (isPrefix(byte))
        {
            //the byte is a prefix
            prefix[numbersOfPrefix] = byte;
            numbersOfPrefix++;
            byteCounter++;
            bytes.push_back(byte);

        }
        else {break;}

    }


    //searcing the rex prefix
    if ((byte & 0xF0) == 0x40) 
    { 
        rex = true;
        bytes.push_back(byte);
        rexprefix = byte;
        byteCounter++;
    }

    byte = memory->readByte(index + static_cast<int64_t>(byteCounter));



    //fetch the opcode
    if (byte == 0x0F)
    {
                //the opcode has two bytes
                opcode = byte;
                bytes.push_back(byte);
                byteCounter++;
                byte = memory->readByte(index + static_cast<int64_t>(byteCounter));

                if (byte == 0x38 || byte == 0x3A)
                {
                    //the opcode has three bytes
                    opcode = static_cast<uint16_t>((opcode << 8) | byte);
                    bytes.push_back(byte);
                    byteCounter++;
                    byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
                    opcode = static_cast<uint32_t>((opcode << 8) | byte);
                    bytes.push_back(byte);
                    byteCounter++;
                }
                else
                {
                    //the opcode has two bytes
                    opcode = static_cast<uint16_t>((opcode << 8) | byte);
                    bytes.push_back(byte);
                    byteCounter++;
                }

    }
    else
    {
        //the opcode has one byte
        opcode = byte;
        bytes.push_back(byte);
        byteCounter++;
    }


    //decode the opcode
    InstructionInfo info = decoder->LenghtOfInstruction(opcode, prefix, numbersOfPrefix, rex, rexprefix);


    //fixing the total length due to the prefix
    fixTotalLengthPrefix(&info);

    
  

    //searching for the sib and displacement
    if (info.hasModRM)
    {
        byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
        r_m rm = decoder->decodeRM(byte);

        std::cout << "Mod: " << std::hex << static_cast<int>(rm.mod) << std::endl;
        std::cout << "Reg: " << std::hex << static_cast<int>(rm.reg) << std::endl;
        std::cout << "R/M: " << std::hex << static_cast<int>(rm.r_m) << std::endl;

        bytes.push_back(byte);

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
                byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
                bytes.push_back(byte);
                byteCounter++;
                std::cout << "SIB: " << std::hex << static_cast<int>(byte) << std::endl;

                info.hasSIB = true;
                info.totalLength += 1;
                info.additionalBytes += 1;
            }
            if (rm.mod == 0b01)
            {
                //there is a displacement of 8 bit
                byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
                bytes.push_back(byte);
                byteCounter++;
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
                    byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
                    std::cout << "Displacement: " << std::hex << static_cast<int>(byte) << std::endl;
                    bytes.push_back(byte);
                    byteCounter++;
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
                    byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
                    std::cout << "Displacement: " << std::hex << static_cast<int>(byte) << std::endl;
                    bytes.push_back(byte);
                    byteCounter++;
                    
                }
                info.totalLength += 4;
                info.additionalBytes += 4;
                info.hasDisplacement = true;
            }

        }
    }


    int bytesToFetch = info.totalLength - byteCounter;

    //fetch the remaining bytes
    for (int i = 0; i < bytesToFetch; i++)
    {
        byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
        bytes.push_back(byte);
        byteCounter++;
    }


    std::cout << "Opcode: " << std::hex << info.opcode << std::endl;
    std::cout << "Prefix Count: " << info.prefixCount << std::endl;
    std::cout << "Total Length: " << info.totalLength << std::endl;
    std::cout << "Opcode Length: " << info.opcodeLength << std::endl;
    std::cout << "Additional Bytes: " << info.additionalBytes << std::endl;
    std::cout << "Description: " << info.description << std::endl;
    std::cout << "Number of Operands: " << info.numOperands << std::endl;
    std::cout << "Operand Length: " << info.operandLength << std::endl;



    for (int i = 0; i < bytes.size(); i++)
    {
        std::cout << "Byte: " << std::hex << static_cast<int>(bytes[i]) << std::endl;
    }

    info.istruction = bytes;

    registers.setRIP(index + static_cast<int64_t>(info.totalLength));

    std::cout << "IR: " << std::hex << registers.getRIP() << std::endl;


    return info;


    
   
    
}


Instruction* CU::decodeInstruction(InstructionInfo instruction)
{


   return decoder->decodeInstruction(instruction, this);
   




}

void CU::executeInstruction(Instruction* instruction)
{
    std::cout << "executeInstruction" << std::endl;
    instruction->execute(this, memory);

    //delete the instruction
    delete instruction;



}


//helpers function for making the code more readable
void CU::fixTotalLengthPrefix(InstructionInfo* info)
{
    if (info->rex)
    {
        info->totalLength += 1;
    }

    for (int i = 0; i < info->prefixCount; i++)
    {
        if (info->prefix[i] != 0)
        {
            info->totalLength += 1;
        }
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

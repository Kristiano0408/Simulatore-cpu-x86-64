#include "decoder.hpp"
#include "addressingMode.hpp"
#include "registerFile.hpp"
#include "alu.hpp"
#include "controlUnit.hpp"



CU::CU(Memory* memory): memory(memory), decoder(), addressingMode(), registers(), alu()
{
    //nothing to do here
    
}

CU::~CU()
{
    delete decoder;
    delete addressingMode;
}

RegisterFile& CU::getRegisters()
{
    return registers;
}

ALU& CU::getALU()
{
    return alu;
}


int64_t CU::fetchInstruction()
{
    uint64_t index; //index of the instruction
    uint8_t prefix[4]; //prefix of the instruction
    int numbersOfPrefix = 0; //number of prefix
    uint8_t opcode; //opcode of the instruction
    std::vector<uint8_t> bytes; //bytes of the instruction()
    uint8_t byte; //byte fetched from the memory
    int byteCounter = 0; //counter of the byte (for IR)
    bool rex = false; //rex prefix

    //take the value of istruction register
    index = registers.getRegisterValue("IR");

    //fetch whit a while loop one byte at time for serching the opcode
    while (numbersOfPrefix < 4)
    {
        byte=memory->readByte(index + static_cast<int64_t>(byteCounter));

        //if the byte is a prefix
        if ((byte == 0xF0) || (byte == 0xF2 || byte == 0xF3) ||  // Gruppo 1
            (byte == 0x2E || byte == 0x36 || byte == 0x3E ||     // Gruppo 2
            byte == 0x26 || byte == 0x64 || byte == 0x65) ||
            (byte == 0x66) ||  // Gruppo 3
            (byte == 0x67))   // Gruppo 4
        {
            //the byte is a prefix
            prefix[numbersOfPrefix] = byte;
            numbersOfPrefix++;
            byteCounter++;
            bytes.push_back(byte);
        }
        else
        {
            break;
        }
    }

    if (byte == 0x40 || byte == 0x41 || byte == 0x42 || byte == 0x43 || byte == 0x44 || byte == 0x45 || byte == 0x46 || byte == 0x47 || // Gruppo 5
        byte == 0x48 || byte == 0x49 || byte == 0x4A || byte == 0x4B || byte == 0x4C || byte == 0x4D || byte == 0x4E || byte == 0x4F)
    {
        //the byte is a REX prefix
        rex = true;
        bytes.push_back(byte);
        byteCounter++;
        byte = memory->readByte(index + static_cast<int64_t>(byteCounter));
    }



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

#ifndef ADRESSING_MODE_HPP
#define ADRESSING_MODE_HPP

#include <cstdint>
#include <string>

class AdressingMode
{
    public:
        virtual ~AdressingMode() = default;
        virtual uint64_t getsize () = 0;
        virtual std::string toString() = 0;

    

};




#endif // ADRESSING_MODE_HPP


/*Registro Diretto (Register Addressing)

L'operando è un registro, senza accesso alla memoria.
Esempio: MOV RAX, RBX
Simulazione: Non serve calcolare un indirizzo, basta copiare i valori tra registri.
Immediato (Immediate Addressing)

Il valore è incluso direttamente nell'istruzione.
Esempio: MOV RAX, 10
Simulazione: Il valore immediato è copiato direttamente nel registro.
Indirizzamento Diretto (Direct Addressing)

L'operando è un indirizzo di memoria fisso.
Esempio: MOV RAX, [0x1000]
Simulazione: Il valore all’indirizzo 0x1000 viene copiato nel registro.
Indirizzamento Indiretto (Indirect Addressing)

L'operando è un registro che contiene un indirizzo.
Esempio: MOV RAX, [RBX]
Simulazione: L'indirizzo contenuto in RBX viene usato per leggere la memoria.
Base + Displacement (Base Displacement Addressing)

Indirizzamento tramite un registro base + un valore costante.
Esempio: MOV RAX, [RBX + 16]
Simulazione: L'indirizzo è dato da RBX + 16, e si accede alla memoria a quell'indirizzo.
Base + Index + Scale + Displacement (SIB - Scale Index Base)

Complesso, ma molto usato per array e strutture.
Esempio: MOV RAX, [RBX + RCX * 4 + 8]
Simulazione: L'indirizzo è calcolato come RBX + (RCX * 4) + 8.*/
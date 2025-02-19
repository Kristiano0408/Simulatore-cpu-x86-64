//for managing the ALU and arithmetic operations

// alu.hpp
#ifndef ALU_HPP
#define ALU_HPP
#include <cstdint>

class ALU {
    // Implemenattion of the ALU class

    public:
        ALU();
        ~ALU();

        //arithmetic operations
        int64_t add(int64_t a, int64_t b);
        int64_t sub(int64_t a, int64_t b);
        int64_t mul(int64_t a, int64_t b);
        //etc 

};

#endif // ALU_HPP


/*Istruzioni Aritmetiche
ADD

Descrizione: Aggiunge due operandi.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
SUB

Descrizione: Sottrae un operando da un altro.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
MUL

Descrizione: Moltiplica due operandi (solo senza segno).
Registri: RAX (moltiplicato) e RDX (risultato alto).
IMUL

Descrizione: Moltiplica due operandi (supporta segno).
Registri: RAX (moltiplicato) e RDX (risultato alto).
DIV

Descrizione: Divide un operando per un altro (solo senza segno).
Registri: RAX (dividendo), RDX (rimanente).
IDIV

Descrizione: Divide un operando per un altro (supporta segno).
Registri: RAX (dividendo), RDX (rimanente).
INC

Descrizione: Incrementa un operando di 1.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
DEC

Descrizione: Decrementa un operando di 1.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
NEG

Descrizione: Negazione di un operando.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
Istruzioni Logiche
AND

Descrizione: Esegue l'operazione AND bit a bit tra due operandi.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
OR

Descrizione: Esegue l'operazione OR bit a bit tra due operandi.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
XOR

Descrizione: Esegue l'operazione XOR bit a bit tra due operandi.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
NOT

Descrizione: Inverte i bit di un operando.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
Istruzioni di Spostamento e Caricamento
MOV

Descrizione: Copia un valore da un operando a un altro.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
PUSH

Descrizione: Aggiunge un valore allo stack.
Registri: RSP (cambia il puntatore allo stack).
POP

Descrizione: Rimuove un valore dallo stack.
Registri: RSP (cambia il puntatore allo stack).
LEA

Descrizione: Carica l'indirizzo efficace di un operando in un registro.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP.
Istruzioni di Controllo del Flusso
JMP

Descrizione: Salta a un'altra istruzione in modo incondizionato.
Registri: RIP (aggiorna il puntatore dell'istruzione).
JE (Jump if Equal)

Descrizione: Salta se i risultati di un'operazione sono uguali.
Registri: RIP.
JNE (Jump if Not Equal)

Descrizione: Salta se i risultati di un'operazione non sono uguali.
Registri: RIP.
JG (Jump if Greater)

Descrizione: Salta se il primo operando è maggiore del secondo.
Registri: RIP.
JL (Jump if Less)

Descrizione: Salta se il primo operando è minore del secondo.
Registri: RIP.
CALL

Descrizione: Chiama una funzione.
Registri: RIP (aggiorna il puntatore dell'istruzione), RSP (per lo stack).
RET

Descrizione: Restituisce da una funzione.
Registri: RIP (aggiorna il puntatore dell'istruzione), RSP (per lo stack).
Istruzioni di Comparazione
CMP

Descrizione: Confronta due operandi e imposta i flag della CPU.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
TEST

Descrizione: Esegue un'operazione AND tra due operandi e imposta i flag della CPU.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, e registri generali.
Istruzioni di Bit Shifting
SHL (Shift Left)

Descrizione: Sposta i bit a sinistra.
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP.
SHR (Shift Right)

Descrizione: Sposta i bit a destra (senza segno).
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP.
SAL (Arithmetic Shift Left)

Descrizione: Sposta i bit a sinistra (arithmetic).
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP.
SAR (Arithmetic Shift Right)

Descrizione: Sposta i bit a destra (con segno).
Registri: RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP.
Istruzioni di Input/Output
IN

Descrizione: Legge un dato da una porta I/O.
Registri: RAX (dato letto).
OUT

Descrizione: Scrive un dato in una porta I/O.
Registri: RAX (dato scritto).
Istruzioni di Sistema
HLT

Descrizione: Ferma l'esecuzione della CPU.
Registri: Nessun registro specifico.
NOP

Descrizione: Nessuna operazione; usato per il riempimento.
Registri: Nessun registro specifico.

        
};*/
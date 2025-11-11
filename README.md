# Simulatore CPU x86-64

Questo progetto è un **simulatore didattico dell’architettura x86-64**, scritto in **C++**, progettato per rappresentare in modo fedele ma semplificato il comportamento interno del processore.

---

## Caratteristiche principali
- Architettura simulata: **x86-64 Long Mode (solo 64 bit)**
- Nessuna modalità legacy: **non** sono supportati real mode o protected mode
- **Registri alti a 8 bit** (`AH`, `BH`, `CH`, `DH`) **non esistono**
- I registri a 8 bit accessibili sono sempre gli **ultimi 8 bit** dei registri a 64 bit:
  - `RSP` → `SPL`
  - `RBP` → `BPL`
  - `RSI` → `SIL`
  - `RDI` → `DIL`  
  (come se fosse sempre presente il **prefisso REX**)
- I nomi dei registri rimangono invariati (es. `RAX`), ma è possibile accedere a **8 / 16 / 32 / 64 bit** dello stesso registro
- Implementata solo la **ALU (Arithmetic Logic Unit)**
  - Nessuna FPU (Floating Point Unit)
  - Nessuna Vector Unit (SIMD)
- Nessun registro di segmento (`CS`, `SS`, ecc.)
- Nessuna eredità da modalità a 32 bit
- Nessuna virtualizzazione, nemmeno della memoria

-Il file main è "prova", usato per testare la logica della rchitettura, in quanto il main vero sarà in python per GUI
-I file `prova.cpp`, `prova2.cpp`, `cose_da_fare.txt` e `flamegraph` sono usati per test e debugging

---

## Struttura del progetto

| File / Cartella | Descrizione |
|------------------|-------------|
| `adressCalculator.hpp` | Calcola l'indirizzo corretto implementando le varie modalità di indirizzamento (es. base + index, base*scale + index, ecc.) |
| `alu.hpp` | Definisce la ALU e le operazioni supportate |
| `bus.hpp` | Struttura logica per la comunicazione tra le unità principali dell’architettura |
| `cacheManager.hpp` | Gestione del sistema di cache (L1, L2, L3) e dei collegamenti con la RAM |
| `clock.hpp` | Implementazione di un clock base, con contatore incrementato ad ogni fase di ciclo |
| `controlUnit.hpp` | Implementazione della Control Unit (CU), che gestisce il ciclo della CPU |
| `cpu.hpp` | Definisce la CPU e coordina le unità principali |
| `decoder.hpp` | Gestisce la decodifica delle istruzioni e la determinazione della lunghezza in fase di fetch |
| `helpers.hpp` | Raccolta di funzioni riutilizzabili in più moduli |
| `instruction_code_map.hpp` | Fornisce le enum class per i modi di indirizzamento e le istruzioni corrispondenti |
| `instruction.hpp` | gestisce con polimorfismo le varie istruzioni e le loro implementazioni 
| `memory.hpp` | Gestione della memoria simulata |
| `opcode_map.hpp` | fornsice in base agli opcode le info geenrali delle istruzioni(per fetching e decodifica) | 
| `operands.hpp` | gestisce la decodifica e il fetching degli opernadi in modo generico ( utilizza gli enum class di instruction_code_map per generalizzare al di sopra delle singole istruzioni) |
| `prova.cpp` | main file per testare logica dell' architettura |
| `RegisterFile.hpp` | Gestisce trammite polimorfismo e classi smart pointers per registri (per ora solo GPR e FLAGS) |


---

## Concetti implementati
- **Registri generali (GPR):**
  - 16 registri a 64 bit (`RAX`–`R15`)
  - `RIP` (Instruction Pointer)
  - `RFLAGS` con flag principali: `ZF`, `CF`, `OF`, `SF`
  - `RSP`, `RBP`
- **Memoria:**
  - Modello piatto (flat memory model)
  - Indirizzamento a 64 bit
- **Ciclo di esecuzione:**
  - Fetch dell’istruzione da memoria
  - Decodifica (opcode → classe derivata)
  - Esecuzione con aggiornamento di registri e flag

---

## Obiettivi futuri
- Implementazione della **FPU (Floating Point Unit)**
- Implementazione della **Vector Unit (SIMD, registri XMM)**
- Supporto completo alle modalità di indirizzamento avanzate
- Interfaccia grafica in **Python + `pybind11`** per visualizzare registri e memoria in tempo reale

---

## Compilazione ed esecuzione

> 💡 Attualmente supportato solo su **Linux** (su windows il linker da errori)

- compilazione con:  "make (opzionanale -j con numero di threads)" con opzionale "DEBUG=1" per stampa degli eventi 

- esecuzione: ./build/prova

 
## Licenza
© 2025 Cristiano Di Gaspero — Tutti i diritti riservati.  
Il codice è pubblicato solo a fini accademici e dimostrativi.  
Non è consentito l’uso o la modifica senza autorizzazione dell’autore.

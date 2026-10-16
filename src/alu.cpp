#include "alu.hpp"

/*
 * ============================================================================
 * ARITHMETIC FLAGS CALCULATION — x86-64 ISA
 * ============================================================================
 *
 * RIFERIMENTI:
 * - Intel 64 and IA-32 Architectures Software Developer's Manual (SDM),
 *   Volume 1, Section 3.4.3.1 — Status Flags.
 * - Intel SDM, Volume 2 — Specification of each instruction, sections
 *   "Operation" and "Flags Affected".
 * - Intel SDM Specification Style Guide, "Example instruction specification":
 *   https://intel.github.io/SDM/style_guide.html
 *
 * PRINCIPIO GENERALE
 * ------------------
 * L'ISA definisce il significato architetturale dei flag e stabilisce quali
 * istruzioni li aggiornano, li preservano o li lasciano indefiniti.
 *
 * Le formule utilizzate dall'ALU sono una traduzione matematica di queste
 * definizioni. Non tutte le istruzioni x86-64 condividono le stesse regole:
 * le formule aritmetiche sottostanti sono applicabili alle operazioni di
 * addizione e sottrazione, mentre le altre famiglie richiedono regole
 * specifiche dell'istruzione.
 *
 * CONVENZIONI
 * -----------
 * n    = larghezza dell'operando in bit (8, 16, 32 o 64).
 * M    = 2^n - 1, maschera della larghezza dell'operando.
 * S    = 2^(n-1), maschera del bit di segno.
 * a    = operando di destinazione, limitato a n bit.
 * b    = operando sorgente, limitato a n bit.
 * r    = risultato effettivo, troncato a n bit.
 * c    = carry-in per ADC oppure borrow-in per SBB (0 oppure 1).
 *
 * Gli operandi e il risultato devono essere normalizzati alla larghezza
 * dell'operazione prima di calcolare i flag.
 *
 * FORMULE DEI FLAG
 * ----------------
 *
 * ZF — Zero Flag
 * Impostato se il risultato troncato è zero.
 *
 *     ZF = (r == 0)
 *
 * SF — Sign Flag
 * Copia il bit più significativo del risultato, che rappresenta il segno
 * nell'interpretazione signed in complemento a due.
 *
 *     SF = ((r & S) != 0)
 *
 * PF — Parity Flag
 * Impostato se gli 8 bit meno significativi del risultato contengono un
 * numero pari di bit a 1. La parità riguarda sempre il byte inferiore,
 * indipendentemente dalla larghezza dell'operando.
 *
 *     PF = (parity(r & 0xFF) == even)
 *
 * AF — Auxiliary Carry Flag
 * Indica un carry o borrow attraverso il confine tra il bit 3 e il bit 4.
 * Per le operazioni aritmetiche di addizione e sottrazione può essere
 * calcolato mediante la seguente relazione bitwise:
 *
 *     AF = (((a ^ b ^ r) & 0x10) != 0)
 *
 * La costante 0x10 seleziona il bit 4. La formula si applica anche ad ADC
 * e SBB, purché r includa l'effetto del carry-in o borrow-in.
 *
 * CF — Carry Flag
 * Indica un carry oltre la larghezza dell'operando nelle addizioni oppure
 * un borrow nelle sottrazioni. Rileva l'overflow nell'interpretazione
 * unsigned e non va confuso con l'overflow signed segnalato da OF.
 *
 * Addizione (ADD):
 *
 *     CF = (a + b > M)
 *
 * Addizione con carry (ADC):
 *
 *     CF = (a + b + c > M)
 *
 * Sottrazione (SUB, CMP):
 *
 *     CF = (a < b)
 *
 * Sottrazione con borrow (SBB):
 *
 *     CF = (a < b + c)
 *
 * Nell'implementazione C++, queste condizioni possono essere riscritte
 * usando confronti e sottrazioni che evitano di calcolare una somma
 * intermedia oltre la capacità di uint64_t.
 *
 * OF — Overflow Flag
 * Indica che il risultato matematico non è rappresentabile nella larghezza
 * dell'operando interpretato come intero signed in complemento a due.
 *
 * Addizione (ADD, ADC):
 *
 *     OF = ((~(a ^ b) & (a ^ r) & S) != 0)
 *
 * Sottrazione (SUB, SBB, CMP):
 *
 *     OF = (((a ^ b) & (a ^ r) & S) != 0)
 *
 * Le espressioni sono formule booleane equivalenti alla verifica
 * dell'overflow signed, espresse in termini di segni degli operandi e
 * del risultato effettivo.
 *
 * EFFETTI SPECIFICI DELLE ISTRUZIONI
 * ----------------------------------
 * Le formule matematiche non determinano, da sole, quali flag debbano
 * essere scritti: questa decisione dipende dalla semantica dell'istruzione.
 *
 * - INC e DEC aggiornano AF, OF, SF, ZF e PF, ma preservano CF.
 * - NEG equivale aritmeticamente a 0 - operando; CF vale 1 se l'operando
 *   è diverso da zero.
 * - CMP calcola i flag come una sottrazione, senza scrivere il risultato
 *   nell'operando di destinazione.
 * - Le istruzioni logiche, gli shift, le rotazioni, le moltiplicazioni
 *   e le divisioni hanno regole specifiche per i flag.
 *
 * Pertanto, l'ALU separa il calcolo matematico dei flag dalla gestione
 * degli effetti architetturali specifici dell'istruzione.
 * ============================================================================
 */


void ALU::calculateCF_add(temporaryValues& tempValues, uint64_t dest, uint64_t src, uint64_t mask,  bool carryIn) {
    tempValues.CF = (dest > (mask - src)) || (carryIn && dest == (mask - src));
}

void ALU::calculateCF_sub(temporaryValues& tempValues, uint64_t dest, uint64_t src, bool carryIn) {
    tempValues.CF = (dest < src) || (carryIn && dest == src);
}

void ALU::calculateOF_add(temporaryValues& tempValues, uint64_t dest, uint64_t src, uint64_t result, uint64_t signBit) {
    tempValues.OF = ((~(dest^ src)) & (dest ^ result) & signBit) != 0;
}

void ALU::calculateOF_sub(temporaryValues& tempValues, uint64_t dest, uint64_t src, uint64_t result, uint64_t signBit) {
    tempValues.OF = ((dest ^ src) & (dest ^ result) & signBit) != 0;
}


void ALU::calculateAF(temporaryValues& tempValues, uint64_t destMasked, uint64_t srcMasked, uint64_t resultMasked) {
    tempValues.AF = ((destMasked ^ srcMasked ^ resultMasked) & 0x10) != 0;
}

void ALU::calculateZF(temporaryValues& tempValues, uint64_t resultMasked) {
    tempValues.ZF = (resultMasked == 0);
}

void ALU::calculateSF(temporaryValues& tempValues, uint64_t resultMasked, uint64_t signBit) {
    tempValues.SF = (resultMasked & signBit) != 0;
}

void ALU::calculatePF(temporaryValues& tempValues, uint64_t resultMasked) {
    // Parity flag is set if the number of set bits in the result is even
    tempValues.PF = static_cast<bool>(__builtin_parity(resultMasked & 0xFF) == 0);
}




void ALU::executeOperation(temporaryValues& tempValues, TypeofInstruction type, uint8_t nbit)
{
    DEBUG_LOG(debugLog("ALU: Executing operation " + toStringTypeofInstruction(type) + " with source value " + to_string_hex(tempValues.srcValue) + " and destination value " + to_string_hex(tempValues.destValue) + "."));
    uint64_t dest = tempValues.destValue;
    uint64_t src  = tempValues.srcValue;
    uint64_t tmp  = 0;

    const uint64_t widthMask = (nbit == 64) ? ~0ULL : ((1ULL << nbit) - 1);
    const uint64_t signBit   = (1ULL << (nbit - 1));
    const uint64_t c         = tempValues.carryIn ? 1ULL : 0ULL;


    uint64_t destMasked = dest & widthMask;
    uint64_t srcMasked  = src & widthMask;

    // Esegui le operazioni normalmente
    switch (type) {
        case TypeofInstruction::ADD:
            tmp = add(dest, src);
            calculateCF_add(tempValues, dest, src, widthMask, false);
            calculateAF(tempValues, destMasked, srcMasked, tmp & widthMask);
            calculateOF_add(tempValues, dest, src, tmp, signBit);
            break;

        case TypeofInstruction::SUB:
        case TypeofInstruction::CMP:
            // CMP calcola dest - src esattamente come SUB:
            // il risultato viene troncato e usato solo per i flag,
            // la scrittura sull'operando di destinazione e' soppressa nella pipeline
            tmp = sub(dest, src);
            calculateCF_sub(tempValues, dest, src, false);
            calculateAF(tempValues, destMasked, srcMasked, tmp & widthMask);
            calculateOF_sub(tempValues, dest, src, tmp, signBit);
            break;

        case TypeofInstruction::ADC: {
            tmp = adc(dest, src, c);
            calculateCF_add(tempValues, dest, src, widthMask, c);
            calculateAF(tempValues, destMasked, srcMasked, tmp & widthMask);
            calculateOF_add(tempValues, dest, src, tmp, signBit);
            break;
        }

        case TypeofInstruction::SBB: {
            tmp = sbb(dest, src, c);
            calculateCF_sub(tempValues, dest, src, c);
            calculateAF(tempValues, destMasked, srcMasked, tmp & widthMask);
            calculateOF_sub(tempValues, dest, src, tmp, signBit);
            break;
        }

        case TypeofInstruction::INC:
            tmp = add(dest, 1);
            calculateAF(tempValues, destMasked, 1, tmp & widthMask);
            // CF unchanged for INC
            // OF: overflow if positive → negative (max positive to most negative of the width)
            calculateOF_add(tempValues, dest, 1, tmp, signBit);
            break;

        case TypeofInstruction::DEC:
            tmp = sub(dest, 1);
            calculateAF(tempValues, destMasked, 1, tmp & widthMask);
            // CF unchanged for DEC
            // OF: overflow if negative → positive (most negative to max positive of the width)
            calculateOF_sub(tempValues, dest, 1, tmp, signBit);
            break;

        case TypeofInstruction::NEG: {
            // NEG dest = 0 - dest = (~dest) + 1
            tmp = (~dest) + 1ULL;

            // CF: 1 for every operand different from 0 (0 - dest needs a borrow)
            calculateCF_sub(tempValues, 0, dest, false);

            // AF: borrow from bit 4 in 0 - (dest & 0xF)
            calculateAF(tempValues, 0, destMasked, tmp & widthMask);
            calculateOF_sub(tempValues, 0, dest, tmp, signBit);
            break;
        }

        default:
            throw std::runtime_error("ALU: operazione non supportata");
    }

    const uint64_t truncatedResult = tmp & widthMask;

    calculateZF(tempValues, truncatedResult);
    calculateSF(tempValues, truncatedResult, signBit);
    calculatePF(tempValues, truncatedResult);
    tempValues.resultValue = truncatedResult;
}


uint64_t ALU::sub(uint64_t dest, uint64_t src)
{
    return dest - src;
}

uint64_t ALU::adc(uint64_t dest, uint64_t src, uint64_t c)
{
    return dest + src + c;
}

uint64_t ALU::sbb(uint64_t dest, uint64_t src, uint64_t c)
{
    return dest - src - c;
}


uint64_t ALU::add(uint64_t dest, uint64_t src)
{
    return dest + src;
}
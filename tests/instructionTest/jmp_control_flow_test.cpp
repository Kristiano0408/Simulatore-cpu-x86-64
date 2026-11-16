


#include "../instructionTest.hpp"
#include <vector>



std::vector<TestCase> buildJmpControlFlowTests()
{
    std::vector<TestCase> tests;

    //---------------------------------------------------------------------------------
    //TEST 1 : jmp + mov rax 5 (non utilizzata) + mov rbx 3
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "JMP rel8 forward 2: RIP 0x02 -> 0x04 + MOV al, imm8 NON ESEGUITA + MOV bl, imm8",
        .program     = {0xEB, 0x02, 0xB0, 0x05, 0xB3, 0x03},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RAX, 0x0A},
                          {Register::RBX, 0x00}},
        .expected_regs = {{Register::RIP, 0x06},
                          {Register::RAX, 0x0A},
                          {Register::RBX, 0x03}},
    });

    //---------------------------------------------------------------------------------
    //TEST 2 : jmp + mov rax 5 (saltata) + mov rbx 3 + mov rcx 7
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "JMP rel8 forward 2: salta MOV al, imm8 + MOV bl, imm8 + MOV cl, imm8",
        .program     = {0xEB, 0x02, 0xB0, 0x05, 0xB3, 0x03, 0xB1, 0x07},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RAX, 0x0A},
                          {Register::RBX, 0x00},
                          {Register::RCX, 0x00}},
        .expected_regs = {{Register::RIP, 0x08},
                          {Register::RAX, 0x0A},
                          {Register::RBX, 0x03},
                          {Register::RCX, 0x07}},
    });

    //---------------------------------------------------------------------------------
    //TEST 3 : JMP rel8 backward (self loop) all'inizio del programma
    //         EB FE a 0x00 -> target = (0x00 + 2) - 2 = 0x00
    //         Il JMP viene rieseguito finche' restano cicli: RIP finale 0x00,
    //         la MOV a 0x02 non viene mai raggiunta.
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "JMP rel8 backward self-loop (EB FE): RIP resta 0x00, MOV BL non eseguita",
        .program     = {0xEB, 0xFE, 0xB3, 0x03},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RBX, 0x00}},
        .expected_regs = {{Register::RIP, 0x00},
                          {Register::RBX, 0x00}},
    });

    //---------------------------------------------------------------------------------
    //TEST 4 : due JMP di fila: la prima salta la seconda e salta la MOV AL,
    //         e atterra sulla MOV BL.
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "doppio JMP (EB 04 poi EB 02): il secondo JMP viene saltato, RBX scritto",
        .program     = {0xEB, 0x04, 0xEB, 0x02, 0xB0, 0x09, 0xB3, 0x03},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RAX, 0x0A},
                          {Register::RBX, 0x00}},
        .expected_regs = {{Register::RIP, 0x08},
                          {Register::RAX, 0x0A},
                          {Register::RBX, 0x03}},
    });

    //---------------------------------------------------------------------------------
    //TEST 5 : JMP a meta' programma: salta MOV AL e MOV CL, esegue solo MOV BL
    //         EB 04 a 0x00 -> target = 0x06
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "JMP rel8 forward 4: salta MOV al + MOV cl, esegue MOV bl",
        .program     = {0xEB, 0x04, 0xB0, 0x05, 0xB1, 0x07, 0xB3, 0x03},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RAX, 0x0A},
                          {Register::RCX, 0x0A},
                          {Register::RBX, 0x00}},
        .expected_regs = {{Register::RIP, 0x08},
                          {Register::RAX, 0x0A},
                          {Register::RCX, 0x0A},
                          {Register::RBX, 0x03}},
    });


    //---------------------------------------------------------------------------------
    //TEST 6 : JMP r/m64 (FF E0 = JMP RAX) in mezzo al programma: RIP <- RAX,
    //         la MOV CL a 0x04 viene saltata (RAX punta a 0x06)
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "JMP r/m64 reg (FF E0): RIP <- RAX = 0x06, salta MOV cl",
        .program     = {0xB3, 0x03, 0xFF, 0xE0, 0xB1, 0x07, 0xB0, 0x05},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RAX, 0x06},
                          {Register::RBX, 0x00},
                          {Register::RCX, 0x00}},
        .expected_regs = {{Register::RIP, 0x08},
                          {Register::RAX, 0x05},
                          {Register::RBX, 0x03},
                          {Register::RCX, 0x00}},
    });

    //---------------------------------------------------------------------------------
    //TEST 7 : flusso con memoria: MOV [RSI], RAX prima del JMP viene eseguita,
    //         la MOV AL dopo il JMP viene saltata.
    //         EB 02 a 0x08 -> target = (0x08 + 2) + 2 = 0x0c
    //--------------------------------------------------------------------------------
    tests.push_back({
        .description = "JMP forward dopo write memoria: [RSI] scritto, MOV al saltata",
        .program     = {0xB8, 0x2A, 0x00, 0x00, 0x00,
                        0x48, 0x89, 0x06,
                        0xEB, 0x02,
                        0xB0, 0x05},
        .initial_regs =  {{Register::RIP, 0x00},
                          {Register::RAX, 0x0A},
                          {Register::RSI, 0x1000}},
        .expected_regs = {{Register::RIP, 0x0C},
                          {Register::RAX, 0x2A},
                          {Register::RSI, 0x1000}},
        .expected_mem = {{0x1000, 0x2A}},
    });

    return tests;
}
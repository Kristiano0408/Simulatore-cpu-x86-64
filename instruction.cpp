#include "instruction.hpp"
#include <cstdint>
#include "controlUnit.hpp"
#include "bus.hpp"
#include "registerFile.hpp"
#include "cpu.hpp"

Instruction::Instruction()
{
    rm = {0, 0, 0, 0};
    sib = {0, 0, 0, 0};
    hasDisplacement = false;
    hasImmediate = false;
    hasModRM = false;
    hasSIB = false;
    regToReg = false;
    regToMem = false;
    memToReg = false;
    numPrefixes = 0;
    rex = false;
    rexprefix = 0;
    nbit = 0;
    opcode = 0;
    value = 0;
    displacement = 0;
    SIBdisplacement = 0;
    sourceOperand = std::make_unique<EmptyOperand>();
    destinationOperand = std::make_unique<EmptyOperand>();

}

Instruction::~Instruction() {
}

void Instruction::execute([[maybe_unused]] Bus& bus) 
{
    //default implementation (do nothing)
}

void Instruction::setOpcode(uint32_t opcode) 
{
    this->opcode = opcode;
}

uint32_t Instruction::getOpcode() 
{
    return opcode;
}

void Instruction::setPrefix(uint8_t prefix[4]) 
{
    for (int i = 0; i < 4; i++) {
        this->prefix[i] = prefix[i];
    }
}

uint8_t* Instruction::getPrefix() 
{
    return prefix;
}

void Instruction::setNumPrefixes(int numPrefixes) 
{
    this->numPrefixes = numPrefixes;
}

int Instruction::getNumPrefixes() {
    return numPrefixes;
}

void Instruction::setRex(bool rex) {
    this->rex = rex;
}

bool Instruction::getRex() {
    return rex;
}

void Instruction::setRexprefix(int8_t rexprefix) {
    this->rexprefix = rexprefix;
}

int8_t Instruction::getRexprefix() {
    return rexprefix;
}


void Instruction::setNbit(int nbit) {
    this->nbit = nbit;
}

int Instruction::getNbit() {
    return nbit;
}


void Instruction::setHasImmediate(bool hasImmediate) {
    this->hasImmediate = hasImmediate;
}

bool Instruction::getHasImmediate() {
    return hasImmediate;
}


void Instruction::setHasDisplacement(bool hasDisplacement) {
    this->hasDisplacement = hasDisplacement;
}

bool Instruction::getHasDisplacement() {
    return hasDisplacement;
}


void Instruction::setHasModRM(bool hasModRM) {
    this->hasModRM = hasModRM;
}

bool Instruction::getHasModRM() {
    return hasModRM;
}


void Instruction::setHasSIB(bool hasSIB) {
    this->hasSIB = hasSIB;
}

bool Instruction::getHasSIB() {
    return hasSIB;
}


void Instruction::setRM(r_m rm) {
    this->rm = rm;
}

r_m Instruction::getRM() {
    return rm;
}


void Instruction::setSIB(SIB sib) {
    this->sib = sib;
}

SIB Instruction::getSIB() {
    return sib;
}


void Instruction::setValue(uint64_t value) {
    this->value = value;
}

uint64_t Instruction::getValue() {
    return value;
}


void Instruction::setDisplacement(uint64_t displacement) {
    this->displacement = displacement;
}

uint64_t Instruction::getDisplacement() {
    return displacement;
}


void Instruction::setSIBdisplacement(uint32_t SIBdisplacement) {
    this->SIBdisplacement = SIBdisplacement;
}

uint32_t Instruction::getSIBdisplacement() {
    return SIBdisplacement;
}


void Instruction::setRegToReg(bool regToReg) {
    this->regToReg = regToReg;
}

bool Instruction::getRegToReg() {
    return regToReg;
}


void Instruction::setRegToMem(bool regToMem) {
    this->regToMem = regToMem;
}

bool Instruction::getRegToMem() {
    return regToMem;
}


void Instruction::setMemToReg(bool memToReg) {
    this->memToReg = memToReg;
}

bool Instruction::getMemToReg() {
    return memToReg;
}

uint64_t Instruction::castingValue(uint64_t value, int nbit) 
{
    switch (nbit)
    {
        case 8:
            return castTo<uint8_t>(value);
        case 16:
            return castTo<uint16_t>(value);
        case 32:
            return castTo<uint32_t>(value);
        case 64:
            return castTo<uint64_t>(value);
        default:
            std::cerr << "Invalid number of bits" << std::endl;
            return 0; // or throw an exception 

    }
}


//getters and setters for the operands
void Instruction::setSourceOperand(std::unique_ptr<Operand> sourceOperand) {
    this->sourceOperand = std::move(sourceOperand); // move the unique_ptr to the member variable

}

Operand* Instruction::getSourceOperand() {
    return sourceOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setDestinationOperand(std::unique_ptr<Operand> destinationOperand) 
{
    this->destinationOperand = std::move(destinationOperand); // move the unique_ptr to the member variable
}

Operand* Instruction::getDestinationOperand() {
    return destinationOperand.get(); // return the raw pointer of the unique_ptr
}

void Instruction::setAddressingMode(AddressingMode addressingMode) {
    this->addressingMode = addressingMode;
}

//calculate the number of bits of the value/operand
int Instruction::calculating_number_of_bits() 
{
    uint32_t opcode = getOpcode();

    if (getRexprefix() & 0x08)
    {
        return 64;
    }

    for (int i = 0; i < getNumPrefixes(); i++)
    {
        if (getPrefix()[i] == 0x66)
        {
            std::cout << "66 prefix" << std::endl;
            return 16;
        }
    }

    static const std::unordered_set<uint32_t> opcode_8bit = {0xA0, 0xA2, 0xC6, 0x88, 0x8A, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, //mov
                                                            0x04, 0x80,0x83, 0x00, 0x02}; //add

    if(opcode_8bit.count(opcode))
    {
        return 8;
    }
   

    return 32;
}

//get the addressing mode
AddressingMode Instruction::getAddressingMode() {
    return addressingMode;
}

uint64_t Instruction::mask(int nbit) 
{
    switch (nbit)
    {
        case 8:
            return 0xFF;
        case 16:
            return 0xFFFF;
        case 32:
            return 0xFFFFFFFF;
        case 64:
            return 0xFFFFFFFFFFFFFFFF;
        default:
            std::cerr << "Invalid number of bits" << std::endl;
            return 0; // or throw an exception 

    }
}

//Move instruction



//fetch the operands
void MoveInstruction::fetchOperands(Bus& bus) {

    //std::cout << "Fetching operands for Move Instruction" << std::endl;
    //getting the opcode
    uint32_t opcode = getOpcode();


    //fetch the operands



    
    //using switch case to get the operands
    switch (getAddressingMode())
    {
        case AddressingMode::MR:                     //move register to R/M
            std::cout << "MOV_MR" << std::endl;
            operandFetch::fetchMR(this, bus);
            break;
        
        case AddressingMode::RM:                    //move R/M to register
            std::cout << "MOV_RM" << std::endl;
            //std::cout << "opcode: " << opcode << std::endl;
            operandFetch::fetchRM(this, bus);
            break;
        
        
        case AddressingMode::MI:                   //move immediate to memory/register
            std::cout << "MOV_MI" << std::endl;
            operandFetch::fetchMI(this, bus);
            break;
        
        case AddressingMode::OI:                  //move immediate to reg
            std::cout << "MOV_OI" << std::endl;
            operandFetch::fetchOI(this, bus, opcode);
            break;
        
        case AddressingMode::FD:                     //move from offset to Rax
            std::cout << "MOV_FD" << std::endl;
            operandFetch::fetchFD(this, bus);
            break;
        
        case AddressingMode::TD:                    //move from Rax to offset
            std::cout << "MOV_TD" << std::endl;
            operandFetch::fetchTD(this, bus);
            break;
        
        default:
            break;
    }


}

// Move instruction
void MoveInstruction::execute([[maybe_unused]] Bus& bus) 
{

    //setting the size of the operands
    int bit = calculating_number_of_bits();

    setNbit(bit);


    //setting the size of the operands(it might not be necessary but for know we dont have a geeneic function for fethcing 
    //from memory so we have to set the size of the operands to know what to fetch from memory)

    

    getSourceOperand()->setSize(bit);
    getDestinationOperand()->setSize(bit);





    if(getDestinationOperand() && getSourceOperand())
    {
         //getting the value from the source operand
        uint64_t value = getSourceOperand()->getValue().data;

        //casting the value to the number of bits of the operand (8, 16, 32, 64) and zero extending it
        value = castingValue(value, getNbit());

        std::cout << "Value: " << value << std::endl;


        //setting the value to the destination operand
        getDestinationOperand()->setValue(value);

    }
    else
    {
        std::cerr << "Error: Source or destination operand is null" << std::endl;
    }

    //std::cout<< "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ"<< std::endl;

    //delete sourceOperand; // delete the source operand after use
    //delete destinationOperand; // delete the destination operand after use

    //sourceOperand = nullptr; // set the pointer to null after deletion
    //destinationOperand = nullptr; // set the pointer to null after deletion


}


//Add instruction

//fetch the operands
void AddInstruction::fetchOperands(Bus& bus)
{
    //std::cout << "Fetching operands for Add Instruction" << std::endl;
    //fetch the operands
    //using switch case to get the operands
    switch (getAddressingMode())
    {
        case AddressingMode::MR:                     //add register to R/M
            std::cout << "ADD_MR" << std::endl;
            operandFetch::fetchMR(this, bus);
            break;  

        case AddressingMode::RM:                    //add R/M to register
            std::cout << "ADD_RM" << std::endl;
            operandFetch::fetchRM(this, bus);
            break;
        case AddressingMode::MI:                   //add immediate to memory/register
            std::cout << "ADD_MI" << std::endl;
            operandFetch::fetchMI(this, bus);
            break;
        case AddressingMode::I:                  //add immediate to accumulator
            std::cout << "ADD_I" << std::endl;
            operandFetch::fetchI(this, bus);
            break;
        default:
            break;
   }
}


void AddInstruction::execute(Bus& bus) 
{
    //setting the size of the operands
    int bit = calculating_number_of_bits();

    setNbit(bit);

    uint64_t mask = this->mask(bit);

    //setting the size of the operands(it might not be necessary but for know we dont have a geeneic function for fethcing 
    //from memory so we have to set the size of the operands to know what to fetch

    getSourceOperand()->setSize(bit);
    getDestinationOperand()->setSize(bit);

    uint64_t src_value = 0, dst_value = 0, res = 0;

    if(getDestinationOperand() && getSourceOperand())
    {
         //getting the value from the source operand
        src_value = getSourceOperand()->getValue().data & mask;

        if(getOpcode() == 0x83) // it is only encessary to mask the source 
        {
            dst_value = getDestinationOperand()->getValue().data;

            res = bus.getCPU().getALU().add(dst_value, src_value);

             //setting the value to the destination operand
            getDestinationOperand()->setValue(res);
            
        }
        else
        {
            //getting the value from the destination operand
            dst_value = getDestinationOperand()->getValue().data & mask;

            res = bus.getCPU().getALU().add(dst_value, src_value) & mask;


            //setting the value to the destination operand (old destination + result of the addition masked)
            getDestinationOperand()->setValue((getDestinationOperand()->getValue().data & ~mask) | (res & mask));
        }
    }
    else
    {
        std::cerr << "Error: Source or destination operand is null" << std::endl;
    }

}








          


    

    



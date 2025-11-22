"""
Bindings PyBind11 per il simulatore x86-64
"""
from __future__ import annotations
import collections.abc
import typing
__all__: list[str] = ['ADD', 'AF', 'ALU', 'ALU_enum', 'AddressingMode', 'AnyData', 'Bus', 'CACHE', 'CACHE_HIT', 'CACHE_L1', 'CACHE_L2', 'CACHE_L3', 'CACHE_MISS', 'CACHE_READ_ERROR', 'CACHE_WRITE_ERROR', 'CF', 'CPU', 'CacheManager', 'Clock', 'ComponentType', 'DecodeOperandFetchBuffer', 'DecodeStage', 'DummyRegister', 'EMPTY', 'ERROR', 'EmptyInstruction', 'EmptyOperand', 'ErrorType', 'Error_Event_Info', 'EventType', 'ExecuteMemoryBuffer', 'ExecuteStage', 'FD', 'FLUSHED', 'FPU', 'FetchDecodeBuffer', 'FetchStage', 'FlagReg', 'Flagbit', 'I', 'INVALID_ADDRESS', 'INVALID_SIZE', 'ImmediateOperand', 'Instruction', 'InstructionInfo', 'MEMORY_DONE', 'MI', 'MOV', 'MR', 'MemOperand', 'Memory', 'MemoryStage', 'MemoryWriteBackBuffer', 'NONE', 'OF', 'OI', 'OPERAND', 'OUT_OF_BOUNDS', 'Operand', 'OperandFetchExecuteBuffer', 'OperandFetchStage', 'PF', 'Pipeline', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15', 'R8', 'R9', 'RAM', 'RAM_ACCESS', 'RAM_READ_ERROR', 'RAM_WRITE_ERROR', 'RAX', 'RBP', 'RBX', 'RCX', 'RDI', 'RDX', 'READY', 'READ_FAIL', 'RIP', 'RM', 'RSI', 'RSP', 'R_M', 'Reg', 'RegOperand', 'Register', 'RegisterFile', 'ResultAnyData', 'ResultArray15', 'ResultArrayCacheLine', 'ResultUint16', 'ResultUint32', 'ResultUint64', 'ResultUint8', 'ResultVoid', 'SF', 'SIB', 'STALLED', 'SUB', 'Stage', 'TD', 'UNKNOWN', 'WAITING_DEST_OPERAND', 'WAITING_MEMORY', 'WAITING_SRC_OPERAND', 'WRITE_FAIL', 'WriteBackStage', 'ZF', 'stageStatus', 'temporaryValues', 'typeofInstruction']
class ALU:
    pass
class AddressingMode:
    """
    Members:
    
      I
    
      OI
    
      MI
    
      MR
    
      RM
    
      FD
    
      TD
    """
    FD: typing.ClassVar[AddressingMode]  # value = <AddressingMode.FD: 5>
    I: typing.ClassVar[AddressingMode]  # value = <AddressingMode.I: 0>
    MI: typing.ClassVar[AddressingMode]  # value = <AddressingMode.MI: 2>
    MR: typing.ClassVar[AddressingMode]  # value = <AddressingMode.MR: 3>
    OI: typing.ClassVar[AddressingMode]  # value = <AddressingMode.OI: 1>
    RM: typing.ClassVar[AddressingMode]  # value = <AddressingMode.RM: 4>
    TD: typing.ClassVar[AddressingMode]  # value = <AddressingMode.TD: 6>
    __members__: typing.ClassVar[dict[str, AddressingMode]]  # value = {'I': <AddressingMode.I: 0>, 'OI': <AddressingMode.OI: 1>, 'MI': <AddressingMode.MI: 2>, 'MR': <AddressingMode.MR: 3>, 'RM': <AddressingMode.RM: 4>, 'FD': <AddressingMode.FD: 5>, 'TD': <AddressingMode.TD: 6>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class AnyData:
    def __init__(self) -> None:
        ...
    def get(self: None | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"] | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> typing.Any:
        ...
class Bus:
    def __init__(self) -> None:
        ...
    def getCPU(self) -> CPU:
        ...
    def getClock(self) -> Clock:
        ...
    def getMemory(self) -> Memory:
        ...
    def memoryReset(self) -> None:
        ...
    def resetClock(self) -> None:
        ...
    def tick(self) -> None:
        ...
class CPU:
    def cpuReset(self) -> None:
        ...
    def cpuStart(self) -> None:
        ...
    def cpuStep(self) -> None:
        ...
    def getALU(self) -> ALU:
        ...
    def getCacheManager(self) -> CacheManager:
        ...
    def getControlUnit(self) -> CU:
        ...
    def getInstructionIdCounter(self) -> int:
        ...
    def getPipeline(self) -> Pipeline:
        ...
    def getRegisters(self) -> RegisterFile:
        ...
    def incrementInstructionIdCounter(self) -> None:
        ...
class CacheManager:
    def printCacheState(self) -> None:
        ...
class Clock:
    def __init__(self) -> None:
        ...
    def getCycles(self) -> int:
        ...
    def reset(self) -> None:
        ...
    def tick(self) -> None:
        ...
class ComponentType:
    """
    Members:
    
      CACHE
    
      CACHE_L1
    
      CACHE_L2
    
      CACHE_L3
    
      RAM
    
      OPERAND
    
      ALU_enum
    
      FPU
    
      UNKNOWN
    """
    ALU_enum: typing.ClassVar[ComponentType]  # value = <ComponentType.ALU_enum: 6>
    CACHE: typing.ClassVar[ComponentType]  # value = <ComponentType.CACHE: 0>
    CACHE_L1: typing.ClassVar[ComponentType]  # value = <ComponentType.CACHE_L1: 1>
    CACHE_L2: typing.ClassVar[ComponentType]  # value = <ComponentType.CACHE_L2: 2>
    CACHE_L3: typing.ClassVar[ComponentType]  # value = <ComponentType.CACHE_L3: 3>
    FPU: typing.ClassVar[ComponentType]  # value = <ComponentType.FPU: 7>
    OPERAND: typing.ClassVar[ComponentType]  # value = <ComponentType.OPERAND: 5>
    RAM: typing.ClassVar[ComponentType]  # value = <ComponentType.RAM: 4>
    UNKNOWN: typing.ClassVar[ComponentType]  # value = <ComponentType.UNKNOWN: 8>
    __members__: typing.ClassVar[dict[str, ComponentType]]  # value = {'CACHE': <ComponentType.CACHE: 0>, 'CACHE_L1': <ComponentType.CACHE_L1: 1>, 'CACHE_L2': <ComponentType.CACHE_L2: 2>, 'CACHE_L3': <ComponentType.CACHE_L3: 3>, 'RAM': <ComponentType.RAM: 4>, 'OPERAND': <ComponentType.OPERAND: 5>, 'ALU_enum': <ComponentType.ALU_enum: 6>, 'FPU': <ComponentType.FPU: 7>, 'UNKNOWN': <ComponentType.UNKNOWN: 8>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class DecodeOperandFetchBuffer:
    flushed: bool
    stalled: bool
    valid: bool
    def peekInstruction(self) -> Instruction:
        ...
class DecodeStage(Stage):
    def __init__(self) -> None:
        ...
    def getInstructionToDecode(self) -> InstructionInfo:
        ...
    def peekInstruction(self) -> Instruction:
        ...
class EmptyInstruction(Instruction):
    def __init__(self) -> None:
        ...
class EmptyOperand(Operand):
    def __init__(self) -> None:
        ...
class ErrorType:
    """
    Members:
    
      NONE
    
      INVALID_ADDRESS
    
      INVALID_SIZE
    
      OUT_OF_BOUNDS
    
      WRITE_FAIL
    
      READ_FAIL
    
      UNKNOWN
    
      WAITING_MEMORY
    """
    INVALID_ADDRESS: typing.ClassVar[ErrorType]  # value = <ErrorType.INVALID_ADDRESS: 1>
    INVALID_SIZE: typing.ClassVar[ErrorType]  # value = <ErrorType.INVALID_SIZE: 2>
    NONE: typing.ClassVar[ErrorType]  # value = <ErrorType.NONE: 0>
    OUT_OF_BOUNDS: typing.ClassVar[ErrorType]  # value = <ErrorType.OUT_OF_BOUNDS: 3>
    READ_FAIL: typing.ClassVar[ErrorType]  # value = <ErrorType.READ_FAIL: 5>
    UNKNOWN: typing.ClassVar[ErrorType]  # value = <ErrorType.UNKNOWN: 6>
    WAITING_MEMORY: typing.ClassVar[ErrorType]  # value = <ErrorType.WAITING_MEMORY: 7>
    WRITE_FAIL: typing.ClassVar[ErrorType]  # value = <ErrorType.WRITE_FAIL: 4>
    __members__: typing.ClassVar[dict[str, ErrorType]]  # value = {'NONE': <ErrorType.NONE: 0>, 'INVALID_ADDRESS': <ErrorType.INVALID_ADDRESS: 1>, 'INVALID_SIZE': <ErrorType.INVALID_SIZE: 2>, 'OUT_OF_BOUNDS': <ErrorType.OUT_OF_BOUNDS: 3>, 'WRITE_FAIL': <ErrorType.WRITE_FAIL: 4>, 'READ_FAIL': <ErrorType.READ_FAIL: 5>, 'UNKNOWN': <ErrorType.UNKNOWN: 6>, 'WAITING_MEMORY': <ErrorType.WAITING_MEMORY: 7>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class Error_Event_Info:
    error: ErrorType
    event: EventType
    message: str
    source: ComponentType
    def __init__(self) -> None:
        ...
class EventType:
    """
    Members:
    
      NONE
    
      CACHE_HIT
    
      CACHE_MISS
    
      RAM_ACCESS
    
      ERROR
    
      CACHE_READ_ERROR
    
      CACHE_WRITE_ERROR
    
      RAM_READ_ERROR
    
      RAM_WRITE_ERROR
    """
    CACHE_HIT: typing.ClassVar[EventType]  # value = <EventType.CACHE_HIT: 1>
    CACHE_MISS: typing.ClassVar[EventType]  # value = <EventType.CACHE_MISS: 2>
    CACHE_READ_ERROR: typing.ClassVar[EventType]  # value = <EventType.CACHE_READ_ERROR: 5>
    CACHE_WRITE_ERROR: typing.ClassVar[EventType]  # value = <EventType.CACHE_WRITE_ERROR: 6>
    ERROR: typing.ClassVar[EventType]  # value = <EventType.ERROR: 4>
    NONE: typing.ClassVar[EventType]  # value = <EventType.NONE: 0>
    RAM_ACCESS: typing.ClassVar[EventType]  # value = <EventType.RAM_ACCESS: 3>
    RAM_READ_ERROR: typing.ClassVar[EventType]  # value = <EventType.RAM_READ_ERROR: 7>
    RAM_WRITE_ERROR: typing.ClassVar[EventType]  # value = <EventType.RAM_WRITE_ERROR: 8>
    __members__: typing.ClassVar[dict[str, EventType]]  # value = {'NONE': <EventType.NONE: 0>, 'CACHE_HIT': <EventType.CACHE_HIT: 1>, 'CACHE_MISS': <EventType.CACHE_MISS: 2>, 'RAM_ACCESS': <EventType.RAM_ACCESS: 3>, 'ERROR': <EventType.ERROR: 4>, 'CACHE_READ_ERROR': <EventType.CACHE_READ_ERROR: 5>, 'CACHE_WRITE_ERROR': <EventType.CACHE_WRITE_ERROR: 6>, 'RAM_READ_ERROR': <EventType.RAM_READ_ERROR: 7>, 'RAM_WRITE_ERROR': <EventType.RAM_WRITE_ERROR: 8>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class ExecuteMemoryBuffer:
    flushed: bool
    stalled: bool
    valid: bool
    def peekInstruction(self) -> Instruction:
        ...
class ExecuteStage(Stage):
    def __init__(self) -> None:
        ...
    def peekInstruction(self) -> Instruction:
        ...
class FetchDecodeBuffer:
    flushed: bool
    instructionInfo: InstructionInfo
    stalled: bool
    valid: bool
class FetchStage(Stage):
    def __init__(self) -> None:
        ...
    def getCurrentInstructionInfo(self) -> InstructionInfo:
        ...
class FlagReg(Reg):
    def __init__(self) -> None:
        ...
    def getFlag(self, arg0: Flagbit) -> bool:
        ...
    def setFlag(self, arg0: Flagbit, arg1: bool) -> None:
        ...
class Flagbit:
    """
    Members:
    
      CF
    
      PF
    
      AF
    
      ZF
    
      SF
    
      OF
    """
    AF: typing.ClassVar[Flagbit]  # value = <Flagbit.AF: 4>
    CF: typing.ClassVar[Flagbit]  # value = <Flagbit.CF: 0>
    OF: typing.ClassVar[Flagbit]  # value = <Flagbit.OF: 11>
    PF: typing.ClassVar[Flagbit]  # value = <Flagbit.PF: 2>
    SF: typing.ClassVar[Flagbit]  # value = <Flagbit.SF: 7>
    ZF: typing.ClassVar[Flagbit]  # value = <Flagbit.ZF: 6>
    __members__: typing.ClassVar[dict[str, Flagbit]]  # value = {'CF': <Flagbit.CF: 0>, 'PF': <Flagbit.PF: 2>, 'AF': <Flagbit.AF: 4>, 'ZF': <Flagbit.ZF: 6>, 'SF': <Flagbit.SF: 7>, 'OF': <Flagbit.OF: 11>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class ImmediateOperand(Operand):
    def __init__(self, arg0: None | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"] | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> None:
        ...
class Instruction:
    def calculating_number_of_bits(self) -> int:
        ...
    def getAddressingMode(self) -> AddressingMode:
        ...
    def getDestinationOperand(self) -> Operand:
        ...
    def getDisplacement(self) -> int:
        ...
    def getHasDisplacement(self) -> bool:
        ...
    def getHasImmediate(self) -> bool:
        ...
    def getHasModRM(self) -> bool:
        ...
    def getHasSIB(self) -> bool:
        ...
    def getInstructionId(self) -> int:
        ...
    def getMemToReg(self) -> bool:
        ...
    def getNbit(self) -> int:
        ...
    def getNumPrefixes(self) -> int:
        ...
    def getOpcode(self) -> int:
        ...
    def getPrefix(self) -> int:
        ...
    def getRM(self) -> R_M:
        ...
    def getRegToMem(self) -> bool:
        ...
    def getRegToReg(self) -> bool:
        ...
    def getRex(self) -> bool:
        ...
    def getRexprefix(self) -> int:
        ...
    def getSIB(self) -> SIB:
        ...
    def getSIBdisplacement(self) -> int:
        ...
    def getSourceOperand(self) -> Operand:
        ...
    def getTemporaryValues(self) -> temporaryValues:
        ...
    def getValue(self) -> int:
        ...
    def isEmpty(self) -> bool:
        ...
    def isWaitingDestOperand(self) -> bool:
        ...
    def isWaitingSrcOperand(self) -> bool:
        ...
    def mask(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> int:
        ...
class InstructionInfo:
    description: str
    hasDisplacement: bool
    hasImmediate: bool
    hasModRM: bool
    hasSIB: bool
    rex: bool
    rex_w_sensitive: bool
    def __init__(self) -> None:
        ...
    @property
    def additionalBytes(self) -> int:
        ...
    @additionalBytes.setter
    def additionalBytes(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def bit_extension(self) -> int:
        ...
    @bit_extension.setter
    def bit_extension(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def dest_operand_length(self) -> int:
        ...
    @dest_operand_length.setter
    def dest_operand_length(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def instruction(self) -> list[int]:
        ...
    @instruction.setter
    def instruction(self, arg0: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex]) -> None:
        ...
    @property
    def instructionId(self) -> int:
        ...
    @instructionId.setter
    def instructionId(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def numOperands(self) -> int:
        ...
    @numOperands.setter
    def numOperands(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def opcode(self) -> int:
        ...
    @opcode.setter
    def opcode(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def opcodeLength(self) -> int:
        ...
    @opcodeLength.setter
    def opcodeLength(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def operandLength(self) -> int:
        ...
    @operandLength.setter
    def operandLength(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def prefix(self) -> list[int]:
        ...
    @prefix.setter
    def prefix(self, arg1: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex]) -> None:
        ...
    @property
    def prefixCount(self) -> int:
        ...
    @prefixCount.setter
    def prefixCount(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def rexprefix(self) -> int:
        ...
    @rexprefix.setter
    def rexprefix(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def src_operand_length(self) -> int:
        ...
    @src_operand_length.setter
    def src_operand_length(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def totalLength(self) -> int:
        ...
    @totalLength.setter
    def totalLength(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class MemOperand(Operand):
    def __init__(self, arg0: Bus, arg1: typing.SupportsInt | typing.SupportsIndex, arg2: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class Memory:
    def clear(self) -> None:
        ...
    def getBasePointer(self) -> int:
        ...
    def getData(self) -> list[int]:
        ...
    def getSize(self) -> int:
        ...
    def getStackPointer(self) -> int:
        ...
    def pop(self) -> ResultUint64:
        ...
    def push(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> ResultVoid:
        ...
    def setBasePointer(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def setData(self, arg0: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex]) -> None:
        ...
    def setDataPartial(self, arg0: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def setStackPointer(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class MemoryStage(Stage):
    def __init__(self) -> None:
        ...
    def peekInstruction(self) -> Instruction:
        ...
class MemoryWriteBackBuffer:
    flushed: bool
    stalled: bool
    valid: bool
    def peekInstruction(self) -> Instruction:
        ...
class Operand:
    def getSize(self) -> int:
        ...
    def getValue(self) -> ResultAnyData:
        ...
    def setSize(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def setValue(self, arg0: None | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"] | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> ResultVoid:
        ...
class OperandFetchExecuteBuffer:
    flushed: bool
    stalled: bool
    valid: bool
    def peekInstruction(self) -> Instruction:
        ...
class OperandFetchStage(Stage):
    def __init__(self) -> None:
        ...
    def peekInstruction(self) -> Instruction:
        ...
class Pipeline:
    def __init__(self, arg0: Bus) -> None:
        ...
    def execute_operation(self) -> None:
        ...
    def getDecodeOperandFetchBuffer(self) -> DecodeOperandFetchBuffer:
        ...
    def getDecodeStage(self) -> DecodeStage:
        ...
    def getExecuteMemoryBuffer(self) -> ExecuteMemoryBuffer:
        ...
    def getExecuteStage(self) -> ExecuteStage:
        ...
    def getFetchDecodeBuffer(self) -> FetchDecodeBuffer:
        ...
    def getFetchStage(self) -> FetchStage:
        ...
    def getMemoryStage(self) -> MemoryStage:
        ...
    def getMemoryWriteBackBuffer(self) -> MemoryWriteBackBuffer:
        ...
    def getOperandFetchExecuteBuffer(self) -> OperandFetchExecuteBuffer:
        ...
    def getOperandFetchStage(self) -> OperandFetchStage:
        ...
    def getWriteBackStage(self) -> WriteBackStage:
        ...
class R_M:
    def __init__(self) -> None:
        ...
    @property
    def byte_r_m(self) -> int:
        ...
    @byte_r_m.setter
    def byte_r_m(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def mod(self) -> int:
        ...
    @mod.setter
    def mod(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def r_m(self) -> int:
        ...
    @r_m.setter
    def r_m(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def reg(self) -> int:
        ...
    @reg.setter
    def reg(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class Reg:
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def raw(self) -> int:
        ...
    def raw_const(self) -> int:
        ...
class RegOperand(Operand):
    def __init__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class Register:
    """
    Members:
    
      RAX
    
      RBX
    
      RCX
    
      RDX
    
      RSI
    
      RDI
    
      RSP
    
      RBP
    
      R8
    
      R9
    
      R10
    
      R11
    
      R12
    
      R13
    
      R14
    
      R15
    
      RIP
    
      DummyRegister
    """
    DummyRegister: typing.ClassVar[Register]  # value = <Register.DummyRegister: 17>
    R10: typing.ClassVar[Register]  # value = <Register.R10: 10>
    R11: typing.ClassVar[Register]  # value = <Register.R11: 11>
    R12: typing.ClassVar[Register]  # value = <Register.R12: 12>
    R13: typing.ClassVar[Register]  # value = <Register.R13: 13>
    R14: typing.ClassVar[Register]  # value = <Register.R14: 14>
    R15: typing.ClassVar[Register]  # value = <Register.R15: 15>
    R8: typing.ClassVar[Register]  # value = <Register.R8: 8>
    R9: typing.ClassVar[Register]  # value = <Register.R9: 9>
    RAX: typing.ClassVar[Register]  # value = <Register.RAX: 0>
    RBP: typing.ClassVar[Register]  # value = <Register.RBP: 7>
    RBX: typing.ClassVar[Register]  # value = <Register.RBX: 1>
    RCX: typing.ClassVar[Register]  # value = <Register.RCX: 2>
    RDI: typing.ClassVar[Register]  # value = <Register.RDI: 5>
    RDX: typing.ClassVar[Register]  # value = <Register.RDX: 3>
    RIP: typing.ClassVar[Register]  # value = <Register.RIP: 16>
    RSI: typing.ClassVar[Register]  # value = <Register.RSI: 4>
    RSP: typing.ClassVar[Register]  # value = <Register.RSP: 6>
    __members__: typing.ClassVar[dict[str, Register]]  # value = {'RAX': <Register.RAX: 0>, 'RBX': <Register.RBX: 1>, 'RCX': <Register.RCX: 2>, 'RDX': <Register.RDX: 3>, 'RSI': <Register.RSI: 4>, 'RDI': <Register.RDI: 5>, 'RSP': <Register.RSP: 6>, 'RBP': <Register.RBP: 7>, 'R8': <Register.R8: 8>, 'R9': <Register.R9: 9>, 'R10': <Register.R10: 10>, 'R11': <Register.R11: 11>, 'R12': <Register.R12: 12>, 'R13': <Register.R13: 13>, 'R14': <Register.R14: 14>, 'R15': <Register.R15: 15>, 'RIP': <Register.RIP: 16>, 'DummyRegister': <Register.DummyRegister: 17>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class RegisterFile:
    def __init__(self) -> None:
        ...
    def getFlags(self) -> FlagReg:
        ...
    def getReg(self, arg0: Register) -> Reg:
        ...
    def printRegisters(self) -> None:
        ...
    def reset(self) -> None:
        ...
class ResultAnyData:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> typing.Any:
        ...
class ResultArray15:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> typing.Annotated[list[int], "FixedSize(15)"]:
        ...
    @data.setter
    def data(self, arg0: typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> None:
        ...
class ResultArrayCacheLine:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> typing.Annotated[list[int], "FixedSize(64)"]:
        ...
    @data.setter
    def data(self, arg0: typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"]) -> None:
        ...
class ResultUint16:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> int:
        ...
    @data.setter
    def data(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class ResultUint32:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> int:
        ...
    @data.setter
    def data(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class ResultUint64:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> int:
        ...
    @data.setter
    def data(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class ResultUint8:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
    @property
    def data(self) -> int:
        ...
    @data.setter
    def data(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class ResultVoid:
    errorInfo: Error_Event_Info
    success: bool
    def __init__(self) -> None:
        ...
class SIB:
    def __init__(self) -> None:
        ...
    @property
    def base(self) -> int:
        ...
    @base.setter
    def base(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def byte_sib(self) -> int:
        ...
    @byte_sib.setter
    def byte_sib(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def index(self) -> int:
        ...
    @index.setter
    def index(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def scale(self) -> int:
        ...
    @scale.setter
    def scale(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class Stage:
    def getStatus(self) -> stageStatus:
        ...
    def isInstructionEmpty(self, arg0: Instruction) -> bool:
        ...
    def isStageReady(self) -> bool:
        ...
class WriteBackStage(Stage):
    def __init__(self) -> None:
        ...
    def peekInstruction(self) -> Instruction:
        ...
class stageStatus:
    """
    Members:
    
      READY
    
      STALLED
    
      MEMORY_DONE
    
      WAITING_MEMORY
    
      WAITING_SRC_OPERAND
    
      WAITING_DEST_OPERAND
    
      FLUSHED
    
      EMPTY
    """
    EMPTY: typing.ClassVar[stageStatus]  # value = <stageStatus.EMPTY: 7>
    FLUSHED: typing.ClassVar[stageStatus]  # value = <stageStatus.FLUSHED: 6>
    MEMORY_DONE: typing.ClassVar[stageStatus]  # value = <stageStatus.MEMORY_DONE: 1>
    READY: typing.ClassVar[stageStatus]  # value = <stageStatus.READY: 0>
    STALLED: typing.ClassVar[stageStatus]  # value = <stageStatus.STALLED: 2>
    WAITING_DEST_OPERAND: typing.ClassVar[stageStatus]  # value = <stageStatus.WAITING_DEST_OPERAND: 5>
    WAITING_MEMORY: typing.ClassVar[stageStatus]  # value = <stageStatus.WAITING_MEMORY: 3>
    WAITING_SRC_OPERAND: typing.ClassVar[stageStatus]  # value = <stageStatus.WAITING_SRC_OPERAND: 4>
    __members__: typing.ClassVar[dict[str, stageStatus]]  # value = {'READY': <stageStatus.READY: 0>, 'STALLED': <stageStatus.STALLED: 2>, 'MEMORY_DONE': <stageStatus.MEMORY_DONE: 1>, 'WAITING_MEMORY': <stageStatus.WAITING_MEMORY: 3>, 'WAITING_SRC_OPERAND': <stageStatus.WAITING_SRC_OPERAND: 4>, 'WAITING_DEST_OPERAND': <stageStatus.WAITING_DEST_OPERAND: 5>, 'FLUSHED': <stageStatus.FLUSHED: 6>, 'EMPTY': <stageStatus.EMPTY: 7>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class temporaryValues:
    AF: bool
    CF: bool
    OF: bool
    PF: bool
    SF: bool
    ZF: bool
    def __init__(self) -> None:
        ...
    @property
    def destValue(self) -> None | int | int | int | int | typing.Annotated[list[int], "FixedSize(64)"] | typing.Annotated[list[int], "FixedSize(15)"]:
        ...
    @destValue.setter
    def destValue(self, arg0: None | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"] | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> None:
        ...
    @property
    def resultValue(self) -> None | int | int | int | int | typing.Annotated[list[int], "FixedSize(64)"] | typing.Annotated[list[int], "FixedSize(15)"]:
        ...
    @resultValue.setter
    def resultValue(self, arg0: None | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"] | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> None:
        ...
    @property
    def srcValue(self) -> None | int | int | int | int | typing.Annotated[list[int], "FixedSize(64)"] | typing.Annotated[list[int], "FixedSize(15)"]:
        ...
    @srcValue.setter
    def srcValue(self, arg0: None | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(64)"] | typing.Annotated[collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], "FixedSize(15)"]) -> None:
        ...
class typeofInstruction:
    """
    Members:
    
      ADD
    
      MOV
    
      SUB
    """
    ADD: typing.ClassVar[typeofInstruction]  # value = <typeofInstruction.ADD: 1>
    MOV: typing.ClassVar[typeofInstruction]  # value = <typeofInstruction.MOV: 0>
    SUB: typing.ClassVar[typeofInstruction]  # value = <typeofInstruction.SUB: 2>
    __members__: typing.ClassVar[dict[str, typeofInstruction]]  # value = {'ADD': <typeofInstruction.ADD: 1>, 'MOV': <typeofInstruction.MOV: 0>, 'SUB': <typeofInstruction.SUB: 2>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
ADD: typeofInstruction  # value = <typeofInstruction.ADD: 1>
AF: Flagbit  # value = <Flagbit.AF: 4>
ALU_enum: ComponentType  # value = <ComponentType.ALU_enum: 6>
CACHE: ComponentType  # value = <ComponentType.CACHE: 0>
CACHE_HIT: EventType  # value = <EventType.CACHE_HIT: 1>
CACHE_L1: ComponentType  # value = <ComponentType.CACHE_L1: 1>
CACHE_L2: ComponentType  # value = <ComponentType.CACHE_L2: 2>
CACHE_L3: ComponentType  # value = <ComponentType.CACHE_L3: 3>
CACHE_MISS: EventType  # value = <EventType.CACHE_MISS: 2>
CACHE_READ_ERROR: EventType  # value = <EventType.CACHE_READ_ERROR: 5>
CACHE_WRITE_ERROR: EventType  # value = <EventType.CACHE_WRITE_ERROR: 6>
CF: Flagbit  # value = <Flagbit.CF: 0>
DummyRegister: Register  # value = <Register.DummyRegister: 17>
EMPTY: stageStatus  # value = <stageStatus.EMPTY: 7>
ERROR: EventType  # value = <EventType.ERROR: 4>
FD: AddressingMode  # value = <AddressingMode.FD: 5>
FLUSHED: stageStatus  # value = <stageStatus.FLUSHED: 6>
FPU: ComponentType  # value = <ComponentType.FPU: 7>
I: AddressingMode  # value = <AddressingMode.I: 0>
INVALID_ADDRESS: ErrorType  # value = <ErrorType.INVALID_ADDRESS: 1>
INVALID_SIZE: ErrorType  # value = <ErrorType.INVALID_SIZE: 2>
MEMORY_DONE: stageStatus  # value = <stageStatus.MEMORY_DONE: 1>
MI: AddressingMode  # value = <AddressingMode.MI: 2>
MOV: typeofInstruction  # value = <typeofInstruction.MOV: 0>
MR: AddressingMode  # value = <AddressingMode.MR: 3>
NONE: EventType  # value = <EventType.NONE: 0>
OF: Flagbit  # value = <Flagbit.OF: 11>
OI: AddressingMode  # value = <AddressingMode.OI: 1>
OPERAND: ComponentType  # value = <ComponentType.OPERAND: 5>
OUT_OF_BOUNDS: ErrorType  # value = <ErrorType.OUT_OF_BOUNDS: 3>
PF: Flagbit  # value = <Flagbit.PF: 2>
R10: Register  # value = <Register.R10: 10>
R11: Register  # value = <Register.R11: 11>
R12: Register  # value = <Register.R12: 12>
R13: Register  # value = <Register.R13: 13>
R14: Register  # value = <Register.R14: 14>
R15: Register  # value = <Register.R15: 15>
R8: Register  # value = <Register.R8: 8>
R9: Register  # value = <Register.R9: 9>
RAM: ComponentType  # value = <ComponentType.RAM: 4>
RAM_ACCESS: EventType  # value = <EventType.RAM_ACCESS: 3>
RAM_READ_ERROR: EventType  # value = <EventType.RAM_READ_ERROR: 7>
RAM_WRITE_ERROR: EventType  # value = <EventType.RAM_WRITE_ERROR: 8>
RAX: Register  # value = <Register.RAX: 0>
RBP: Register  # value = <Register.RBP: 7>
RBX: Register  # value = <Register.RBX: 1>
RCX: Register  # value = <Register.RCX: 2>
RDI: Register  # value = <Register.RDI: 5>
RDX: Register  # value = <Register.RDX: 3>
READY: stageStatus  # value = <stageStatus.READY: 0>
READ_FAIL: ErrorType  # value = <ErrorType.READ_FAIL: 5>
RIP: Register  # value = <Register.RIP: 16>
RM: AddressingMode  # value = <AddressingMode.RM: 4>
RSI: Register  # value = <Register.RSI: 4>
RSP: Register  # value = <Register.RSP: 6>
SF: Flagbit  # value = <Flagbit.SF: 7>
STALLED: stageStatus  # value = <stageStatus.STALLED: 2>
SUB: typeofInstruction  # value = <typeofInstruction.SUB: 2>
TD: AddressingMode  # value = <AddressingMode.TD: 6>
UNKNOWN: ErrorType  # value = <ErrorType.UNKNOWN: 6>
WAITING_DEST_OPERAND: stageStatus  # value = <stageStatus.WAITING_DEST_OPERAND: 5>
WAITING_MEMORY: ErrorType  # value = <ErrorType.WAITING_MEMORY: 7>
WAITING_SRC_OPERAND: stageStatus  # value = <stageStatus.WAITING_SRC_OPERAND: 4>
WRITE_FAIL: ErrorType  # value = <ErrorType.WRITE_FAIL: 4>
ZF: Flagbit  # value = <Flagbit.ZF: 6>

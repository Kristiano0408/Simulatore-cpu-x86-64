"""
Bindings PyBind11 per il simulatore x86-64
"""
from __future__ import annotations
import collections.abc
import typing
__all__: list[str] = ['AF', 'ALU', 'Bus', 'CF', 'CPU', 'CacheManager', 'Clock', 'DummyRegister', 'FlagReg', 'Flagbit', 'Memory', 'OF', 'PF', 'Pipeline', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15', 'R8', 'R9', 'RAX', 'RBP', 'RBX', 'RCX', 'RDI', 'RDX', 'RIP', 'RSI', 'RSP', 'Reg', 'Register', 'RegisterFile', 'SF', 'ZF']
class ALU:
    pass
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
    def pop(self) -> ...:
        ...
    def push(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> ...:
        ...
    def setBasePointer(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def setData(self, arg0: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex]) -> None:
        ...
    def setDataPartial(self, arg0: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex], arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def setStackPointer(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class Pipeline:
    pass
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
AF: Flagbit  # value = <Flagbit.AF: 4>
CF: Flagbit  # value = <Flagbit.CF: 0>
DummyRegister: Register  # value = <Register.DummyRegister: 17>
OF: Flagbit  # value = <Flagbit.OF: 11>
PF: Flagbit  # value = <Flagbit.PF: 2>
R10: Register  # value = <Register.R10: 10>
R11: Register  # value = <Register.R11: 11>
R12: Register  # value = <Register.R12: 12>
R13: Register  # value = <Register.R13: 13>
R14: Register  # value = <Register.R14: 14>
R15: Register  # value = <Register.R15: 15>
R8: Register  # value = <Register.R8: 8>
R9: Register  # value = <Register.R9: 9>
RAX: Register  # value = <Register.RAX: 0>
RBP: Register  # value = <Register.RBP: 7>
RBX: Register  # value = <Register.RBX: 1>
RCX: Register  # value = <Register.RCX: 2>
RDI: Register  # value = <Register.RDI: 5>
RDX: Register  # value = <Register.RDX: 3>
RIP: Register  # value = <Register.RIP: 16>
RSI: Register  # value = <Register.RSI: 4>
RSP: Register  # value = <Register.RSP: 6>
SF: Flagbit  # value = <Flagbit.SF: 7>
ZF: Flagbit  # value = <Flagbit.ZF: 6>

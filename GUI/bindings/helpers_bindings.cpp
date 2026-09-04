#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include "../include/helpers/pipelineTypes.hpp"
#include "../include/helpers/instructionTypes.hpp"


namespace py = pybind11;

void bind_helpers(py::module &m)
{
    //def_property is needed to bind bitfields correctly cause pybind11 does not support them directly
    // ----- RM -----
    py::class_<r_m>(m, "R_M")
        .def(py::init<>())

        .def_property(
            "r_m",
            [](const r_m &self) { return self.r_m; },
            [](r_m &self, uint8_t v) { self.r_m = v & 0b111; }  // 3 bit
        )

        .def_property(
            "mod",
            [](const r_m &self) { return self.mod; },
            [](r_m &self, uint8_t v) { self.mod = v & 0b11; }   // 2 bit
        )

        .def_property(
            "reg",
            [](const r_m &self) { return self.reg; },
            [](r_m &self, uint8_t v) { self.reg = v & 0b111; }  // 3 bit
        )

        .def_property_readonly("byte_r_m", [](const r_m &self) { return py::bytes(reinterpret_cast<const char*>(&self.byte_r_m), sizeof(self.byte_r_m)); });


    // ----- SIB -----
    py::class_<SIB>(m, "SIB")
        .def(py::init<>())

        .def_property(
            "base",
            [](const SIB &self) { return self.base; },
            [](SIB &self, uint8_t v) { self.base = v & 0b111; } // 3 bit
        )

        .def_property(
            "index",
            [](const SIB &self) { return self.index; },
            [](SIB &self, uint8_t v) { self.index = v & 0b111; } // 3 bit
        )

        .def_property(
            "scale",
            [](const SIB &self) { return self.scale; },
            [](SIB &self, uint8_t v) { self.scale = v & 0b11; } // 2 bit
        )

        .def_property_readonly("byte_sib", [](const SIB &self) { return py::bytes(reinterpret_cast<const char*>(&self.byte_sib), sizeof(self.byte_sib)); });

    //instructionInfo
    py::class_<InstructionInfo>(m, "InstructionInfo")
        .def(py::init<>())
        .def_readwrite("instructionId", &InstructionInfo::instructionId)
        .def_readwrite("totalLength", &InstructionInfo::totalLength)
        .def_readwrite("opcodeLength", &InstructionInfo::opcodeLength)
        .def_readwrite("prefixCount", &InstructionInfo::prefixCount)
        
        // array prefix -> py::list/vector
        .def_readonly("prefix", &InstructionInfo::prefix)

        .def_readwrite("rex", &InstructionInfo::rex)
        .def_readwrite("rexprefix", &InstructionInfo::rexprefix)
        .def_readwrite("opcode", &InstructionInfo::opcode)
        .def_readwrite("additionalBytes", &InstructionInfo::additionalBytes)
        .def_readwrite("numOperands", &InstructionInfo::numOperands)
        .def_readwrite("operandLength", &InstructionInfo::operandLength)
        .def_readwrite("src_operand_length", &InstructionInfo::src_operand_length)
        .def_readwrite("dest_operand_length", &InstructionInfo::dest_operand_length)
        .def_readwrite("bit_extension", &InstructionInfo::bit_extension)
        .def_readwrite("rex_w_sensitive", &InstructionInfo::rex_w_sensitive)
        .def_readwrite("hasModRM", &InstructionInfo::hasModRM)
        .def_readwrite("hasSIB", &InstructionInfo::hasSIB)
        .def_readwrite("hasDisplacement", &InstructionInfo::hasDisplacement)
        .def_readwrite("hasImmediate", &InstructionInfo::hasImmediate)
        .def_property_readonly("instruction", [](const InstructionInfo& info) {
            return py::bytearray(reinterpret_cast<const char*>(info.instruction.data()), info.instruction.size());
        })
        .def_readwrite("description", &InstructionInfo::description);   


    // typeofInstruction enum
    py::enum_<TypeofInstruction>(m, "typeofInstruction")
        .value("ADD", TypeofInstruction::ADD)
        .value("MOV", TypeofInstruction::MOV)
        .value("SUB", TypeofInstruction::SUB)
        .export_values();

    // AddressingMode enum
    py::enum_<AddressingMode>(m, "AddressingMode")
        .value("I", AddressingMode::I)
        .value("OI", AddressingMode::OI)
        .value("MI", AddressingMode::MI)
        .value("MR", AddressingMode::MR)
        .value("RM", AddressingMode::RM)
        .value("FD", AddressingMode::FD)
        .value("TD", AddressingMode::TD)
        .export_values();
    
    // stageStatus enum
    py::enum_<StageStatus>(m, "stageStatus")
        .value("READY", StageStatus::READY)
        .value("STALLED", StageStatus::STALLED)
        .value("MEMORY_DONE", StageStatus::MEMORY_DONE)
        .value("WAITING_MEMORY", StageStatus::WAITING_MEMORY)
        .value("WAITING_SRC_OPERAND", StageStatus::WAITING_SRC_OPERAND)
        .value("WAITING_DEST_OPERAND", StageStatus::WAITING_DEST_OPERAND)
        .value("FLUSHED", StageStatus::FLUSHED)
        .value("EMPTY", StageStatus::EMPTY)
        .export_values();

    //temporaryvalues class
    py::class_<temporaryValues>(m, "temporaryValues")
        .def(py::init<>())
        .def_readwrite("srcValue", &temporaryValues::srcValue)
        .def_readwrite("destValue", &temporaryValues::destValue)
        .def_readwrite("resultValue", &temporaryValues::resultValue)
        .def_readwrite("CF", &temporaryValues::CF)
        .def_readwrite("ZF", &temporaryValues::ZF)
        .def_readwrite("SF", &temporaryValues::SF)
        .def_readwrite("OF", &temporaryValues::OF)
        .def_readwrite("PF", &temporaryValues::PF)
        .def_readwrite("AF", &temporaryValues::AF);


}

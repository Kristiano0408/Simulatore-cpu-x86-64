#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/instruction.hpp"
#include "helpers/instructionTypes.hpp"

namespace py = pybind11;

void bind_instruction(py::module &m) {

    py::class_<InstructionCore>(m, "InstructionCore")
        .def_readonly("InstructionId", &InstructionCore::InstructionId)
        .def_readonly("value", &InstructionCore::value);

    py::enum_<InstructionDataTransferMode>(m, "InstructionDataTransferMode")
        .value("REGISTER_TO_REGISTER", InstructionDataTransferMode::REGISTER_TO_REGISTER)
        .value("REGISTER_TO_MEMORY", InstructionDataTransferMode::REGISTER_TO_MEMORY)
        .value("MEMORY_TO_REGISTER", InstructionDataTransferMode::MEMORY_TO_REGISTER)
        .value("IMMEDIATE_TO_REGISTER", InstructionDataTransferMode::IMMEDIATE_TO_REGISTER)
        .value("IMMEDIATE_TO_ACCUMULATOR", InstructionDataTransferMode::IMMEDIATE_TO_ACCUMULATOR)
        .value("IMMEDIATE_TO_MEMORY", InstructionDataTransferMode::IMMEDIATE_TO_MEMORY)
        .value("MEMORY_TO_MEMORY", InstructionDataTransferMode::MEMORY_TO_MEMORY)
        .export_values();
    
    py::class_<InstructionFlags>(m, "InstructionFlags")
        .def_property_readonly("isRex", [](const InstructionFlags& flags) { return flags.rex; })
        .def_property_readonly("hasImmediate", [](const InstructionFlags& flags) { return flags.hasImmediate; })
        .def_property_readonly("hasDisplacement", [](const InstructionFlags& flags) { return flags.hasDisplacement; })
        .def_property_readonly("hasModRM", [](const InstructionFlags& flags) { return flags.hasModRM; })
        .def_property_readonly("hasSIB", [](const InstructionFlags& flags) { return flags.hasSIB; })
        .def_property_readonly("regToReg", [](const InstructionFlags& flags) { return flags.regToReg; })
        .def_property_readonly("regToMem", [](const InstructionFlags& flags) { return flags.regToMem; })
        .def_property_readonly("memToReg", [](const InstructionFlags& flags) { return flags.memToReg; })
        .def_property_readonly("isWaitingForSrcValue", [](const InstructionFlags& flags) { return flags.waitingSrcOperand; })
        .def_property_readonly("isWaitingForDestValue", [](const InstructionFlags& flags) { return flags.waitingDestOperand; })
        .def_readonly("dataTransferMode", &InstructionFlags::dataTransferMode);


    py::class_<Instruction>(m, "Instruction")
        .def("calculating_number_of_bits", &Instruction::calculatingNumberOfBits)
        .def("getTemporaryValues", &Instruction::getTemporaryValues)
        .def("getCore", &Instruction::getCore, py::return_value_policy::reference_internal)
        .def("getFlags", &Instruction::getFlags, py::return_value_policy::reference_internal)
        .def("getSourceOperand", &Instruction::getSourceOperand, py::return_value_policy::reference_internal)
        .def("getDestinationOperand", &Instruction::getDestinationOperand, py::return_value_policy::reference_internal);


}

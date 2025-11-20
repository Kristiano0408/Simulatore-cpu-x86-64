#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/instruction.hpp"

namespace py = pybind11;

void bind_instruction(py::module &m) {

    py::class_<Instruction>(m, "Instruction")
        .def("isEmpty", &Instruction::isEmpty)
        .def("getOpcode", &Instruction::getOpcode)
        .def("getPrefix", &Instruction::getPrefix)
        .def("getNumPrefixes", &Instruction::getNumPrefixes)
        .def("getRex", &Instruction::getRex)
        .def("getRexprefix", &Instruction::getRexprefix)
        .def("getNbit", &Instruction::getNbit)
        .def("getHasImmediate", &Instruction::getHasImmediate)
        .def("getHasDisplacement", &Instruction::getHasDisplacement)
        .def("getHasModRM", &Instruction::getHasModRM)
        .def("getHasSIB", &Instruction::getHasSIB)
        .def("getRM", &Instruction::getRM)
        .def("getSIB", &Instruction::getSIB)
        .def("getValue", &Instruction::getValue)
        .def("getDisplacement", &Instruction::getDisplacement)
        .def("getSIBdisplacement", &Instruction::getSIBdisplacement)
        .def("getRegToReg", &Instruction::getRegToReg)
        .def("getRegToMem", &Instruction::getRegToMem)
        .def("getMemToReg", &Instruction::getMemToReg)
        .def("getSourceOperand", &Instruction::getSourceOperand, py::return_value_policy::reference)
        .def("getDestinationOperand", &Instruction::getDestinationOperand, py::return_value_policy::reference)
        .def("getAddressingMode", &Instruction::getAddressingMode)
        .def("calculating_number_of_bits", &Instruction::calculating_number_of_bits)
        .def("mask", &Instruction::mask)
        .def("getInstructionId", &Instruction::getInstructionId)
        .def("isWaitingSrcOperand", &Instruction::isWaitingSrcOperand)
        .def("isWaitingDestOperand", &Instruction::isWaitingDestOperand)
        .def("getTemporaryValues", &Instruction::getTemporaryValues);

    py::class_<EmptyInstruction, Instruction>(m, "EmptyInstruction")
        .def(py::init<>());
}

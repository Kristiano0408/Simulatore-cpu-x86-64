#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/instruction.hpp"

namespace py = pybind11;

void bind_instruction(py::module &m) {

    py::class_<InstructionCore>(m, "InstructionCore")
        .def_readonly("InstructionId", &InstructionCore::InstructionId)
        .def_readonly("value", &InstructionCore::value);


    py::class_<Instruction>(m, "Instruction")
        .def("calculating_number_of_bits", &Instruction::calculating_number_of_bits)
        .def("mask", &Instruction::mask)
        .def("getTemporaryValues", &Instruction::getTemporaryValues)
        .def("getCore", &Instruction::getCore, py::return_value_policy::reference_internal)
        .def("getFlags", &Instruction::getFlags, py::return_value_policy::reference_internal)
        .def("getSourceOperand", &Instruction::getSourceOperand, py::return_value_policy::reference_internal)
        .def("getDestinationOperand", &Instruction::getDestinationOperand, py::return_value_policy::reference_internal);


}

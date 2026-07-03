#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/operands.hpp"
#include "../include/bus.hpp"

namespace py = pybind11;

void bind_operands(py::module &m) {

    py::class_<Operand>(m, "Operand")
        .def("getSize", &Operand::getSize)
        .def("setSize", &Operand::setSize)
        .def("gettype", &Operand::getType)
        .def("setType", &Operand::setType);

    py::class_<EmptyOperand, Operand>(m, "EmptyOperand")
        .def(py::init<>());

    py::class_<RegOperand, Operand>(m, "RegOperand")
        .def(py::init<uint64_t&>());

    py::class_<MemOperand, Operand>(m, "MemOperand")
        .def(py::init< int64_t>());

    py::class_<ImmediateOperand, Operand>(m, "ImmediateOperand")
        .def(py::init<uint64_t>());
}

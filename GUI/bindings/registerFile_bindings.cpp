#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../include/registerFile.hpp"

namespace py = pybind11;

// -----------------------------
// Binding dei registri
// -----------------------------
void bind_registerFile(py::module &m) {
    //
    // Enum Register
    //
    py::enum_<Register>(m, "Register")
        .value("RAX", Register::RAX)
        .value("RBX", Register::RBX)
        .value("RCX", Register::RCX)
        .value("RDX", Register::RDX)
        .value("RSI", Register::RSI)
        .value("RDI", Register::RDI)
        .value("RSP", Register::RSP)
        .value("RBP", Register::RBP)
        .value("R8",  Register::R8)
        .value("R9",  Register::R9)
        .value("R10", Register::R10)
        .value("R11", Register::R11)
        .value("R12", Register::R12)
        .value("R13", Register::R13)
        .value("R14", Register::R14)
        .value("R15", Register::R15)
        .value("RIP", Register::RIP)
        .value("DummyRegister", Register::DummyRegister)
        .export_values();

    //
    // Enum Flagbit
    //
    py::enum_<Flagbit>(m, "Flagbit")
        .value("CF", Flagbit::CF)
        .value("PF", Flagbit::PF)
        .value("AF", Flagbit::AF)
        .value("ZF", Flagbit::ZF)
        .value("SF", Flagbit::SF)
        .value("OF", Flagbit::OF)
        .export_values();


    //
    // Reg class
    //
    py::class_<Reg>(m, "Reg")
        .def(py::init<>())
        .def(py::init<uint64_t>())
        
        .def("raw", 
             static_cast<uint64_t&(Reg::*)()>(&Reg::raw),
             py::return_value_policy::reference_internal)

        .def("raw_const",
             static_cast<const uint64_t&(Reg::*)() const>(&Reg::raw),
             py::return_value_policy::reference_internal)

        .def("__int__", [](const Reg &r){ return r.raw(); })
        .def("__repr__", [](const Reg &r){
            return "<Reg value=" + std::to_string(r.raw()) + ">";
        });


    //
    // FlagReg class (inherits from Reg)
    //
    py::class_<FlagReg, Reg>(m, "FlagReg")
        .def(py::init<>())
        .def("getFlag", &FlagReg::getFlag)
        .def("setFlag", &FlagReg::setFlag);


    //
    // RegisterFile class
    //
    py::class_<RegisterFile>(m, "RegisterFile")
        .def(py::init<>())

        .def("getReg", 
             &RegisterFile::getReg,
             py::return_value_policy::reference_internal)

        .def("getFlags",
             &RegisterFile::getFlags,
             py::return_value_policy::reference_internal)

        .def("reset", &RegisterFile::reset)
        .def("printRegisters", &RegisterFile::printRegisters);
}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstdint>

#include "../include/cpu.hpp"
#include "../include/cacheManager.hpp"
#include "../include/pipeline.hpp"
#include "../include/alu.hpp"
#include "../include/registerFile.hpp"
#include "../include/controlUnit.hpp"

namespace py = pybind11;


void bind_cpu(py::module &m) {
    
    // 
    // ------------------------------
    // CPU
    // ------------------------------
    py::class_<CPU>(m, "CPU")
        .def("cpuReset", &CPU::cpuReset)
        .def("cpuStep", &CPU::cpuStep)
        .def("cpuStart", &CPU::cpuStart)
        .def("getInstructionIdCounter", &CPU::getInstructionIdCounter)
        .def("incrementInstructionIdCounter", &CPU::incrementInstructionIdCounter)
        .def("getALU", &CPU::getALU, py::return_value_policy::reference)
        .def("getRegisters", &CPU::getRegisters, py::return_value_policy::reference)
        .def("getControlUnit", &CPU::getControlUnit, py::return_value_policy::reference)
        .def("getCacheManager", &CPU::getCacheManager, py::return_value_policy::reference_internal)
        .def("getPipeline", &CPU::getPipeline, py::return_value_policy::reference);
}
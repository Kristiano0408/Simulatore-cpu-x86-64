#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstdint>

#include "../include/bus.hpp"
#include "../include/cpu.hpp"
#include "../include/clock.hpp"
#include "../include/memory.hpp"
#include "../include/cacheManager.hpp"
#include "../include/device.hpp"
#include "../include/pipeline.hpp"
#include "../include/alu.hpp"
#include "../include/registerFile.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulator, m) {
    m.doc() = "Bindings PyBind11 per il simulatore x86-64";

    // ------------------------------
    // ALU
    // ------------------------------
    py::class_<ALU>(m, "ALU");

    // ------------------------------
    // REGISTER FILE
    // ------------------------------
    py::class_<Reg>(m, "Reg")
        .def(py::init<>())                    // default constructor
        .def(py::init<uint64_t>())            // constructor with uint64_t argument
        .def("__int__", [](const Reg &r){ return static_cast<uint64_t>(r); })  // int conversion
        .def("__index__", [](const Reg &r){ return static_cast<uint64_t>(r); }) // for indexing
        .def("__repr__", [](const Reg &r){ return "<Reg " + std::to_string(static_cast<uint64_t>(r)) + ">"; })
        .def("raw", (uint64_t& (Reg::*)()) &Reg::raw, py::return_value_policy::reference)
        .def("raw_const", (const uint64_t& (Reg::*)() const) &Reg::raw, py::return_value_policy::copy)
        .def("__assign__", [](Reg &r, uint64_t val){ r = val; return r; }); // assignment operator

    py::class_<RegisterFile>(m, "RegisterFile")
        .def("getReg", &RegisterFile::getReg, py::return_value_policy::reference);

    // ------------------------------
    // PIPELINE
    // ------------------------------
    py::class_<Pipeline>(m, "Pipeline");

    // ------------------------------
    // CACHE MANAGER
    // ------------------------------
    py::class_<CacheManager>(m, "CacheManager");
    //.def(py::init<>());  

    // ------------------------------
    // CLOCK
    // ------------------------------
    py::class_<Clock>(m, "Clock")
        .def(py::init<>())
        .def("tick", &Clock::tick)
        .def("getCycles", &Clock::getCycles)
        .def("reset", &Clock::reset);

    // ------------------------------
    // MEMORY (solo metodi concreti)
    // ------------------------------
    py::class_<Memory>(m, "Memory")
        .def("getSize", &Memory::getSize)
        .def("getData", &Memory::getData)
        .def("setData", &Memory::setData)
        .def("clear", &Memory::clear)
        .def("getStackPointer", &Memory::getStackPointer)
        .def("setStackPointer", &Memory::setStackPointer)
        .def("getBasePointer", &Memory::getBasePointer)
        .def("setBasePointer", &Memory::setBasePointer)
        .def("push", &Memory::push)
        .def("pop", &Memory::pop);
       
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

    // ------------------------------
    // BUS
    // ------------------------------
    py::class_<Bus>(m, "Bus")
        .def(py::init<>())
        .def("tick", &Bus::tick)
        .def("resetClock", &Bus::resetClock)
        .def("memoryReset", &Bus::memoryReset)
        .def("getCPU", &Bus::getCPU, py::return_value_policy::reference)
        .def("getMemory", &Bus::getMemory, py::return_value_policy::reference)
        .def("getClock", &Bus::getClock, py::return_value_policy::reference);
}

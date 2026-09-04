#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstdint>

#include "../include/bus.hpp"
#include "../include/cpu.hpp"
#include "../include/clock.hpp"
#include "../include/memory.hpp"
#include "../include/cache/cacheManager.hpp"
#include "../include/device.hpp"
#include "../include/pipeline.hpp"
#include "../include/alu.hpp"
#include "../include/registerFile.hpp"
#include "../include/eventLog.hpp"


namespace py = pybind11;

//forword declarations if needed
void bind_cpu(py::module &m);
void bind_registerFile(py::module &m);
void bind_cacheManager(py::module &m);
void bind_pipeline(py::module &m);
void bind_instruction(py::module &m);
void bind_operands(py::module &m);
void bind_helpers(py::module &m);
void bind_result_info(py::module &m);
void bind_eventLog(py::module &m);



PYBIND11_MODULE(simulator, m) {
    m.doc() = "Bindings PyBind11 per il simulatore x86-64";


    //call binding functions
    bind_helpers(m);
    bind_result_info(m);
    bind_cpu(m);
    bind_registerFile(m);
    bind_cacheManager(m);
    bind_pipeline(m);
    bind_instruction(m);
    bind_operands(m);
    bind_eventLog(m);
    

    // ------------------------------
    // ALU
    // ------------------------------
    py::class_<ALU>(m, "ALU");

     py::class_<CU>(m, "CU");

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
        .def("getData", &Memory::getData, py::return_value_policy::reference_internal)
        .def("setData", &Memory::setData)
        .def("setDataPartial", &Memory::setDataPartial)
        .def("clear", &Memory::clear)
        .def("getStackPointer", &Memory::getStackPointer)
        .def("setStackPointer", &Memory::setStackPointer)
        .def("getBasePointer", &Memory::getBasePointer)
        .def("setBasePointer", &Memory::setBasePointer)
        .def("push", &Memory::push)
        .def("pop", &Memory::pop)
        .def("readByte", &Memory::readTest);
       
    

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

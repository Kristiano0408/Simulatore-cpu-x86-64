// GUI/bindings/result_bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <variant>
#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include "../include/helpers.hpp" 

namespace py = pybind11;

// -------------------------------------------------------------------------
// Template bind_result e specializzazioni
// -------------------------------------------------------------------------

void bind_result(py::module &m) {
    py::class_<Result>(m, "Result")
        .def(py::init<>())
        .def_readwrite("success", &Result::success)
        .def_readwrite("errorInfo", &Result::errorInfo);
}


// Funzione che crea i binding (usa gli helper definiti sopra)
// -------------------------------------------------------------------------
void bind_result_info(py::module &m) {
    // Enums
    py::enum_<ComponentType>(m, "ComponentType")
        .value("CACHE", ComponentType::CACHE)
        .value("CACHE_L1", ComponentType::CACHE_L1)
        .value("CACHE_L2", ComponentType::CACHE_L2)
        .value("CACHE_L3", ComponentType::CACHE_L3)
        .value("RAM", ComponentType::RAM)
        .value("OPERAND", ComponentType::OPERAND)
        .value("ALU_enum", ComponentType::ALU)
        .value("FPU", ComponentType::FPU)
        .value("UNKNOWN", ComponentType::UNKNOWN)
        .export_values();

    py::enum_<ErrorType>(m, "ErrorType")
        .value("NONE", ErrorType::NONE)
        .value("INVALID_ADDRESS", ErrorType::INVALID_ADDRESS)
        .value("INVALID_SIZE", ErrorType::INVALID_SIZE)
        .value("OUT_OF_BOUNDS", ErrorType::OUT_OF_BOUNDS)
        .value("WRITE_FAIL", ErrorType::WRITE_FAIL)
        .value("READ_FAIL", ErrorType::READ_FAIL)
        .value("UNKNOWN", ErrorType::UNKNOWN)
        .value("WAITING_MEMORY", ErrorType::WAITING_MEMORY)
        .export_values();

    py::enum_<EventType>(m, "EventType")
        .value("NONE", EventType::NONE)
        .value("CACHE_HIT", EventType::CACHE_HIT)
        .value("CACHE_MISS", EventType::CACHE_MISS)
        .value("RAM_ACCESS", EventType::RAM_ACCESS)
        .value("ERROR", EventType::ERROR)
        .value("CACHE_READ_ERROR", EventType::CACHE_READ_ERROR)
        .value("CACHE_WRITE_ERROR", EventType::CACHE_WRITE_ERROR)
        .value("RAM_READ_ERROR", EventType::RAM_READ_ERROR)
        .value("RAM_WRITE_ERROR", EventType::RAM_WRITE_ERROR)
        .export_values();

    // Error_Event_Info
    py::class_<Error_Event_Info>(m, "Error_Event_Info")
        .def(py::init<>())
        .def_readwrite("source", &Error_Event_Info::source)
        .def_readwrite("event", &Error_Event_Info::event)
        .def_readwrite("error", &Error_Event_Info::error);

    bind_result(m);
    // fine
}

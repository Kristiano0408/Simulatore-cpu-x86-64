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

namespace { // LOCAL NAMESPACE FOR HELPERS TO CONVERT ANYDATA <-> PYTHON TYPES

// Convert anydata -> py::object
inline py::object anydata_to_py(const anydata &v) {
    return std::visit([](auto&& val) -> py::object {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return py::none();
        } else if constexpr (std::is_integral_v<T>) {
            return py::int_(val);
        } else if constexpr (std::is_array_v<T>) {
            // converte array in list of uint8_t
            return py::cast(std::vector<uint8_t>(std::begin(val), std::end(val)));
        } else {
            return py::none();
        }
    }, v);
}

// Convert py::object -> anydata 

//NOT IMPLEMENTED cause GUI doesn't need to create anydata from Python, it can only read them 


}

// -------------------------------------------------------------------------
// Template bind_result e specializzazioni
// -------------------------------------------------------------------------
template<typename T>
void bind_result(py::module &m, const char* name) {
    py::class_<Result<T>>(m, name)
        .def(py::init<>())
        .def_readwrite("data", &Result<T>::data)
        .def_readwrite("success", &Result<T>::success)
        .def_readwrite("errorInfo", &Result<T>::errorInfo);
}

// specialization for void
template<>
void bind_result<void>(py::module &m, const char* name) {
    py::class_<Result<void>>(m, name)
        .def(py::init<>())
        .def_readwrite("success", &Result<void>::success)
        .def_readwrite("errorInfo", &Result<void>::errorInfo);
}

// specialization for anydata: usa gli helper per convertire in/out Python
template<>
void bind_result<anydata>(py::module &m, const char* name) {
    py::class_<Result<anydata>>(m, name)
        .def(py::init<>())
        .def_property_readonly(
            "data",
            [](const Result<anydata>& self) -> py::object {
                return anydata_to_py(self.data);
            }
        )
        .def_readwrite("success", &Result<anydata>::success)
        .def_readwrite("errorInfo", &Result<anydata>::errorInfo);
}

// -------------------------------------------------------------------------
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
        .def_readwrite("error", &Error_Event_Info::error)
        .def_readwrite("message", &Error_Event_Info::message);

    // ---- bind anydata (class wrapper in Python) ----
    py::class_<anydata>(m, "AnyData")
        .def(py::init<>())
        .def("get", [](const anydata &v) { return anydata_to_py(v); });

    // ---- ora bind Result<T> per i tipi che ti servono ----
    bind_result<anydata>(m, "ResultAnyData");                       // uses helper
    bind_result<void>(m, "ResultVoid");
    bind_result<uint8_t>(m, "ResultUint8");
    bind_result<uint16_t>(m, "ResultUint16");
    bind_result<uint32_t>(m, "ResultUint32");
    bind_result<uint64_t>(m, "ResultUint64");
    bind_result<std::array<uint8_t, 15>>(m, "ResultArray15");
    bind_result<std::array<uint8_t, CACHE_LINE_SIZE>>(m, "ResultArrayCacheLine");

    // fine
}

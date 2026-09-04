#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstdint>


#include "../include/eventLog.hpp"


namespace py = pybind11;



void bind_eventLog(py::module &m) {

    py::class_<LogEntry>(m, "LogEntry")
        .def_readonly("result", &LogEntry::result)
        .def_readonly("timestamp", &LogEntry::timestamp);
    
    py::class_<AddressInfo>(m, "AddressInfo")
        .def_readonly("address", &AddressInfo::address)
        .def_readonly("setIndex", &AddressInfo::setIndex)
        .def_readonly("tag", &AddressInfo::tag)
        .def_readonly("offset", &AddressInfo::offset)
        .def(py::init<uint64_t, uint32_t, uint64_t, uint64_t>());

    py::class_<CacheDataLogEntry>(m, "CacheDataLogEntry")
        .def_readonly("logEntry", &CacheDataLogEntry::logEntry)
        /*
        .def_property_readonly("line1", [](const CacheDataLogEntry& entry) {
            return py::memoryview::from_memory(entry.line1->data(), entry.line1->size());
        })*/
        .def_property_readonly("line1", [](const CacheDataLogEntry& entry) {
            if (entry.line1) 
            {
                return py::bytearray(reinterpret_cast<const char*>(entry.line1->data()), entry.line1->size());
            } 
            return py::bytearray(); // Return an empty bytearray if line1 is nullptr
        })
        .def_property_readonly("line2", [](const CacheDataLogEntry& entry) {
            if (entry.line2) 
            {
                return py::bytearray(reinterpret_cast<const char*>(entry.line2->data()), entry.line2->size());
            } 
            return py::bytearray(); // Return an empty bytearray if line2 is nullptr
        })
        .def_readonly("addressInfo1", &CacheDataLogEntry::addressInfo1)
        .def_readonly("addressInfo2", &CacheDataLogEntry::addressInfo2);

    py::class_<MemoryDataLogEntry>(m, "MemoryDataLogEntry")
        .def_readonly("logEntry", &MemoryDataLogEntry::logEntry)
        .def_property_readonly("lineData", [](const MemoryDataLogEntry& entry) {
            if (entry.lineData) 
            {
                return py::bytearray(reinterpret_cast<const char*>(entry.lineData->data()), entry.lineData->size());
            } 
            return py::bytearray(); // Return an empty bytearray if lineData is nullptr
        })
        .def_readonly("addressLine", &MemoryDataLogEntry::addressLine);

    py::class_<OperandDataLogEntry>(m, "OperandDataLogEntry")
        .def_readonly("logEntry", &OperandDataLogEntry::logEntry)
        .def_readonly("operandValue", &OperandDataLogEntry::operandValue);

    py::class_<EventLog>(m, "EventLog")
        .def_static("getInstance", &EventLog::getInstance, py::return_value_policy::reference)
        .def("getCacheDataLogSize", &EventLog::getCacheDataLogSize)
        .def("getCacheDataLogEntry", &EventLog::getCacheDataLogEntry)
        .def("getMemoryDataLogSize", &EventLog::getMemoryDataLogSize)
        .def("getMemoryDataLogEntry", &EventLog::getMemoryDataLogEntry)
        .def("getOperandDataLogSize", &EventLog::getOperandDataLogSize)
        .def("getOperandDataLogEntry", &EventLog::getOperandDataLogEntry)
        .def("getGeneralLogSize", &EventLog::getGeneralLogSize)
        .def("getGeneralLogEntry", &EventLog::getGeneralLogEntry)
        .def("isLogEmpty", &EventLog::isLogEmpty); // Bind the new method
}


   
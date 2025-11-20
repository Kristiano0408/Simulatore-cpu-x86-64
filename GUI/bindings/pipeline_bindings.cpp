#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/bus.hpp"
#include "../include/pipeline.hpp"

namespace py = pybind11;


void bind_pipeline(py::module &m) {

    // ------------------- STAGES -------------------
    py::class_<Stage>(m, "Stage")
        .def("isStageReady", &Stage::isStageReady)
        .def("getStatus", &Stage::getStatus);

    py::class_<FetchStage, Stage>(m, "FetchStage")
        .def(py::init<>())
        .def("getCurrentInstructionInfo", &FetchStage::getCurrentInstructionInfo);

    py::class_<DecodeStage, Stage>(m, "DecodeStage")
        .def(py::init<>())
        .def("getInstructionToDecode", &DecodeStage::getInstructionToDecode)
        .def("peekInstruction", &DecodeStage::peekInstruction);

    py::class_<OperandFetchStage, Stage>(m, "OperandFetchStage")
        .def(py::init<>())
        .def("peekInstruction", &OperandFetchStage::peekInstruction);

    py::class_<ExecuteStage, Stage>(m, "ExecuteStage")
        .def(py::init<>())
        .def("peekInstruction", &ExecuteStage::peekInstruction);

    py::class_<MemoryStage, Stage>(m, "MemoryStage")
        .def(py::init<>())
        .def("peekInstruction", &MemoryStage::peekInstruction);

    py::class_<WriteBackStage, Stage>(m, "WriteBackStage")
        .def(py::init<>())
        .def("peekInstruction", &WriteBackStage::peekInstruction);

    // ------------------- BUFFERS -------------------
    py::class_<FetchDecodeBuffer>(m, "FetchDecodeBuffer")
        .def_readwrite("valid", &FetchDecodeBuffer::valid)
        .def_readwrite("stalled", &FetchDecodeBuffer::stalled)
        .def_readwrite("flushed", &FetchDecodeBuffer::flushed)
        .def_readwrite("instructionInfo", &FetchDecodeBuffer::instructionInfo);

    py::class_<DecodeOperandFetchBuffer>(m, "DecodeOperandFetchBuffer")
        .def_readwrite("valid", &DecodeOperandFetchBuffer::valid)
        .def_readwrite("stalled", &DecodeOperandFetchBuffer::stalled)
        .def_readwrite("flushed", &DecodeOperandFetchBuffer::flushed)
        .def_property_readonly("peekInstruction", &DecodeOperandFetchBuffer::peekInstruction);

    py::class_<OperandFetchExecuteBuffer>(m, "OperandFetchExecuteBuffer")
        .def_readwrite("valid", &OperandFetchExecuteBuffer::valid)
        .def_readwrite("stalled", &OperandFetchExecuteBuffer::stalled)
        .def_readwrite("flushed", &OperandFetchExecuteBuffer::flushed)
        .def_property_readonly("peekInstruction", &OperandFetchExecuteBuffer::peekInstruction);

    py::class_<ExecuteMemoryBuffer>(m, "ExecuteMemoryBuffer")
        .def_readwrite("valid", &ExecuteMemoryBuffer::valid)
        .def_readwrite("stalled", &ExecuteMemoryBuffer::stalled)
        .def_readwrite("flushed", &ExecuteMemoryBuffer::flushed)
        .def_property_readonly("peekInstruction", &ExecuteMemoryBuffer::peekInstruction);

    py::class_<MemoryWriteBackBuffer>(m, "MemoryWriteBackBuffer")
        .def_readwrite("valid", &MemoryWriteBackBuffer::valid)
        .def_readwrite("stalled", &MemoryWriteBackBuffer::stalled)
        .def_readwrite("flushed", &MemoryWriteBackBuffer::flushed)
        .def_property_readonly("peekInstruction", &MemoryWriteBackBuffer::peekInstruction);

    // ------------------- PIPELINE -------------------
    py::class_<Pipeline>(m, "Pipeline")
        .def(py::init<Bus&>())
        .def("execute_operation", &Pipeline::execute_operation)
        .def("getFetchStage", &Pipeline::getFetchStage, py::return_value_policy::reference_internal)
        .def("getDecodeStage", &Pipeline::getDecodeStage, py::return_value_policy::reference_internal)
        .def("getOperandFetchStage", &Pipeline::getOperandFetchStage, py::return_value_policy::reference_internal)
        .def("getExecuteStage", &Pipeline::getExecuteStage, py::return_value_policy::reference_internal)
        .def("getMemoryStage", &Pipeline::getMemoryStage, py::return_value_policy::reference_internal)
        .def("getWriteBackStage", &Pipeline::getWriteBackStage, py::return_value_policy::reference_internal)
        .def("getFetchDecodeBuffer", &Pipeline::getFetchDecodeBuffer, py::return_value_policy::reference_internal)
        .def("getDecodeOperandFetchBuffer", &Pipeline::getDecodeOperandFetchBuffer, py::return_value_policy::reference_internal)
        .def("getOperandFetchExecuteBuffer", &Pipeline::getOperandFetchExecuteBuffer, py::return_value_policy::reference_internal)
        .def("getExecuteMemoryBuffer", &Pipeline::getExecuteMemoryBuffer, py::return_value_policy::reference_internal)
        .def("getMemoryWriteBackBuffer", &Pipeline::getMemoryWriteBackBuffer, py::return_value_policy::reference_internal);
}

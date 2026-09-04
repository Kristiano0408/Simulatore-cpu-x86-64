#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../include/cache/cacheManager.hpp"

namespace py = pybind11;


void bind_cacheManager(py::module &m) {
    //
    // CacheManager class
    //
    py::class_<CacheLine>(m, "CacheLine")
        .def(py::init<>())
        .def_property_readonly("data", [](const CacheLine &line) {
            std::vector<uint8_t> data;
            data.reserve(sizeof(line.data));
            std::memcpy(data.data(), line.data.data(), sizeof(line.data));
            return data;
        })
        .def_readonly("tag", &CacheLine::tag)
        .def_readonly("lastAccessTime", &CacheLine::lastAccessTime)
        .def_readonly("valid", &CacheLine::valid)
        .def_readonly("dirty", &CacheLine::dirty);

    py::enum_<RequestState>(m, "RequestState")
        .value("IDLE", RequestState::IDLE)
        .value("WAITING_LATENCY", RequestState::WAITING_LATENCY)
        .value("PROCESSING", RequestState::PROCESSING)
        .value("DONE", RequestState::DONE)
        .value("HIT_READY", RequestState::HIT_READY)
        .value("MISS_PENDING", RequestState::MISS_PENDING);

    py::enum_<RequestType>(m, "RequestType")
        .value("READ", RequestType::READ)
        .value("READ_MEMORY_FOR_WRITE_MISS", RequestType::READ_MEMORY_FOR_WRITE_MISS)
        .value("WRITE", RequestType::WRITE)
        .value("FILL", RequestType::FILL)
        .value("READ_AFTER_FILL", RequestType::READ_AFTER_FILL)
        .value("WRITE_AFTER_FILL", RequestType::WRITE_AFTER_FILL)
        .value("PREFETCH", RequestType::PREFETCH)
        .value("NONE", RequestType::NONE)
        .value("WRITE_BACK", RequestType::WRITE_BACK);
    
    py::enum_<TypeofData>(m, "TypeofData")
        .value("UINT8_T", TypeofData::UINT8_T)
        .value("UINT16_T", TypeofData::UINT16_T)
        .value("UINT32_T", TypeofData::UINT32_T)
        .value("UINT64_T", TypeofData::UINT64_T)
        .value("ARRAY_16B", TypeofData::ARRAY_16B)
        .value("ARRAY_64B", TypeofData::ARRAY_64B)
        .value("ARRAY_128B", TypeofData::ARRAY_128B)
        .value("UNKNOWN", TypeofData::UNKNOWN);
        
    py::class_<CacheRequest>(m, "CacheRequest")
        .def(py::init<>())
        .def_readonly("type", &CacheRequest::type)
        .def_readonly("dataType", &CacheRequest::dataType)
        .def_readonly("address", &CacheRequest::address)
        .def_property_readonly("data", [](const CacheRequest &request) {
            std::vector<uint8_t> data;
            data.reserve(request.data.size());
            std::memcpy(data.data(), request.data.data(), request.data.size());
            return data;
        })
        .def_readonly("isWrite", &CacheRequest::completed)
        .def_readonly("requestId", &CacheRequest::requestID);


    py::class_<PendingRequest>(m, "PendingRequest")
        .def(py::init<>())
        .def_readonly("CacheLine", &PendingRequest::line)
        .def_readonly("request", &PendingRequest::request)
        .def_readonly("remainingLatency", &PendingRequest::remainingLatency)
        .def_readonly("requestState", &PendingRequest::state);

    py::class_<CacheLevel>(m, "CacheLevel")
        .def("getPendingRequests", &CacheLevel::getPendingRequests, py::return_value_policy::reference_internal);

    py::class_<MemoryScheduler>(m, "MemoryScheduler")
        .def("getPendingRequests", &MemoryScheduler::getPendingRequests, py::return_value_policy::reference_internal);
    
    py::class_<CacheManager>(m, "CacheManager")
        .def("getL1Cache", &CacheManager::getL1ICache,py::return_value_policy::reference_internal)
        .def("getL1DCache", &CacheManager::getL1DCache, py::return_value_policy::reference_internal)
        .def("getL2Cache", &CacheManager::getL2Cache, py::return_value_policy::reference_internal)
        .def("getL3Cache", &CacheManager::getL3Cache, py::return_value_policy::reference_internal)
        .def("getMemoryScheduler", &CacheManager::getMemoryScheduler, py::return_value_policy::reference_internal)
        .def("getL1RequestQueue", &CacheManager::getL1IRequestQueue, py::return_value_policy::reference_internal)
        .def("getL1DRequestQueue", &CacheManager::getL1DRequestQueue, py::return_value_policy::reference_internal)
        .def("getL2RequestQueue", &CacheManager::getL2RequestQueue, py::return_value_policy::reference_internal)
        .def("getL3RequestQueue", &CacheManager::getL3RequestQueue, py::return_value_policy::reference_internal)
        .def("getMemoryRequestQueue", &CacheManager::getMemoryRequestQueue, py::return_value_policy::reference_internal)
        .def("flushAllCaches", &CacheManager::flushAllCaches);


    

    

}
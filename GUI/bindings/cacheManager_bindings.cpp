#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../include/cacheManager.hpp"

namespace py = pybind11;


void bind_cacheManager(py::module &m) {
    //
    // CacheManager class
    //
    py::class_<CacheManager>(m, "CacheManager")
        .def("getL1Cache", &CacheManager::getL1Cache, "Get the L1 cache level")
        .def("getL2Cache", &CacheManager::getL2Cache, "Get the L2 cache level")
        .def("getL3Cache", &CacheManager::getL3Cache, "Get the L3 cache level")
        .def("getMemoryScheduler", &CacheManager::getMemoryScheduler, "Get the memory scheduler")
        .def("getL1RequestQueue", &CacheManager::getL1RequestQueue, "Get the pending requests in the L1 cache level")
        .def("getL2RequestQueue", &CacheManager::getL2RequestQueue, "Get the pending requests in the L2 cache level")
        .def("getL3RequestQueue", &CacheManager::getL3RequestQueue, "Get the pending requests in the L3 cache level")
        .def("getMemoryRequestQueue", &CacheManager::getMemoryRequestQueue, "Get the pending requests in the memory scheduler");
        

    py::class_<CacheLevel>(m, "CacheLevel")
        .def("getPendingRequests", &CacheLevel::getPendingRequests, "Get the pending requests being processed by the cache scheduler");
        
}
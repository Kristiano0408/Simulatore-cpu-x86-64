#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../include/cacheManager.hpp"

namespace py = pybind11;


void bind_cacheManager(py::module &m) {
    //
    // CacheManager class
    //
    py::class_<CacheManager>(m, "CacheManager")
        .def("printCacheState", &CacheManager::printCacheState);
}
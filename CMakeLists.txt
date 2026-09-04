cmake_minimum_required(VERSION 3.20)

project(SimulatoreCPU CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ============================================================================ #
# OPTIONS
# ============================================================================ #

option(ENABLE_DEBUG "Enable debug logging" OFF)
option(GUI_ENABLED "Enable Python/pybind11 GUI integration" OFF)
option(PERF "Enable profiling" OFF)

# ============================================================================ #
# COMPILER OPTIONS
# ============================================================================ #

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
)

# ============================================================================ #
# CORE SOURCES
# ============================================================================ #

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# prova.cpp contains the main() and is not part of the core library
list(REMOVE_ITEM CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/prova.cpp"
)

# ============================================================================ #
# CORE LIBRARY
# ============================================================================ #

add_library(cpu_core STATIC ${CORE_SOURCES})

target_include_directories(cpu_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(ENABLE_DEBUG)
    target_compile_definitions(cpu_core PUBLIC DEBUG)
endif()

if(PERF)
    target_compile_definitions(cpu_core PUBLIC PERF)
    target_compile_options(cpu_core PUBLIC -g)
endif()

# ============================================================================ #
# MAIN EXECUTABLE
# ============================================================================ #

add_executable(prova
    "${CMAKE_CURRENT_SOURCE_DIR}/src/prova.cpp"
)

target_link_libraries(prova
    PRIVATE
        cpu_core
)

# ============================================================================ #
# TESTS
# ============================================================================ #

enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

foreach(test_src ${TEST_SOURCES})

    get_filename_component(test_name
        "${test_src}"
        NAME_WE
    )

    add_executable(${test_name}
        "${test_src}"
    )

    target_link_libraries(${test_name}
        PRIVATE
            cpu_core
    )

    add_test(
        NAME ${test_name}
        COMMAND ${test_name}
    )

endforeach()

# ============================================================================ #
# PYBIND11 / GUI
# ============================================================================ #

if(GUI_ENABLED)
    target_compile_definitions(cpu_core PUBLIC GUI_ENABLED)
    
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    # Use bundled pybind11 if available, otherwise search the system.
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/CMakeLists.txt")
        add_subdirectory(pybind11)
    else()
        find_package(pybind11 CONFIG REQUIRED)
    endif()

    file(GLOB_RECURSE BINDINGS_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/GUI/bindings/*.cpp"
    )

    pybind11_add_module(simulator
        ${BINDINGS_SOURCES}
    )

    target_link_libraries(simulator
        PRIVATE
            cpu_core
            Python3::Python
    )

    set_target_properties(simulator PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/GUI"
    )

endif()
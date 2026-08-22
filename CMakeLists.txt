cmake_minimum_required(VERSION 3.20)
project(SimulatoreCPU CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Utile per clangd e VSCode
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Definiamo le opzioni (equivalenti a DEBUG=1, GUI_ENABLED=1, PERF=1 del Makefile)
option(ENABLE_DEBUG "Abilita macro di debug (DEBUG=1)" OFF)
option(GUI_ENABLED "Abilita integrazione GUI (GUI_ENABLED=1)" OFF)
option(PERF "Abilita profiling (PERF=1)" OFF)

# Flag di compilazione globali (Hardening!)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)

# Applicazione delle definizioni se le opzioni sono attive
if(ENABLE_DEBUG)
    add_compile_definitions(DEBUG)
endif()

if(GUI_ENABLED)
    add_compile_definitions(GUI_ENABLED)
endif()

if(PERF)
    add_compile_options(-g)
    add_compile_definitions(PERF)
endif()

# Troviamo tutti i file sorgente in src/ ricorsivamente
file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
# Ma escludiamo prova.cpp, che è il main del nostro eseguibile di test
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/prova.cpp")

# --- 1. CORE LIBRARY ---
# Compiliamo il cuore della CPU in una libreria statica per riusarlo ovunque
add_library(cpu_core STATIC ${CORE_SOURCES})
target_include_directories(cpu_core PUBLIC include)


# --- 2. MAIN EXECUTABLE (prova) ---
add_executable(prova src/prova.cpp)
target_link_libraries(prova PRIVATE cpu_core)


# --- 3. TESTS EXECUTABLES ---
file(GLOB TEST_SOURCES "tests/*.cpp")
foreach(test_src ${TEST_SOURCES})
    # Estraiamo il nome del file senza estensione (es. da cacheTest.cpp a cacheTest)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE cpu_core)
endforeach()


# --- 4. PYBIND11 MODULE (GUI/simulator.so) ---
# Controlliamo se pybind11 è scaricato localmente (come fa il tuo makefile)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/CMakeLists.txt")
    add_subdirectory(pybind11)
else()
    # Fallback se è installato a livello di sistema
    find_package(pybind11 REQUIRED)
endif()

# Troviamo tutti i file di binding
file(GLOB_RECURSE BINDINGS_SOURCES "GUI/bindings/*.cpp")

# Creiamo il modulo Python
pybind11_add_module(simulator ${BINDINGS_SOURCES})
target_link_libraries(simulator PRIVATE cpu_core)

# Forziamo CMake a mettere l'output direttamente in GUI/ e non in build/
set_target_properties(simulator PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/GUI"
)

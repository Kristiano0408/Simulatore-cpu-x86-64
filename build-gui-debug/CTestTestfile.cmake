# CMake generated Testfile for 
# Source directory: /home/franco/Documents/GitHub/Simulatore-cpu-x86-64
# Build directory: /home/franco/Documents/GitHub/Simulatore-cpu-x86-64/build-gui-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[cacheTest]=] "/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/build-gui-debug/cacheTest")
set_tests_properties([=[cacheTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/CMakeLists.txt;103;add_test;/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/CMakeLists.txt;0;")
add_test([=[instructionTest]=] "/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/build-gui-debug/instructionTest")
set_tests_properties([=[instructionTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/CMakeLists.txt;103;add_test;/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/CMakeLists.txt;0;")
add_test([=[resultTest]=] "/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/build-gui-debug/resultTest")
set_tests_properties([=[resultTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/CMakeLists.txt;103;add_test;/home/franco/Documents/GitHub/Simulatore-cpu-x86-64/CMakeLists.txt;0;")
subdirs("pybind11")

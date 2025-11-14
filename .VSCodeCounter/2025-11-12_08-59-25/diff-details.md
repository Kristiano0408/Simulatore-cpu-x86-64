# Diff Details

Date : 2025-11-12 08:59:25

Directory /home/franco/Documents/GitHub/Simulatore-cpu-x86-64

Total : 79 files,  421 codes, 108 comments, 157 blanks, all 686 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [addressCalculator.cpp](/addressCalculator.cpp) | C++ | -62 | 0 | -34 | -96 |
| [addressCalculator.hpp](/addressCalculator.hpp) | C++ | -21 | 0 | -17 | -38 |
| [alu.cpp](/alu.cpp) | C++ | -33 | -16 | -18 | -67 |
| [alu.hpp](/alu.hpp) | C++ | -15 | -153 | -8 | -176 |
| [bus.cpp](/bus.cpp) | C++ | -12 | 0 | -7 | -19 |
| [bus.hpp](/bus.hpp) | C++ | -22 | 0 | -7 | -29 |
| [cacheManager.cpp](/cacheManager.cpp) | C++ | -365 | -77 | -143 | -585 |
| [cacheManager.hpp](/cacheManager.hpp) | C++ | -298 | -51 | -125 | -474 |
| [clock.cpp](/clock.cpp) | C++ | -7 | 0 | -3 | -10 |
| [clock.hpp](/clock.hpp) | C++ | -11 | 0 | -4 | -15 |
| [controlUnit.cpp](/controlUnit.cpp) | C++ | -236 | -104 | -131 | -471 |
| [controlUnit.hpp](/controlUnit.hpp) | C++ | -28 | -2 | -26 | -56 |
| [cpu.cpp](/cpu.cpp) | C++ | -37 | -52 | -13 | -102 |
| [cpu.hpp](/cpu.hpp) | C++ | -40 | -5 | -30 | -75 |
| [decoder.cpp](/decoder.cpp) | C++ | -285 | -40 | -123 | -448 |
| [decoder.hpp](/decoder.hpp) | C++ | -30 | -11 | -25 | -66 |
| [device.cpp](/device.cpp) | C++ | -20 | -1 | -8 | -29 |
| [device.hpp](/device.hpp) | C++ | -16 | -1 | -15 | -32 |
| [helpers.cpp](/helpers.cpp) | C++ | -341 | -3 | -54 | -398 |
| [helpers.hpp](/helpers.hpp) | C++ | -191 | -10 | -52 | -253 |
| [include/addressCalculator.hpp](/include/addressCalculator.hpp) | C++ | 21 | 0 | 17 | 38 |
| [include/alu.hpp](/include/alu.hpp) | C++ | 16 | 153 | 12 | 181 |
| [include/bus.hpp](/include/bus.hpp) | C++ | 22 | 0 | 7 | 29 |
| [include/cacheManager.hpp](/include/cacheManager.hpp) | C++ | 302 | 53 | 131 | 486 |
| [include/clock.hpp](/include/clock.hpp) | C++ | 11 | 0 | 4 | 15 |
| [include/controlUnit.hpp](/include/controlUnit.hpp) | C++ | 31 | 2 | 26 | 59 |
| [include/cpu.hpp](/include/cpu.hpp) | C++ | 46 | 6 | 39 | 91 |
| [include/decoder.hpp](/include/decoder.hpp) | C++ | 30 | 11 | 25 | 66 |
| [include/device.hpp](/include/device.hpp) | C++ | 16 | 1 | 15 | 32 |
| [include/helpers.hpp](/include/helpers.hpp) | C++ | 290 | 24 | 80 | 394 |
| [include/instruction.hpp](/include/instruction.hpp) | C++ | 117 | 13 | 73 | 203 |
| [include/instruction\_code\_map.hpp](/include/instruction_code_map.hpp) | C++ | 15 | 6 | 23 | 44 |
| [include/instruction\_wrapper.hpp](/include/instruction_wrapper.hpp) | C++ | 7 | 0 | 1 | 8 |
| [include/instructions/add\_instruction.hpp](/include/instructions/add_instruction.hpp) | C++ | 15 | 3 | 9 | 27 |
| [include/instructions/mov\_instruction.hpp](/include/instructions/mov_instruction.hpp) | C++ | 15 | 2 | 13 | 30 |
| [include/instructions/sub\_instruction.hpp](/include/instructions/sub_instruction.hpp) | C++ | 15 | 3 | 9 | 27 |
| [include/memory.hpp](/include/memory.hpp) | C++ | 63 | 11 | 37 | 111 |
| [include/opcode\_map.hpp](/include/opcode_map.hpp) | C++ | 21 | 2 | 6 | 29 |
| [include/operands.hpp](/include/operands.hpp) | C++ | 68 | 2 | 29 | 99 |
| [include/pipeline.hpp](/include/pipeline.hpp) | C++ | 162 | 11 | 99 | 272 |
| [include/registerFile.hpp](/include/registerFile.hpp) | C++ | 57 | 7 | 41 | 105 |
| [instruction.cpp](/instruction.cpp) | C++ | -400 | -55 | -170 | -625 |
| [instruction.hpp](/instruction.hpp) | C++ | -135 | -22 | -77 | -234 |
| [instruction\_code\_map.cpp](/instruction_code_map.cpp) | C++ | -68 | -4 | -24 | -96 |
| [instruction\_code\_map.hpp](/instruction_code_map.hpp) | C++ | -35 | -10 | -23 | -68 |
| [makefile](/makefile) | Makefile | 20 | 8 | 11 | 39 |
| [memory.cpp](/memory.cpp) | C++ | -72 | -9 | -24 | -105 |
| [memory.hpp](/memory.hpp) | C++ | -63 | -11 | -37 | -111 |
| [opcode\_map.cpp](/opcode_map.cpp) | C++ | -50 | -33 | -25 | -108 |
| [opcode\_map.hpp](/opcode_map.hpp) | C++ | -21 | -2 | -6 | -29 |
| [operands.cpp](/operands.cpp) | C++ | -244 | -55 | -113 | -412 |
| [operands.hpp](/operands.hpp) | C++ | -65 | -2 | -29 | -96 |
| [pipeline.cpp](/pipeline.cpp) | C++ | -205 | -30 | -85 | -320 |
| [pipeline.hpp](/pipeline.hpp) | C++ | -124 | -6 | -75 | -205 |
| [prova.cpp](/prova.cpp) | C++ | -64 | -221 | -44 | -329 |
| [prova2.cpp](/prova2.cpp) | C++ | -99 | -16 | -28 | -143 |
| [registerFile.cpp](/registerFile.cpp) | C++ | -50 | -7 | -25 | -82 |
| [registerFile.hpp](/registerFile.hpp) | C++ | -57 | -7 | -41 | -105 |
| [src/addressCalculator.cpp](/src/addressCalculator.cpp) | C++ | 62 | 0 | 34 | 96 |
| [src/alu.cpp](/src/alu.cpp) | C++ | 54 | 3 | 14 | 71 |
| [src/bus.cpp](/src/bus.cpp) | C++ | 13 | 0 | 7 | 20 |
| [src/cacheManager.cpp](/src/cacheManager.cpp) | C++ | 403 | 79 | 163 | 645 |
| [src/clock.cpp](/src/clock.cpp) | C++ | 7 | 0 | 3 | 10 |
| [src/controlUnit.cpp](/src/controlUnit.cpp) | C++ | 285 | 113 | 154 | 552 |
| [src/cpu.cpp](/src/cpu.cpp) | C++ | 41 | 52 | 15 | 108 |
| [src/decoder.cpp](/src/decoder.cpp) | C++ | 286 | 40 | 123 | 449 |
| [src/device.cpp](/src/device.cpp) | C++ | 20 | 1 | 8 | 29 |
| [src/helpers.cpp](/src/helpers.cpp) | C++ | 341 | 3 | 54 | 398 |
| [src/instruction.cpp](/src/instruction.cpp) | C++ | 223 | 5 | 80 | 308 |
| [src/instruction\_code\_map.cpp](/src/instruction_code_map.cpp) | C++ | 68 | 4 | 24 | 96 |
| [src/instructions/add\_instruction.cpp](/src/instructions/add_instruction.cpp) | C++ | 45 | 55 | 11 | 111 |
| [src/instructions/mov\_instruction.cpp](/src/instructions/mov_instruction.cpp) | C++ | 55 | 55 | 31 | 141 |
| [src/instructions/sub\_instruction.cpp](/src/instructions/sub_instruction.cpp) | C++ | 137 | 11 | 41 | 189 |
| [src/memory.cpp](/src/memory.cpp) | C++ | 72 | 9 | 24 | 105 |
| [src/opcode\_map.cpp](/src/opcode_map.cpp) | C++ | 50 | 33 | 25 | 108 |
| [src/operands.cpp](/src/operands.cpp) | C++ | 278 | 62 | 139 | 479 |
| [src/pipeline.cpp](/src/pipeline.cpp) | C++ | 306 | 115 | 96 | 517 |
| [src/prova.cpp](/src/prova.cpp) | C++ | 87 | 159 | 48 | 294 |
| [src/registerFile.cpp](/src/registerFile.cpp) | C++ | 50 | 7 | 25 | 82 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details
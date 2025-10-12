#ifndef PIPELINE_HPP
#define PIPELINE_HPP


#include <cstdint>
#include <array>
#include <string>
#include <memory>
#include "instruction.hpp"
#include "helpers.hpp"

struct Stage {

    bool busy = false; //indicates if the stage is busy
    bool stalled = false; //indicates if the stage is stalled
    bool flushed = false; //indicates if the stage is flushed

    std::unique_ptr<Instruction> instruction = nullptr; //pointer to the instruction in the stage
    

    
};


class Pipeline {




};










#endif // PIPELINE_HPP
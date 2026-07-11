#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifdef DEBUG
#define DEBUG_LOG(x) x
#else
#define DEBUG_LOG(x)
#endif

#include <iostream>
#include <string>
#include "instructionTypes.hpp"
#include "pipelineTypes.hpp"
#include "operandTypes.hpp"
#include "cacheTypes.hpp"

//debuging functions
inline void debugLog([[maybe_unused]] const std::string& message) 
{

    #ifdef DEBUG
    std::cout << "[DEBUG] " << message << std::endl;    
    #endif
}


template<typename T>
std::string to_string_hex(const T& value) {
    if constexpr (std::is_integral_v<T>) { // Solo tipi interi
        std::ostringstream oss;
        oss << "0x" 
            << std::hex << std::setw(sizeof(T)*2) << std::setfill('0') 
            << +value; // +value promuove i tipi piccoli
        return oss.str();
    }
    else 
    {
        return std::to_string(value); // fallback per altri tipi
    }
}


std::ostream& operator<<(std::ostream& os, const ErrorType& type);

std::ostream& operator<<(std::ostream& os, const EventType& type);

std::ostream& operator<<(std::ostream& os, const ComponentType& type);

std::string toStringTypeofInstruction(TypeofInstruction type);



#endif // DEBUG_HPP
#ifndef VARIOUS_HPP
#define VARIOUS_HPP

#include <array>
#include <cstdint>
#include <bit>
#include <string>
#include <cstring>
#include <type_traits>


template<typename T>
T extractValueFromBuffer(const std::array<uint8_t, 16>& buffer)
{
    std::array<uint8_t, sizeof(T)> tempBuffer{}; // Temporary buffer to hold the bytes for the value
    std::memcpy(tempBuffer.data(), buffer.data(), sizeof(T)); // Copy the bytes from the buffer to the temporary buffer
    return std::bit_cast<T>(tempBuffer); // Cast the bytes to the desired type T and return the value
}


template<typename T>
T castTo(uint64_t value) {

    static_assert(std::is_integral<T>::value, "T must be an integral type");
    return static_cast<T>(value);
}


// Utility function to compute log2 of power-of-two sizes
inline unsigned ilog2(uint64_t x)
{
    if (x == 0)
        return 0;

    unsigned result = 0;
    while (x > 1) {
        x >>= 1;   // sposta a destra di 1 bit
        result++;  // conta quanti shift fai
    }
    return result;
}

constexpr unsigned ilog2_constexpr(uint64_t x)
{
    return (x == 1) ? 0 : 1 + ilog2_constexpr(x >> 1);
}





#endif // VARIOUS_HPP
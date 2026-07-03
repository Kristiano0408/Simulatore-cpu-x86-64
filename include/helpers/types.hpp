#ifndef TYPES_HPP
#define TYPES_HPP
#include <cstddef>
#include <compare>
#include "maskAndConst.hpp"



using LineData = std::array<std::byte, CACHE_LINE_SIZE>; // Type alias for cache line data
using MaxCPUInstructionLength = std::array<std::byte, 16>; // Type alias for maximum CPU instruction length (16 bytes)
using Index = std::ptrdiff_t; // Use std::ptrdiff_t for index type, which is a signed integer type used for pointer arithmetic and array indexing


template<typename T, typename Tag>
struct TypeWrapper 
{
    T value{};
    
    constexpr TypeWrapper() = default;
    constexpr explicit TypeWrapper(T val) : value(val) {}
    constexpr operator T() const { return value; }



};


struct CacheAssociativityTag {};
struct CacheSizeTag {};
struct CacheLatencyTag {};
struct AddressTag {};
struct OffsetTag {};
struct TagLineTag {};


using CacheSize = TypeWrapper<std::uint32_t, CacheSizeTag>;
using CacheAssociativity = TypeWrapper<std::uint8_t, CacheAssociativityTag>;
using CacheLatency = TypeWrapper<std::uint8_t, CacheLatencyTag>;
using Address = TypeWrapper<std::uint64_t, AddressTag>; //or use std::uintptr_t if you want to ensure it's the same size as a pointer
using Offset = TypeWrapper<std::uint64_t, OffsetTag>; //or use std::uintptr_t if you want to ensure it's the same size as a pointer
using LineTag = TypeWrapper<std::uint64_t, TagLineTag>; //or use std::uintptr_t if you want to ensure it's the same size as a pointer

#endif // TYPES_HPP
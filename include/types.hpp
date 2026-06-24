#ifndef TYPES_HPP
#define TYPES_HPP
#include <cstddef>
#include <compare>

struct Index
{
    std::ptrdiff_t value {};

    constexpr Index() = default;
    constexpr explicit Index(std::ptrdiff_t v) : value(v) {}

    constexpr Index operator+(std::ptrdiff_t offset) const { return Index(value + offset); }
    constexpr Index operator-(std::ptrdiff_t offset) const { return Index(value - offset); }
    constexpr Index& operator+=(std::ptrdiff_t offset) { value += offset; return *this; }
    constexpr Index& operator-=(std::ptrdiff_t offset) { value -= offset; return *this; }
    constexpr Index operator*(std::ptrdiff_t other) const noexcept { return Index{value * other}; }
    constexpr Index operator/(std::ptrdiff_t divisor) const { return Index(value / divisor); }
    constexpr Index& operator++() { ++value; return *this; }
    constexpr Index& operator--() { --value; return *this; }
    constexpr Index operator++(int) { Index temp = *this; ++value; return temp; }
    constexpr Index operator--(int) { Index temp = *this; --value; return temp; }
    constexpr auto operator<=>(const Index& other) const = default;
    constexpr auto operator<=>(std::ptrdiff_t other) const { return value <=> other; }
    constexpr bool operator==(std::ptrdiff_t other) const { return value == other; }
    constexpr Index& operator=(std::ptrdiff_t v) { value = v; return *this; }
    constexpr Index& operator=(std::size_t v) { value = static_cast<std::ptrdiff_t>(v); return *this; }
    constexpr Index& operator=(int v) { value = static_cast<std::ptrdiff_t>(v); return *this; }

    constexpr  explicit operator std::ptrdiff_t() const { return value; }
    constexpr explicit operator std::size_t() const { return static_cast<std::size_t>(value); }
    constexpr explicit operator int() const { return static_cast<int>(value); }
    constexpr explicit operator unsigned() const { return static_cast<unsigned>(value); }
    constexpr explicit operator std::uint8_t() const { return static_cast<std::uint8_t>(value); }

};

template<typename T, typename Tag>
class TypeWrapper
{
    private:
        T value;

    public:
    constexpr TypeWrapper() = default;
    constexpr explicit TypeWrapper(const T& val) : value(val) {}
    constexpr TypeWrapper<T, Tag>& operator=(const T& val) { value = val; return *this; }
    constexpr TypeWrapper<T, Tag> operator+(const TypeWrapper<T, Tag>& other) const { return TypeWrapper(value + other.value); }
    constexpr TypeWrapper<T, Tag> operator-(const TypeWrapper<T, Tag>& other) const { return TypeWrapper(value - other.value); }
    constexpr TypeWrapper<T, Tag>& operator+=(const TypeWrapper<T, Tag>& other) { value += other.value; return *this; }

    constexpr auto operator<=>(const TypeWrapper<T, Tag>& other) const = default;
    constexpr bool operator==(const TypeWrapper<T, Tag>& other) const = default;
    
    template<std::integral U>
    constexpr TypeWrapper<T, Tag> operator+(const U& val) const { return TypeWrapper(value + static_cast<T>(val)); }
    template<std::integral U>
    constexpr TypeWrapper<T, Tag> operator-(const U& val) const { return TypeWrapper(value - static_cast<T>(val)); }

    constexpr explicit operator std::size_t() const  requires(!std::is_same_v<T, std::size_t>) { return static_cast<std::size_t>(value); }
    constexpr explicit operator T() const { return value; }

    std::ptrdiff_t toIndex() const { return static_cast<std::ptrdiff_t>(value); }

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
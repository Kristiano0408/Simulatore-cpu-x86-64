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


template<typename T>
class ReplacementPolicyQueueCacheFriendly
{
    private:
        std::vector<T> data; 
        std::vector<uint8_t> indexes; // This will hold the indexes of the elements in the order(LIFO)
        std::vector<uint8_t> positionOfIndexes; // This will hold the positions of the indexes in the order they were added to the queue.

    //indexes[i] will give you the index of the element in data, and positionOfIndexes[indexes[i]] will give you the position of that index in the queue.

    public:
        ReplacementPolicyQueueCacheFriendly(){
            data.reserve(256); // Reserve space for 256 elements to avoid frequent reallocations
            indexes.reserve(256); // Reserve space for 256 indexes
            positionOfIndexes.reserve(256); // Reserve space for 256 positions of indexes
        }

        void touch(const uint8_t& index)
        {
            uint8_t position = positionOfIndexes[index]; // Get the position of the index in the positionOfIndexes vector
            uint8_t lastPosition = static_cast<uint8_t>(indexes.size() - 1); // Get the last position in the positionOfIndexes vector
            if (position == lastPosition) 
                return; // If the index is already at the last position, do nothing
            
            std::swap(indexes[position], indexes[lastPosition]); // Swap the position of the index with the last position
            positionOfIndexes[indexes[lastPosition]] = lastPosition; // Update the position of the index that was swapped to the last position
            positionOfIndexes[indexes[position]] = position; // Update the position of the index that was swapped to the last position
        }
        void push(const T& value)
        {
            data.push_back(value);
            indexes.push_back(static_cast<uint8_t>(data.size() - 1)); // Store the index of the newly added element
            positionOfIndexes.push_back(static_cast<uint8_t>(indexes.size() - 1)); // Store the position of the index
        }
        
        T& getLastRecent()
        {
            uint8_t frontIndexOriginal = indexes.front(); // Get the index of the front element in the queue
            uint8_t backIndexOriginal = indexes.back(); // Get the index of the back element in the queue
            std::swap(indexes.front(), indexes.back()); // Swap the front index with the back index
            std::swap(positionOfIndexes[frontIndexOriginal], positionOfIndexes[backIndexOriginal]); // Swap the positions of the front and back indexes in the positionOfIndexes vector
            std::swap(data[frontIndexOriginal], data[backIndexOriginal]); // Swap the data of the front and back indexes in the data vector
        
            return data[indexes.front()]; // Return the value of the element that was originally at the front of the queue
        }

};

template<typename T>
class QueueCacheFriendly
{
    private:
        std::vector<T> data;
        std::vector<uint32_t> indexes; // This will hold the indexes of the elements in the order(LIFO)
        std::vector<uint32_t> positionOfIndexes; // This will hold the positions of the indexes in the order they were added to the queue.

    //indexes[i] will give you the index of the element in data, and positionOfIndexes[indexes[i]] will give you the position of that index in the queue.

    public:
        QueueCacheFriendly(){}

        void reserve(size_t size)
        {
            data.reserve(size); // Reserve space for 'size' elements to avoid frequent reallocations
            indexes.reserve(size); // Reserve space for 'size' indexes
            positionOfIndexes.reserve(size); // Reserve space for 'size' positions of indexes
        }

        size_t size() const
        {
            return indexes.size(); // Return the number of elements in the queue
        }
        
        void push(T&& value)
        {
            indexes.push_back(static_cast<uint32_t>(data.size()));
            positionOfIndexes.push_back(static_cast<uint32_t>(indexes.size() - 1)); // Store the position of the index
            data.push_back(std::move(value));
        }

        void push(const T& value)
        {
            indexes.push_back(static_cast<uint32_t>(data.size()));
            positionOfIndexes.push_back(static_cast<uint32_t>(indexes.size() - 1)); // Store the position of the index
            data.push_back(value);
        }

        T pop()   // rimuove l'elemento in testa logica (indexes.front())
        {
            uint32_t physIdx = indexes.front();      // slot fisico dell'elemento da rimuovere
            T value = std::move(data[physIdx]);      // estratto SUBITO, per valore: sicuro

            uint32_t lastPhys = static_cast<uint32_t>(data.size() - 1);   // vero ultimo slot fisico
            if (physIdx != lastPhys)
            {
                data[physIdx] = std::move(data[lastPhys]);              // rilocazione fisica corretta
                uint32_t movedLogicalPos = positionOfIndexes[lastPhys];
                indexes[movedLogicalPos] = physIdx;
                positionOfIndexes[physIdx] = movedLogicalPos;
            }
            data.pop_back();

            uint32_t lastLogical = static_cast<uint32_t>(indexes.size() - 1);
            indexes[0] = indexes[lastLogical];       // rimuovi la posizione logica 0 (swap-and-pop sull'ORDINE)
            positionOfIndexes[indexes[0]] = 0;
            indexes.pop_back();
            positionOfIndexes.pop_back();

            return value;
        }
        
};

#endif // TYPES_HPP
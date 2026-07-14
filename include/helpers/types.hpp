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
        std::vector<uint8_t> reverseIndexes; // This will hold the positions of the indexes in the order they were added to the queue.

    //indexes[i] will give you the index of the element in data, and reverseIndexes[indexes[i]] will give you the position of that index in the queue.

    public:
        ReplacementPolicyQueueCacheFriendly(){
            data.reserve(256); // Reserve space for 256 elements to avoid frequent reallocations
            indexes.reserve(256); // Reserve space for 256 indexes
            reverseIndexes.reserve(256); // Reserve space for 256 positions of indexes
        }

        void touch(const uint8_t& index)
        {
            uint8_t position = reverseIndexes[index]; // Get the position of the index in the reverseIndexes vector
            uint8_t lastPosition = static_cast<uint8_t>(indexes.size() - 1); // Get the last position in the reverseIndexes vector
            if (position == lastPosition) 
                return; // If the index is already at the last position, do nothing
            
            std::swap(indexes[position], indexes[lastPosition]); // Swap the position of the index with the last position
            reverseIndexes[indexes[lastPosition]] = lastPosition; // Update the position of the index that was swapped to the last position
            reverseIndexes[indexes[position]] = position; // Update the position of the index that was swapped to the last position
        }
        void push(const T& value)
        {
            data.push_back(value);
            indexes.push_back(static_cast<uint8_t>(data.size() - 1)); // Store the index of the newly added element
            reverseIndexes.push_back(static_cast<uint8_t>(indexes.size() - 1)); // Store the position of the index
        }
        
        T& getLastRecent()
        {
            uint8_t frontIndexOriginal = indexes.front(); // Get the index of the front element in the queue
            uint8_t backIndexOriginal = indexes.back(); // Get the index of the back element in the queue
            std::swap(indexes.front(), indexes.back()); // Swap the front index with the back index
            std::swap(reverseIndexes[frontIndexOriginal], reverseIndexes[backIndexOriginal]); // Swap the positions of the front and back indexes in the reverseIndexes vector
    
            return data[frontIndexOriginal]; // Return the value of the element that was originally at the front of the queue
        }

};

template<typename T, size_t N>
class FixedSizeQueueCacheFriendly
{
    private:
        std::array<T, N> data;
        uint64_t head = 0; // Index of the head of the queue
        uint64_t tail = 0; // Index of the tail of the queue


    public:
        FixedSizeQueueCacheFriendly(){}

        size_t size() const
        {
            return tail - head; // Return the current size of the queue
        }

        bool isFull() const
        {
            return size() == N; // Check if the queue is full
        }

        bool empty() const
        {
            return size() == 0; // Check if the queue is empty
        }
     
        void push(T&& value)
        {
            data[tail] = std::move(value);
            tail = (tail + 1) % N; // Wrap around if we reach the end of the array
            if (tail == head) // If the queue is full, move the head forward to overwrite the oldest element
            {
                head = (head + 1) % N;
            }
        }

        void push(const T& value)
        {
            data[tail] = value;
            tail = (tail + 1) % N; // Wrap around if we reach the end of the array
            if (tail == head) // If the queue is full, move the head forward to overwrite the oldest element
            {
                head = (head + 1) % N;
            }
        }

        T front() const
        {
            if (head == tail) 
                throw std::out_of_range("Queue is empty");
            return data[head]; // Return the value at the head of the queue
        }

        T pop()   // rimuove l'elemento in testa logica (indexes.front())
        {
            if (head == tail) 
                throw std::out_of_range("Queue is empty");
            T value = std::move(data[head]); // Extract the value at the head of the queue
            head = (head + 1) % N; // Move the head forward, wrapping around if necessary
            return value; // Return the extracted value
        }

        
};


template<typename T>
class QueueCacheFriendly
{
    private:

        std::vector<T> data;
        uint64_t head = 0; // Index of the head of the queue
        uint64_t tail = 0; // Index of the tail of the queue

    public:
        QueueCacheFriendly(){}

        void reserve(size_t size)
        {
            data.reserve(size); // Reserve space for 'size' elements to avoid frequent reallocations
        }

        size_t size() const
        {
            return data.size(); // Return the current size of the queue
        }

        void push(T&& value)
        {
            if(head >= tail/2)
            {
                data.erase(data.begin(), data.begin() + head); // Remove elements from the front of the vector to free up space
                tail = tail - head; // Adjust the tail index after erasing elements
                head = 0; // Reset the head index to 0
            }

            data.push_back(std::move(value));
            tail = (tail + 1) % data.capacity(); // Wrap around if we reach the end of the array
            if (tail == head) // If the queue is full, move the head forward to overwrite the oldest element
            {
                head = (head + 1) % data.capacity();
            }
        }

        void push(const T& value)
        {
            if(head >= tail/2)
            {
                data.erase(data.begin(), data.begin() + head); // Remove elements from the front of the vector to free up space
                tail = tail - head; // Adjust the tail index after erasing elements
                head = 0; // Reset the head index to 0
            }

            data.push_back(std::move(value));
            tail = (tail + 1) % data.capacity(); // Wrap around if we reach the end of the array
            if (tail == head) // If the queue is full, move the head forward to overwrite the oldest element
            {
                head = (head + 1) % data.capacity();
            }
        }

        T pop()   
        {
            if (head == tail) 
                throw std::out_of_range("Queue is empty");
            T value = std::move(data[head]); // Extract the value at the head of the queue
            head = (head + 1) % data.capacity(); // Move the head forward, wrapping around if necessary
            return value; // Return the extracted value
        }

        
};

#endif // TYPES_HPP
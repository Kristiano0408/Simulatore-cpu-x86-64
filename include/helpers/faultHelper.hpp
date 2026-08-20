#ifndef FAULTHELPER_HPP
#define FAULTHELPER_HPP


enum class FaultType : uint8_t
{
    NONE = 0,
    READ_ERROR = 1,
    WRITE_ERROR = 2,
    MEMORY_CORRUPTION = 3,
    BUS_ERROR = 4,
    DEVICE_NOT_RESPONDING = 5,
    UNKNOWN_ERROR = 254,
    SIZE = 255
};


constexpr uint16_t MAX_DEVICES = 256;

#endif // FAULTHELPER_HPP

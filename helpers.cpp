#include <cstdint>


bool isPrefix(uint8_t byte)
{
    if (byte == 0x66 || byte == 0x67 || byte == 0xF0 || byte == 0xF2 || byte == 0xF3 || byte == 0x2E || byte == 0x3E || byte == 0x26 || byte == 0x64 || byte == 0x65 || byte == 0x36)
    {
        return true;
    }
    else
    {
        return false;
    }
}
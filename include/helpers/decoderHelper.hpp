#ifndef DECODER_HELPER_HPP
#define DECODER_HELPER_HPP

#include <cstdint>

enum class Register : uint8_t;



bool isPrefix(uint8_t byte);

Register decodeRegisterReg(uint8_t reg, uint8_t rexprefix);

Register decodeRegisterRM(uint8_t reg, uint8_t rexprefix, bool hasSIB);

Register decodeRegisterSIB_base(uint8_t reg, uint8_t rexprefix, bool hasSIB);

Register decodeRegisterSIB_index(uint8_t reg, uint8_t rexprefix, bool hasSIB);


#endif // DECODER_HELPER_HPP
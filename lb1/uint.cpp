#include <iostream>
#include <cstdint>

void lfsr_uint128(uint64_t& high, uint64_t& low, uint64_t taps_high, uint64_t taps_low, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        bool new_bit = ((low & 1) ^ ((low & taps_low) != 0) ^ ((high & taps_high) != 0));
        
        low = (low >> 1) | ((high & 1) << 63);
        high = (high >> 1) | (static_cast<uint64_t>(new_bit) << 63);
        
        std::cout << new_bit;
    }
    std::cout << std::endl;
}

int main() {
    uint64_t high = 0b10110100101010011011010010101001; // Верхние 64 бита
    uint64_t low  = 0b11011010010101001101101001010100; // Нижние 64 бита
    uint64_t taps_high = 0xD000000000000000; // Полином для верхних бит
    uint64_t taps_low  = 0x000000000000000D; // Полином для нижних бит

    lfsr_uint128(high, low, taps_high, taps_low, 1000000);
    return 0;
}

#include <bitset>
#include <iostream>

template <size_t N>
void lfsr_bitset(std::bitset<N>& state, uint64_t taps, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        bool new_bit = (state[0] ^ (state & std::bitset<N>(taps)).count() % 2);  
        state >>= 1;              
        state[N - 1] = new_bit;   
        std::cout << new_bit;     
    }
    std::cout << std::endl;
}

int main() {
    std::bitset<128> state("1101010110010101100101011001010110010101100101011001010110010101");
    uint64_t taps = 0xD000000000000000; // Пример полинома
    lfsr_bitset(state, taps, 1000000);
    return 0;
}

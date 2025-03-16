#include <iostream>
#include <bitset>
#include <vector>
#include <cstdint>
#include <iomanip>

using namespace std;

vector<uint8_t> S = {0x8, 0xE, 0x2, 0x5, 0x6, 0x9, 0x1, 0xC, 0xF, 0x4, 0xB, 0x0, 0xD, 0xA, 0x3, 0x7};
const bitset<16> INIT_STATE16("1011010010101001");
const bitset<108> INIT_STATE108("1011011001");

bitset<16> state16 = INIT_STATE16;
bitset<108> state108 = INIT_STATE108;
vector<int> taps16 = {0, 6}, taps108 = {0, 4};

template <size_t N>
unsigned int lfsr(bitset<N> &state, const vector<int> &taps) {
    unsigned int bit = state[0];
    for (int tap : taps) bit ^= state[tap];
    state >>= 1;
    state.set(state.size() - 1, bit);
    return bit;
}

vector<uint8_t> mainFunction(bitset<108>& state108, bitset<16>& state16, const vector<int>& taps108, const vector<int>& taps16, const vector<uint8_t>& S, int length, int groupSize) {
    vector<uint8_t> sequence;
    int countBlocks = length / groupSize;
    for (int i = 0; i < countBlocks; i++) {
        uint8_t block = 0;
        for (int j = 0; j < groupSize; j += 4) {
            uint8_t fourBitValue = 0;
            for (int k = 0; k < 4; k++) {
                fourBitValue = (fourBitValue << 1) | (lfsr(state108, taps108) ^ lfsr(state16, taps16));
            }
            block = (block << 4) | S[fourBitValue];
        }
        if (groupSize == 8) {
            for (int shiftCount  = 0; shiftCount < 3; shiftCount++) {
                uint8_t bit = block & 1;
                block >>= 1;
                block |= (bit << 7);
            }    
        }
        sequence.push_back(block);
    }
    return sequence;
}

int main() {
    int length = 80;
    int groupSize;
    cout << "Enter the number of bits per group (4 or 8): ";
    cin >> groupSize;
    vector<uint8_t> sequence = mainFunction(state108, state16, taps108, taps16, S, length, groupSize);
    cout << "Generated sequence: ";
    for (uint8_t val : sequence) {
        if (groupSize == 4) {
            cout << hex << uppercase << (int)val << " ";
        } else {
            cout << hex << uppercase << setw(2) << setfill('0') << (int)val << " ";
        }
    }
    cout << endl;
    return 0;
}

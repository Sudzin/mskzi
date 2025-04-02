#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <string>
#include <map>
#include <iomanip>

using namespace std;

const bitset<16> INIT_STATE16(0b1011010010101001);
const bitset<108> INIT_STATE108("1111111001110010011000011001010100010010000101111111110101001010000000110001101111111011111011110010010000101111001101");
bitset<16> state16;
bitset<108> state108;
vector<int> taps16 = {0, 6}, taps108 = {0, 30};

template <size_t N>
unsigned int lfsr(bitset<N> &state, const vector<int> &taps) {
    unsigned int bit = state[0];
    for (int tap : taps) bit ^= state[tap];
    state >>= 1;
    state.set(state.size() - 1, bit);
    return bit;
}

vector<bool> generate_sequence(size_t sequence_length) {
    state16 = INIT_STATE16;
    state108 = INIT_STATE108;
    vector<bool> sequence;
    
    for (size_t i = 0; i < sequence_length; i++) {
        sequence.push_back(lfsr(state16, taps16) ^ lfsr(state108, taps108));
    }
    return sequence;
}

map<string, double> calculate_probabilities(const vector<bool>& sequence, int n) {
    map<string, int> count;
    map<string, double> probabilities;
    int len = sequence.size();

    for (int i = 0; i <= len - n; i++) {
        string combo(n, '0');
        for (int j = 0; j < n; j++)
            combo[j] = sequence[i + j] ? '1' : '0';
        count[combo]++;
    }

    for (const auto &pair : count)
        probabilities[pair.first] = (double)pair.second / (len - n + 1);
    
    return probabilities;
}

void save_csv(const string &filename, const map<string, double> &p10, const map<string, double> &p1000, const map<string, double> &p1000000, int n) {
    ofstream file(filename, ios::app);
    file << "Длина последовательности: " << n << "-битные комбинации\n";
    file << "Комбинация,10,1000,1000000\n";
    for (const auto &pair : p10) {
        file << pair.first << "," << fixed << setprecision(6)
             << pair.second << "," << (p1000.count(pair.first) ? p1000.at(pair.first) : 0.0) << ","
             << (p1000000.count(pair.first) ? p1000000.at(pair.first) : 0.0) << "\n";
    }
    file << "\n";
}

int main() {
    vector<bool> sequence_10 = generate_sequence(10);
    vector<bool> sequence_1000 = generate_sequence(1000);
    vector<bool> sequence_1000000 = generate_sequence(1000000);
    
    ofstream file("probabilities.csv", ios::trunc);
    file.close();
    
    for (int n = 1; n <= 4; n++) {
        auto p10 = calculate_probabilities(sequence_10, n);
        auto p1000 = calculate_probabilities(sequence_1000, n);
        auto p1000000 = calculate_probabilities(sequence_1000000, n);
        save_csv("probabilities.csv", p10, p1000, p1000000, n);
    }
    
    cout << "Результаты сохранены в probabilities.csv.\n";
    return 0;
}

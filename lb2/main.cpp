#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <string>
#include <map>

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

void maskFile(const string &input_path, const string &output_path, size_t sequence_length) {
    state16 = INIT_STATE16;
    state108 = INIT_STATE108;

    ifstream input(input_path, ios::binary);
    ofstream output(output_path, ios::binary);
    vector<char> buffer((istreambuf_iterator<char>(input)), {});

    size_t max_length = (sequence_length == 0) ? buffer.size() : min(sequence_length, buffer.size());

    for (size_t i = 0; i < max_length; i++) {
        unsigned char byte_mask = 0;
        for (int bit_pos = 0; bit_pos < 8; ++bit_pos)
            byte_mask |= ((lfsr(state16, taps16) ^ lfsr(state108, taps108)) << (7 - bit_pos));

        buffer[i] ^= byte_mask;
    }
    output.write(buffer.data(), buffer.size());
}

vector<bool> file_to_bit_sequence(const string &file_path) {
    ifstream file(file_path, ios::binary);
    vector<char> buffer((istreambuf_iterator<char>(file)), {});

    vector<bool> sequence;
    for (char byte : buffer)
        for (int i = 7; i >= 0; --i)
            sequence.push_back((byte >> i) & 1);

    return sequence;
}

map<string, double> calculate_probabilities(const vector<bool>& sequence, int n) {
    map<string, int> count;
    map<string, double> probabilities;
    int len = sequence.size();

    for (int i = 0; i <= len - n; i++) {
        string combo;
        for (int j = 0; j < n; j++)
            combo += sequence[i + j] ? '1' : '0';

        count[combo]++;
    }

    for (const auto &pair : count)
        probabilities[pair.first] = (double)pair.second / (len - n + 1);

    return probabilities;
}

void save_csv(const string &filename, const string &header, const map<string, double> &before, const map<string, double> &after, int length) {
    ofstream file(filename, ios::trunc);
    file << header << "\n";

    for (const auto &pair : before)
        file << length << "," << pair.first << "," << pair.second << "," 
             << (after.count(pair.first) ? after.at(pair.first) : 0.0) << "\n";
}

double autocorrelation(int shift, const vector<bool>& sequence) {
    int match = 0, size = sequence.size();
    for (int i = 0; i < size - shift; i++)
        if (sequence[i] == sequence[i + shift])
            match++;

    return (double)match / (size - shift);
}

void save_autocorrelation(const vector<bool>& sequence) {
    ofstream file("autocorrelation.csv", ios::trunc);
    file << "shift,value\n";
    for (int tau = 0; tau <= 32; tau++)
        file << tau << "," << autocorrelation(tau, sequence) << "\n";
}

int main() {
    string input_file, masked_file = "masked.bin";
    size_t sequence_length;
    int n;

    cout << "Введите путь к входному файлу: ";
    cin >> input_file;

    cout << "Введите длину маскируемой последовательности (0 - весь файл): ";
    cin >> sequence_length;

    if (sequence_length == 0) {
        ifstream input(input_file, ios::binary);
        input.seekg(0, ios::end);
        sequence_length = input.tellg();
        cout << "Длина последовательности установлена: " << sequence_length << " байт.\n";
    }

    cout << "Введите длину комбинации (1-4): ";
    cin >> n;
    if (n < 1 || n > 4) return cout << "Ошибка: n должно быть от 1 до 4!\n", 1;

    vector<bool> original_sequence = file_to_bit_sequence(input_file);
    auto before_prob = calculate_probabilities(original_sequence, n);

    maskFile(input_file, masked_file, sequence_length);
    vector<bool> masked_sequence = file_to_bit_sequence(masked_file);
    auto after_prob = calculate_probabilities(masked_sequence, n);

    save_csv("probabilities.csv", "length,combination,before,after", before_prob, after_prob, n);
    cout << "Результаты сохранены в probabilities.csv.\n";

    save_autocorrelation(original_sequence);
    cout << "Автокорреляция сохранена в autocorrelation.csv.\n";

    return 0;
}

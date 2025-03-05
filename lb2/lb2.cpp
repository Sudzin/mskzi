#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include <string>
#include <map>
#include <vector>

using namespace std;

bitset<16> state16(0b1011010010101001);
bitset<108> state108("1111111001110010011000011001010100010010000101111111110101001010000000110001101111111011111011110010010000101111001101");
vector<int> taps16 = {0, 6};
vector<int> taps108 = {0, 30};

template <size_t N>
unsigned int lfsr(bitset<N> &state, const vector<int> &taps) {
    unsigned int bit = state[0];
    for (int tap : taps) bit ^= state[tap];
    state >>= 1;
    state.set(state.size() - 1, bit);
    return bit;
}

void maskFile(const string &input_path, const string &output_path, bool mask_header, size_t sequence_length) {
    ifstream input(input_path, ios::binary);
    ofstream output(output_path, ios::binary);
    vector<char> buffer((istreambuf_iterator<char>(input)), {});
    input.close();
    size_t start_index = mask_header ? 100 : 1;
    size_t max_length = sequence_length == 0 ? buffer.size() : min(sequence_length, buffer.size());

    for (size_t i = start_index; i < max_length; i++) {
        unsigned char byte_mask = 0;
        for (int bit_pos = 0; bit_pos < 8; ++bit_pos) {
            unsigned int bit16 = lfsr(state16, taps16);
            unsigned int bit108 = lfsr(state108, taps108);
            unsigned int bit = bit16 ^ bit108;
            byte_mask |= (bit << (7 - bit_pos));
        }
        buffer[i] ^= byte_mask;
    }

    output.write(buffer.data(), buffer.size());
    output.close();
}

map<string, double> calculations(int n, bitset<16> bits) {
    int len = bits.size();
    map<string, int> count;
    map<string, double> probabilities;
    for (int i = len - 1; i >= (n - 1); i--) {
        string combo = "";
        for (int j = 0; j < n; j++) {
            combo += bits[i - j] ? '1' : '0';
        }
        count[combo]++;
    }
    int countCombinations = len - n + 1;
    for (const auto& pair : count) {
        probabilities[pair.first] = (double)pair.second / countCombinations;
    }
    return probabilities;
}

double autocorrelation(int shift, const bitset<16>& bits) {
    int size = bits.size();
    int match = 0;
    for (int i = 0; i < size - shift; i++) {
        if (bits[i] == bits[i + shift]) {
            match++;
        }
    }
    return (double)match / (size - shift);
}

int main() {
    int choice;
    cout << "1. Маскировать файл\n";
    cout << "2. Выполнить статистический анализ\n";
	cout << "Выберите действие: "  << endl;
    cin >> choice;

    if (choice == 1) {
        string input_file, output_file;
        bool mask_header;
        size_t sequence_length;

        cout << "Введите путь к входному файлу: ";
        cin >> input_file;
        cout << "Введите путь к выходному файлу: ";
        cin >> output_file;
        cout << "Введите длину последовательности (0 - весь файл): ";
        cin >> sequence_length;
        cout << "Маскировать ли заголовок? (1 - да, 0 - нет): ";
        cin >> mask_header;

        maskFile(input_file, output_file, mask_header, sequence_length);
    } else if (choice == 2) {
        int n, shift;
        bitset<16> bits(0b1011010100011001);
        
        cout << "Введите N (длину комбинации): ";
        cin >> n;
        cout << "Введите сдвиг: ";
        cin >> shift;
        
        if (n > 4) return 0;
        map<string, double> probabilities = calculations(n, bits);
        cout << fixed << setprecision(2);
        cout << "Вероятности комбинаций:\n";
        for (const auto& pair : probabilities) {
            cout << pair.first << " " << pair.second << endl;
        }

        cout << "Автокорреляция:\n";
        for (int shift = 0; shift <= bits.size() - 1; shift++) {
            double result = autocorrelation(shift, bits);
            cout << shift << " - " << result << endl;
        }
    } else {
        cout << "Некорректный выбор!\n";
    }
    return 0;
}

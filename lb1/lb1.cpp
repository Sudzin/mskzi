#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>

using namespace std;

template <size_t N>
unsigned int lfsr(bitset<N> &state, const vector<int> &taps) {
    unsigned int bit = state[0];
    for (int tap : taps) {
        bit ^= state[tap];
    }
    state >>= 1;
    state.set(state.size() - 1, bit);
    return bit;
}

void generatePSP(bitset<16> &state16, bitset<108> &state108, vector<int> &taps16, vector<int> &taps108, size_t length, vector<uint8_t> &sequence) {
    sequence.clear();
    for (size_t i = 0; i < length / 8; ++i) {
        uint8_t byte = 0;
        for (int j = 0; j < 8; j++) {
            unsigned int bit16 = lfsr(state16, taps16);
            unsigned int bit108 = lfsr(state108, taps108);
            unsigned int combined_bit = bit16 ^ bit108;
            byte |= (combined_bit << j);
        }
        sequence.push_back(byte);
    }
}

void maskFile(const string &input_path, const string &output_path, size_t sequence_length, bool mask_header) {
    bitset<16> state16(0b1011010010101001);
    bitset<108> state108(string("1111111001110010011000011001010100010010000101111111110101001010000000110001101111111011111011110010010000101111001101"));
    vector<int> taps16 = {0, 6};
    vector<int> taps108 = {0, 30};

    vector<uint8_t> sequence;
    generatePSP(state16, state108, taps16, taps108, sequence_length, sequence);

    ifstream input(input_path, ios::binary | ios::ate);
    streamsize file_size = input.tellg();
    input.seekg(0, ios::beg);
    ofstream output(output_path, ios::binary);

    vector<char> buffer(file_size);
    input.read(buffer.data(), file_size);
    input.close();

    vector<char> masked_buffer(file_size);

    size_t sequence_index = 0;
    for (streamsize i = 0; i < file_size; i++) {	
        if (mask_header && i < 100) {
            masked_buffer[i] = buffer[i];
        } else {
            masked_buffer[i] = buffer[i] ^ sequence[sequence_index++ % sequence.size()];
        }
    }

    output.write(masked_buffer.data(), file_size);
    output.close();
    cout << "Файл успешно замаскирован!" << endl;
}

int main() {
    string input_file, output_file;
    size_t sequence_length;
    bool mask_header;
    int choice;

    cout << "Введите путь к входному файлу: ";
    cin >> input_file;

    ifstream input(input_file, ios::binary | ios::ate);

    streamsize file_size = input.tellg();
    input.close();

    cout << "Размер файла: " << file_size << " байт" << endl;
    
    cout << "Выберите вариант длины ПСП:\n"
         << "1 - Использовать размер файла (" << file_size << " байт)\n"
         << "2 - Ввести длину вручную\n"
         << "Ваш выбор: ";
    cin >> choice;

    if (choice == 1) {
        sequence_length = file_size;
    } else {
        cout << "Введите желаемую длину последовательности: ";
        cin >> sequence_length;
        if (sequence_length <= 0) {
            cerr << "Ошибка: длина должна быть больше 0!" << endl;
            return 1;
        }
    }

    cout << "Введите путь к выходному файлу: ";
    cin >> output_file;
    cout << "Маскировать ли заголовок? (1 - да, 0 - нет): ";
    cin >> mask_header;

    maskFile(input_file, output_file, sequence_length, mask_header);
    return 0;
}

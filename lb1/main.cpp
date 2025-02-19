#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

uint lfsr16(uint state, uint length, ostringstream &buffer) {
    const uint tap = 1 << 6;

    for (uint i = 0; i < length; i++) {
        uint bit1 = (state & 0x01) ^ ((state & tap) >> 6);
        state = (state >> 1) | (bit1 << 15);
        buffer << i + 1 << "," << bit1 << ",";
    }

    return state;
}

bitset<108> lfsr108(bitset<108> state, uint length, ostringstream &buffer) {
    const uint tap = 108 - 31 - 1;

    for (uint i = 0; i < length; i++) {
        uint bit1 = state[0] ^ state[tap];
        state >>= 1;
        state.set(107, bit1);
        buffer << bit1 << ",";
    }

    return state;
}

void combinedGenerator(uint length) {
    ofstream outputFile("output.csv");
    if (!outputFile) {
        cout << "Ошибка при открытии файла!" << endl;
        return;
    }

    uint state16 = 0b1011010010101001;
    bitset<108> state108(string("1111111001110010011000011001010100010010000101111111110101001010000000110001101111111011111011110010010000101111001101"));

    ostringstream buffer;
    buffer << "Шаг,Выход 16-бит,Выход 108-бит,Итоговый бит (XOR)\n";

    for (uint i = 0; i < length; i++) {
        uint bit16 = (state16 & 0x01) ^ ((state16 & (1 << 6)) >> 6);
        uint bit108 = state108[0] ^ state108[108 - 31 - 1];
        uint final_bit = bit16 ^ bit108;

        state16 = (state16 >> 1) | (bit16 << 15);
        state108 >>= 1;
        state108.set(107, bit108);

        buffer << i + 1 << "," << bit16 << "," << bit108 << "," << final_bit << "\n";
    }

    outputFile << buffer.str();
    outputFile.close();
    cout << "Результаты записаны в файл output.csv" << endl;
}

void maskFile(const string& input_path, const string& output_path, bool mask_header, size_t header_size) {
    uint state16 = 0b1011010010101001;
    bitset<108> state108(string("1111111001110010011000011001010100010010000101111111110101001010000000110001101111111011111011110010010000101111001101"));

    ifstream input(input_path, ios::binary);
    ofstream output(output_path, ios::binary);

    if (!input || !output) {
        cerr << "Ошибка при открытии файлов!" << endl;
        return;
    }

    if (!mask_header && header_size > 0) {
        vector<char> header(header_size);
        input.read(header.data(), header_size);
        output.write(header.data(), header_size);
    }

    char byte;
    while (input.get(byte)) {
        uint bit16 = (state16 & 0x01) ^ ((state16 & (1 << 6)) >> 6);
        uint bit108 = state108[0] ^ state108[108 - 31 - 1];
        uint mask_byte = (bit16 ^ bit108) * 255;
        output.put(byte ^ mask_byte);

        state16 = (state16 >> 1) | (bit16 << 15);
        state108 >>= 1;
        state108.set(107, bit108);
    }

    input.close();
    output.close();
    cout << "Файл успешно замаскирован!" << endl;
}

int main() {
    int choice;
    cout << "Выберите режим:\n1 - Генерация ПСП\n2 - Маскирование файла\n";
    cin >> choice;

    if (choice == 1) {
        int length;
        cout << "Введите количество бит в последовательности: ";
        cin >> length;
        combinedGenerator(length);
    } else if (choice == 2) {
        string input_file, output_file;
        int mask_choice;
        size_t header_size = 0;

        cout << "Введите путь к входному файлу: ";
        cin >> input_file;
        cout << "Введите путь к выходному файлу: ";
        cin >> output_file;
        cout << "Маскировать заголовок? (1-да/0-нет): ";
        cin >> mask_choice;

        if (!mask_choice) {
            cout << "Введите размер заголовка: ";
            cin >> header_size;
        }

        maskFile(input_file, output_file, mask_choice == 1, header_size);
    } else {
        cout << "Неверный выбор!" << endl;
    }

    return 0;
}

#include <iostream>
<<<<<<< HEAD
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
=======
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <sys/resource.h> // Для измерения памяти в Linux

using namespace std;
using namespace std::chrono;

// Таблица замены S (вариант 7)
const uint8_t S[16] = {0x8, 0xE, 0x2, 0x5, 0x6, 0x9, 0x1, 0xC, 
                       0xF, 0x4, 0xB, 0x0, 0xD, 0xA, 0x3, 0x7};

// Глобальные таблицы
uint8_t S_star[256];     // Для 8-битной подстановки (S*)
uint8_t S_combined[256]; // Для комбинированного преобразования (S')
const int p = 3;         // Сдвиг для перестановки

// Инициализация таблиц
void init_tables() {
    // 1. Таблица S* для 8-битной подстановки
    for (int x1 = 0; x1 < 16; ++x1) {
        for (int x2 = 0; x2 < 16; ++x2) {
            S_star[(x1 << 4) | x2] = (S[x1] << 4) | S[x2];
        }
    }
    
    // 2. Комбинированная таблица S' (S* + сдвиг)
    for (int i = 0; i < 256; ++i) {
        S_combined[i] = (S_star[i] >> p) | (S_star[i] << (8 - p));
    }
}

// Задание 1: 4-битная подстановка
void task1(uint64_t input, uint8_t output[16]) {
    for (int i = 0; i < 16; ++i) {
        output[15-i] = S[(input >> (i * 4)) & 0xF];
    }
}

// Задание 2: 8-битная подстановка
void task2(uint64_t input, uint8_t output[8]) {
    for (int i = 0; i < 8; ++i) {
        output[7-i] = S_star[(input >> (i * 8)) & 0xFF];
    }
}

// Задание 3: Перестановка сдвигом
void task3(uint8_t input[8], uint8_t output[8]) {
    for (int i = 0; i < 8; ++i) {
        output[i] = (input[i] >> p) | (input[i] << (8 - p));
    }
}

// Задание 4: Комбинированное преобразование
void task4(uint64_t input, uint8_t output[8]) {
    uint8_t temp[8];
    task2(input, temp);
    task3(temp, output);
}

// Функция для измерения использования памяти (Linux)
size_t get_memory_usage() {
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // В KB
}

// Задание 5: Обработка файла с замером времени/памяти
void task5() {
    auto start = high_resolution_clock::now();
    size_t mem_before = get_memory_usage();

    ifstream in("text.txt", ios::binary);
    if (!in) {
        cerr << "Error opening file!" << endl;
        return;
    }
    
    cout << "\n5. File content after transformation (S* + shift):\n";
    char block[64];
    int counter = 0;
    
    while (in.read(block, sizeof(block))) {
        for (int i = 0; i < 64; ++i) {
            uint8_t val = S_star[(uint8_t)block[i]];
            uint8_t transformed = (val >> p) | (val << (8 - p));
            cout << hex << setw(2) << setfill('0') << static_cast<int>(transformed) << " ";
            if (++counter % 16 == 0) cout << endl;
        }
    }
    
    // Обработка остатка
    if (in.gcount() > 0) {
        for (int i = 0; i < in.gcount(); ++i) {
            uint8_t val = S_star[(uint8_t)block[i]];
            uint8_t transformed = (val >> p) | (val << (8 - p));
            cout << hex << setw(2) << setfill('0') << static_cast<int>(transformed) << " ";
            if (++counter % 16 == 0) cout << endl;
        }
    }
    
    auto end = high_resolution_clock::now();
    size_t mem_after = get_memory_usage();
    
    cout << "\nTotal bytes processed: " << dec << counter << endl;
    cout << "Time taken: " << duration_cast<microseconds>(end - start).count() << " μs" << endl;
    cout << "Memory used: " << (mem_after - mem_before) << " KB" << endl;
}

// Задание 6: Комбинированная операция через S'
void task6(uint64_t input, uint8_t output[8]) {
    for (int i = 0; i < 8; ++i) {
        output[7-i] = S_combined[(input >> (i * 8)) & 0xFF];
    }
}

// Задание 7: Обработка файла с замером времени/памяти
void task7() {
    auto start = high_resolution_clock::now();
    size_t mem_before = get_memory_usage();

    ifstream in("text.txt", ios::binary);
    if (!in) {
        cerr << "Error opening file!" << endl;
        return;
    }
    
    cout << "\n7. File content after transformation (using S'):\n";
    char block[64];
    int counter = 0;
    
    while (in.read(block, sizeof(block))) {
        for (int i = 0; i < 64; ++i) {
            uint8_t transformed = S_combined[(uint8_t)block[i]];
            cout << hex << setw(2) << setfill('0') << static_cast<int>(transformed) << " ";
            if (++counter % 16 == 0) cout << endl;
        }
    }
    
    // Обработка остатка
    if (in.gcount() > 0) {
        for (int i = 0; i < in.gcount(); ++i) {
            uint8_t transformed = S_combined[(uint8_t)block[i]];
            cout << hex << setw(2) << setfill('0') << static_cast<int>(transformed) << " ";
            if (++counter % 16 == 0) cout << endl;
        }
    }
    
    auto end = high_resolution_clock::now();
    size_t mem_after = get_memory_usage();
    
    cout << "\nTotal bytes processed: " << dec << counter << endl;
    cout << "Time taken: " << duration_cast<microseconds>(end - start).count() << " μs" << endl;
    cout << "Memory used: " << (mem_after - mem_before) << " KB" << endl;
}

int main() {
    init_tables();
    uint64_t test_data = 0x0123456789ABCDEF;
    uint8_t result[16];
    
    // Задание 1
    task1(test_data, result);
    cout << "1. 4-bit substitution: ";
    for (int i = 0; i < 16; ++i) {
        cout << hex << uppercase << static_cast<int>(result[i]);
    }
    
    // Задание 2
    task2(test_data, result);
    cout << "\n2. 8-bit substitution: ";
    for (int i = 0; i < 8; ++i) {
        cout << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(result[i]);
    }
    
    // Задание 3
    uint8_t perm_result[8];
    task3(result, perm_result);
    cout << "\n3. Permutation: ";
    for (int i = 0; i < 8; ++i) {
        cout << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(perm_result[i]);
    }
    
    // Задание 4
    task4(test_data, result);
    cout << "\n4. Combined: ";
    for (int i = 0; i < 8; ++i) {
        cout << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(result[i]);
    }
    
    // Задание 5 с замерами
    cout << "\n\n5. File processing (S* + shift)...";
    task5();
    
    // Задание 6
    task6(test_data, result);
    cout << "\n6. Combined using S': ";
    for (int i = 0; i < 8; ++i) {
        cout << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(result[i]);
    }
    
    // Задание 7 с замерами
    cout << "\n\n7. File processing (using S')...";
    task7();
    
    cout << "\nAll tasks completed!" << endl;
>>>>>>> 18db726 (54)
    return 0;
}

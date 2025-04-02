#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;

// Два регистра LFSR
bitset<16> state16(0b1011010010101001);
bitset<108> state108("1111111001110010011000011001010100010010000101111111110101001010000000110001101111111011111011110010010000101111001101");

vector<int> taps16 = {0, 6};   // Точки обратной связи для 16-битного регистра
vector<int> taps108 = {0, 30}; // Точки обратной связи для 108-битного регистра

// Функция генерации случайных битов через два LFSR
template <size_t N>
unsigned int lfsr(bitset<N>& state, const vector<int>& taps) {
    unsigned int bit = state[0];
    for (int tap : taps) bit ^= state[tap];
    state >>= 1;
    state.set(state.size() - 1, bit);
    return bit;
}

// Генерация ПСП длиной len через два регистра и XOR
bitset<1000000> generate_psp(int len) {
    bitset<1000000> sequence;
    for (int i = 0; i < len; i++) {
        unsigned int bit16 = lfsr(state16, taps16);
        unsigned int bit108 = lfsr(state108, taps108);
        sequence.set(i, bit16 ^ bit108); // XOR двух битов
    }
    return sequence;
}

// Функция для вероятностной оценки комбинаций
map<string, double> calculate_probabilities(int n, const bitset<1000>& bits) {
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

// Функция автокорреляции
double autocorrelation(int shift, const bitset<1000>& bits) {
    int size = bits.size();
    int match = 0;
    for (int i = 0; i < size - shift; i++) {
        if (bits[i] == bits[i + shift]) {
            match++;
        }
    }
    return ((double)match - (size - match)) / ((double)match + (size - match));
}

// Запись автокорреляции в файл
void save_autocorrelation_to_csv(const string& filename, const bitset<1000>& bits) {
    ofstream output(filename);
    if (!output) {
        cerr << "Ошибка: не удалось создать " << filename << endl;
        return;
    }

    cout << "Записываем автокорреляцию в " << filename << "...\n";
    output << "Сдвиг,Автокорреляция\n";
    for (int shift = 0; shift <= 32; shift++) {
        double result = autocorrelation(shift, bits);
        output << shift << "," << fixed << setprecision(5) << result << "\n";
    }
    output.close();
    cout << "Файл " << filename << " успешно записан!\n";
}

// Функция для записи вероятностей в CSV файл
void save_probabilities_to_csv(const string& filename, const bitset<1000>& bits) {
    ofstream output(filename);
    if (!output) {
        cerr << "Ошибка: не удалось создать " << filename << endl;
        return;
    }

    output << "Комбинация,Вероятность_1,Вероятность_2,Вероятность_3,Вероятность_4\n"; // Заголовки для разных длин
    for (int n = 1; n <= 4; n++) {
        auto probabilities = calculate_probabilities(n, bits);
        for (const auto& pair : probabilities) {
            output << pair.first;
            for (int i = 1; i <= 4; i++) {
                output << "," << fixed << setprecision(5) << pair.second;
            }
            output << "\n";
        }
    }
    output.close();
    cout << "Файл с вероятностями успешно записан!\n";
}

int main() {
    // Генерация последовательности длиной 1000 бит
    cout << "Генерируем последовательность длиной 1000 бит...\n";
    bitset<1000> sequence = generate_psp(1000);
    cout << "Готово! Начинаем автокорреляцию...\n";

    // Запись автокорреляции в CSV файл
    save_autocorrelation_to_csv("autocorrelation.csv", sequence);

    // Запись вероятностей в CSV файл
    save_probabilities_to_csv("probabilities.csv", sequence);

    cout << "Все операции завершены!\n";
    return 0;
}

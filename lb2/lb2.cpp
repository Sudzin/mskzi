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
vector<int> taps108 = {0, 7, 15, 30}; // Точки обратной связи для 108-битного регистра

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

// Подсчет вероятностей появления комбинаций длины N
map<string, double> calculate_probabilities(int N, const bitset<1000000>& sequence, int len) {
    map<string, int> count;
    int total_combinations = len - N + 1;

    for (int i = 0; i < total_combinations; i++) {
        string combo = "";
        for (int j = 0; j < N; j++) {
            combo += sequence[i + j] ? '1' : '0';
        }
        count[combo]++;
    }

    map<string, double> probabilities;
    for (const auto& pair : count) {
        probabilities[pair.first] = (double)pair.second / total_combinations;
    }
    return probabilities;
}

// Функция автокорреляции (исправленная версия)
double autocorrelation(int shift, const bitset<1000000>& bits) {
    if (shift == 0) return 1.0; // При нулевом сдвиге корреляция = 1
    
    int size = bits.size();
    if (shift >= size) return 0.0; // При слишком большом сдвиге = 0

    int match = 0;
    for (int i = 0; i < size - shift; i++) {
        if (bits[i] == bits[i + shift]) match++;
    }

    double total = size - shift;
    return (2.0 * match - total) / total; // Нормализованное значение [-1, 1]
}

// Сохранение результатов автокорреляции в CSV с выводом в консоль
void save_autocorrelation_to_csv(const bitset<1000000>& sequence) {
    ofstream file("autocorrelation.csv");
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла для автокорреляции!\n";
        return;
    }

    file << "Shift,Autocorrelation\n";
    cout << "Результаты автокорреляции:\n";
    cout << "Shift\tAutocorrelation\n";
    cout << "----------------------\n";
    
    for (int shift = 0; shift <= 32; shift++) {
        double result = autocorrelation(shift, sequence);
        file << shift << "," << fixed << setprecision(6) << result << "\n";
        cout << shift << "\t" << fixed << setprecision(6) << result << "\n";
    }

    file.close();
    cout << "\nРезультаты сохранены в файл: autocorrelation.csv\n";
}

// Запись результатов в CSV-файл + вывод в консоль
void save_to_csv(const string& filename, int N, const vector<int>& lengths) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла!\n";
        return;
    }

    // Получаем все возможные комбинации длины N
    set<string> unique_combinations;
    int num_combinations = 1 << N;

    for (int i = 0; i < num_combinations; i++) {
        string binary = bitset<4>(i).to_string().substr(4 - N);
        unique_combinations.insert(binary);
    }

    // Заголовок CSV
    file << "Длина;";
    for (const string& combo : unique_combinations) {
        file << combo << ";";
    }
    file << "\n";

    // Вывод в консоль
    cout << "\nСтатистика для N = " << N << "\n";
    cout << setw(10) << "Длина";
    for (const string& combo : unique_combinations) {
        cout << setw(10) << combo;
    }
    cout << "\n" << string(60, '-') << "\n";

    // Обработка данных
    map<int, map<string, double>> results;
    for (int len : lengths) {
        bitset<1000000> sequence = generate_psp(len);
        results[len] = calculate_probabilities(N, sequence, len);
    }

    // Запись результатов
    for (int len : lengths) {
        file << len << ";";
        cout << setw(10) << len;

        for (const string& combo : unique_combinations) {
            double probability = results[len][combo];
            file << fixed << setprecision(5) << probability << ";";
            cout << setw(10) << fixed << setprecision(5) << probability;
        }
        file << "\n";
        cout << "\n";
    }

    file.close();
}

int main() {
    vector<int> lengths = {10, 1000, 1000000};
    
    // Сохранение вероятностей комбинаций
    for (int N = 1; N <= 4; N++) {
        string filename = "probabilities_N" + to_string(N) + ".csv";
        save_to_csv(filename, N, lengths);
    }

    // Генерация ПСП и расчет автокорреляции
    bitset<1000000> sequence = generate_psp(1000000);
    save_autocorrelation_to_csv(sequence);

    cout << "\nГотово! Все результаты сохранены в CSV-файлы.\n";
    return 0;
}

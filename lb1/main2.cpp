#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>

using namespace std;

const size_t LFSR1_SIZE = 17;
const size_t LFSR2_SIZE = 109;

struct LFSR {
    uint32_t state;
    uint32_t tap;
    uint32_t size;
    
    LFSR(uint32_t initial_state, uint32_t tap_position, uint32_t register_size)
        : state(initial_state), tap(tap_position), size(register_size) {}
    
    bool step() {
        bool last_bit = state & 1;
        bool tap_bit = (state >> tap) & 1;
        bool new_bit = last_bit ^ tap_bit;
        state = (state >> 1) | (new_bit << (size - 1));
        return new_bit;
    }
};

class CombiningGenerator {
private:
    LFSR lfsr1;
    LFSR lfsr2;
    std::ofstream& csv_file;

public:
    CombiningGenerator(uint32_t init1, uint32_t tap1, uint32_t size1,
                      uint32_t init2, uint32_t tap2, uint32_t size2,
                      std::ofstream& csv)
        : lfsr1(init1, tap1, size1)
        , lfsr2(init2, tap2, size2)
        , csv_file(csv) {
        csv_file << "Step,LFSR1_State,LFSR1_Output,LFSR2_State,LFSR2_Output,Combined_Output\n";
    }
    
    bool generateBit(int step_num) {
        bool bit1 = lfsr1.step();
        bool bit2 = lfsr2.step();
        bool result = bit1 ^ bit2;

        csv_file << step_num << ","
                << std::bitset<32>(lfsr1.state) << ","
                << bit1 << ","
                << std::bitset<32>(lfsr2.state) << ","
                << bit2 << ","
                << result << "\n";
                
        return result;
    }
    
    std::vector<uint8_t> generateSequence(size_t num_bytes) {
        std::vector<uint8_t> sequence;
        sequence.reserve(num_bytes);
        
        for (size_t i = 0; i < num_bytes * 8; i += 8) {
            uint8_t byte = 0;
            for (int j = 0; j < 8; j++) {
                byte = (byte << 1) | generateBit(i + j);
            }
            sequence.push_back(byte);
        }
        return sequence;
    }
    
    void calculatePeriods() {
        uint64_t period1 = calculatePeriod(lfsr1);
        uint64_t period2 = calculatePeriod(lfsr2);
        uint64_t total_period = std::lcm(period1, period2);
        
        csv_file << "\nPeriods:\n";
        csv_file << "LFSR1 Period," << period1 << "\n";
        csv_file << "LFSR2 Period," << period2 << "\n";
        csv_file << "Total Period," << total_period << "\n";
    }

private:
    uint64_t calculatePeriod(LFSR& lfsr) {
        uint32_t initial_state = lfsr.state;
        uint64_t period = 0;
        
        do {
            lfsr.step();
            period++;
            if (period > (1ULL << lfsr.size)) return 0;
        } while (lfsr.state != initial_state);
        
        return period;
    }
};

void maskFile(const std::string& input_path, const std::string& output_path, 
              bool mask_header, size_t header_size, std::ofstream& csv_file) {
    LFSR lfsr1(0b1011010010101001, 7, 16);
    LFSR lfsr2(0b1101101010101011, 31, 108);
    
    ifstream input(input_path, ios::binary);
    ofstream output(output_path, ios::binary);
    
    if (!input || !output) {
        cerr << "Ошибка при открытии файлов!" << endl;
        return;
    }

    csv_file << "Byte_Position,Original_Byte,Mask_Byte,Masked_Byte\n";

    if (!mask_header && header_size > 0) {
        char* header = new char[header_size];
        input.read(header, header_size);
        output.write(header, header_size);
        delete[] header;
    }

    char byte;
    size_t position = 0;
    while (input.get(byte)) {
        uint8_t mask_byte = 0;
        for (int i = 0; i < 8; i++) {
            bool bit1 = lfsr1.step();
            bool bit2 = lfsr2.step();
            mask_byte = (mask_byte << 1) | (bit1 ^ bit2);
        }
        
        uint8_t masked_byte = byte ^ mask_byte;
        output.put(masked_byte);

        csv_file << position << ","
                << std::bitset<8>(static_cast<uint8_t>(byte)) << ","
                << std::bitset<8>(mask_byte) << ","
                << std::bitset<8>(masked_byte) << "\n";
        
        position++;
    }
    
    input.close();
    output.close();

    CombiningGenerator generator(0b1011010010101001, 7, 16, 
                               0b1101101010101011, 31, 108, 
                               csv_file);
    generator.calculatePeriods();
}

int main() {
    int choice;
    cout << "Выберите режим:\n"
         << "1. Генерация последовательности\n"
         << "2. Маскирование файла\n";
    cin >> choice;
    
    if (choice == 1) {
        uint32_t length;
        cout << "Введите длину последовательности: ";
        cin >> length;
        std::ofstream csv_file("Output.csv");
        if (!csv_file) {
            cerr << "Ошибка при создании CSV файла!\n";
            return 1;
        }
        CombiningGenerator generator(0b1011010010101001, 7, 16,
                                   0b1101101010101011, 31, 108,
                                   csv_file);
        std::vector<uint8_t> sequence = generator.generateSequence(length);
        csv_file.close();
        cout << "Результаты записаны в Output.csv" << endl;
    }
    else if (choice == 2) {
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
        
        std::ofstream csv_file("masking_log.csv");
        if (!csv_file) {
            cerr << "Ошибка при создании CSV файла!\n";
            return 1;
        }
        maskFile(input_file, output_file, mask_choice == 1, header_size, csv_file);
        csv_file.close();
        cout << "Маскирование завершено" << endl;
        cout << "Результаты записаны в masking_log.csv" << endl;
    } else {
        cout << "Неверный выбор\n";
        return 1;
    }
    
    return 0;
} 

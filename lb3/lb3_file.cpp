#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <vector>

using namespace std;

vector<uint8_t> readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    vector<uint8_t> data;
    uint8_t byte;
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        data.push_back(byte);
    }
    return data;
}

vector<uint8_t> processSequence(const vector<uint8_t>& inputData, const vector<uint8_t>& S, int groupSize, bool applyShift) {
    vector<uint8_t> sequence;
    for (uint8_t block : inputData) {
        uint8_t processedBlock = 0;
        for (int j = 0; j < groupSize; j += 4) {
            uint8_t fourBitValue = (block >> (groupSize - 4 - j)) & 0xF;
            processedBlock = (processedBlock << 4) | S[fourBitValue];
        }
        if (applyShift && groupSize == 8) {
            for (int shiftCount = 0; shiftCount < 3; shiftCount++) {
                uint8_t bit = processedBlock & 1;
                processedBlock >>= 1;
                processedBlock |= (bit << 7);
            }
        }
        sequence.push_back(processedBlock);
    }
    return sequence;
}

int main() {
    string filename;
    int groupSize;
    bool applyShift;
    cout << "Enter file name: ";
    cin >> filename;
    cout << "Enter the size of each group (4 or 8): ";
    cin >> groupSize;
    cout << "Enter operation? (1 - S', 0 - S*): ";
    cin >> applyShift;
    vector<uint8_t> inputData = readFile(filename);
    vector<uint8_t> S = {0x8, 0xE, 0x2, 0x5, 0x6, 0x9, 0x1, 0xC, 0xF, 0x4, 0xB, 0x0, 0xD, 0xA, 0x3, 0x7};
    vector<uint8_t> result = processSequence(inputData, S, groupSize, applyShift);
    cout << "Processed sequence: ";
    for (uint8_t val : result) {
        if (groupSize == 4) {
            cout << hex << uppercase << (val & 0xF) << " ";
        } else {
            cout << hex << uppercase << setw(2) << setfill('0') << (int)val << " ";
        }
    } cout << endl;
    return 0;
}
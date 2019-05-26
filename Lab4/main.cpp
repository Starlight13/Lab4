#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
#include <limits>
#include <unordered_map>

using namespace std;
using byte = unsigned char;
using bits_in_byte = bitset<size_t(8)>;
constexpr size_t BITS_PER_BYTE = numeric_limits<byte>::digits;


class archivator {
    ofstream output;
    ifstream input;
    
public:
    ~archivator();
    
    void decompress(string);
    string buffer;
    void fill_buffer();
    string fetch_bits(int);
    int bits_to_int(string);
    void zlwDecode(string &);
    
    void compress(char const **, int);
    string content_to_bits(string);
    string zlwEncode(string);
    int len(int);
    string to_binary_string(unsigned long int, int);
};

archivator::~archivator() {
    if (input.is_open())
        input.close();
    
    if (output.is_open())
        output.close();
}

void archivator::fill_buffer() {
    char c;
    buffer = "";
    
    while (!input.eof()) {
        input.get(c);
        buffer += bits_in_byte(byte(c)).to_string();
    }
}

void archivator::decompress(string path) {
    input.open(path, ios::binary);
    fill_buffer();
    
    string buff = "", newItem;
    string result = "";
    vector<string> dictionary;
    dictionary.push_back(buff);
    int i = 0, k = 0;
    
    buff = fetch_bits(1);
    dictionary.push_back(buff);
    result.append(dictionary[1]);
    
    while (!buffer.empty()) {
        for (int j = 0; j < pow(2, k) && !buffer.empty(); j++) {
            buff = fetch_bits(k + 2);
            i += k + 2;
            newItem = dictionary[bits_to_int(buff.substr(0, k + 1))];
            
            if (buff.size() > k + 1)
                newItem += buff.substr(k + 1);
            
            dictionary.push_back(newItem);
            result.append(newItem);
        }
        k++;
    }
    
    zlwDecode(result);
}

string archivator::fetch_bits(int lenght) {
    string result = buffer.substr(0, lenght);
    buffer.erase(0, lenght);
    
    return result;
}

int archivator::bits_to_int(string bits) {
    int result = 0, i = 0, size = bits.size();
    
    while (bits[i] == 0)
        i++;
    
    for (; i < size; i++)
        result += (int(bits[i]) - int('0')) *  int(pow(2, size - i - 1));
    
    return result;
}

void archivator::zlwDecode(string &bitstring) {
    ofstream out;
    string filename = "";
    int size_of_file;
    int i = 0, limit = bitstring.size() - bitstring.size() % 8, count = 0;
    char c;
    
    while (i + 8 < limit) {
        c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
        i += 8;
        
        while (c != '\a' && i + 8 < limit) {
            filename += c;
            c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
            i += 8;
        }
        
        cout << "Getting out file " << filename << "... ";
        count++;
        
        out.open(filename, ios::binary);
        
        size_of_file = bits_to_int(bitstring.substr(i, 64));
        i += 64;
        
        c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
        i += 8;
        size_of_file -= 8;
        
        while (size_of_file > 0) {
            out << c;
            c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
            i += 8;
            size_of_file -= 8;
        }
        
        cout << "Done." << endl;
        
        out.close();
        filename = "";
    }
    
    cout << count << " files written." << endl;
}

void archivator::compress(char const **argv, int argc) {
    string bitstring("");
    
    for (int i = 3; i < argc; i++) {
        cout << "Compressing file " << argv[i] << "... ";
        
        input.open(argv[i], ios::binary);
        bitstring += content_to_bits(argv[i]);
        input.close();
        
        cout << "Done." << endl;
    }
    
    bitstring = zlwEncode(bitstring);
    bitstring.erase(0, 1);
    
    while (bitstring.size() % BITS_PER_BYTE)
        bitstring += '0';
    
    output.open(argv[2], ios::binary);
    for (size_t i = 0; i < bitstring.size(); i += BITS_PER_BYTE) {
        byte b = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
        output << b;
    }
    
    cout << "Result written to " << argv[2] << endl;
}

string archivator::content_to_bits(string filename) {
    string buff(""), bitstring("");
    char c;
    
    for (size_t i = 0; i < filename.size(); i++) {
        bitstring += bits_in_byte(byte(filename.at(i))).to_string();
    }
    
    bitstring += bits_in_byte(byte('\a')).to_string();
    
    while (input.get(c)) {
        buff += bits_in_byte(byte(c)).to_string();
    }
    
    bitstring += to_binary_string(buff.size(), 64);
    bitstring.append(buff);
    
    return bitstring;
}

string archivator::zlwEncode(string bitstring) {
    string buff(""), result("");
    unordered_map<std::string, int> dictionary = {{"", 0}};
    
    for (int i = 0; i < bitstring.size(); i++) {
        if (dictionary.find(buff + bitstring.at(i)) != dictionary.end()) {
            buff += bitstring.at(i);
        } else {
            result += to_binary_string(dictionary[buff], len(dictionary.size()));
            result += bitstring.at(i);
            dictionary[buff + bitstring.at(i)] = dictionary.size();
            buff = "";
        }
    }
    
    if (buff != "")
        result += to_binary_string(dictionary[buff], len(dictionary.size()));
    
    return result;
}

int archivator::len(int number) {
    return ceil(log2(number));
}

string archivator::to_binary_string(unsigned long int n, int lenght) {
    string result = "", buff;
    
    do {
        buff = result;
        result = ('0' + (n % 2));
        result += buff;
        n = n / 2;
    } while (n > 0);
    
    while (result.size() < lenght)
        result.insert(0, "0");
    
    return result;
}

int main(int argc, const char * argv[]) {
    archivator arc;
    
    if (!(argc > 1))
        cout << "Not enough arguments";
    else if(!strncmp(argv[1], "--compress", 10)){
        if (argc >= 4)
            arc.compress(argv, argc);
        else
            cout << "Enter at 1 output file and at least 1 file to compress";
    } else if(!strncmp(argv[1], "--decompress", 12)){
        if (argc == 3)
            arc.decompress(argv[2]);
        else
            cout << "Enter only a file to decompress";
    } else
        cout << "Choose an option --compress/--decompress";
    
    return 0;
}

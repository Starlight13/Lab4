#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
#include <limits>
#include <unordered_map>
#include <map>

using namespace std;
using byte = unsigned char;
using bits_in_byte = bitset<size_t(8)>;
constexpr size_t BITS_PER_BYTE = numeric_limits<byte>::digits;


class archivator {
    ofstream output;
    ifstream input;
    
public:
    ~archivator();
    
    void compress(char const **, int);
    string content_to_bits(string);
    string zlwEncode(string);
    int len(int);
    string to_binary_string(unsigned long int, int);
    
    void decompress(int, char const**);
    int bits_to_int(string);
    string zlwDecode(string bitstring);
};

archivator::~archivator() {
    if (input.is_open())
        input.close();
    
    if (output.is_open())
        output.close();
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

int archivator::bits_to_int(string bits) {
    int result = 0, i = 0, size = bits.size();
    while (bits[i] == 0)
        i++;
    for (; i < size; i++)
        result += (int(bits[i]) - int('0')) *  int(pow(2, size - i - 1));
    
    return result;
}

string archivator::zlwDecode(string bitstring) {
    vector<int> str;
    cout << endl;
    for (int i = 0; i < bitstring.size(); i += BITS_PER_BYTE)
        str.push_back(bits_to_int(bitstring.substr(i, BITS_PER_BYTE)));
    
    std::__1::vector<int, std::__1::allocator<int> >::iterator begin = str.begin();
    std::__1::vector<int, std::__1::allocator<int> >::iterator end = str.end();
    
    int dictSize = 256;
    std::map<int,std::string> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[i] = std::string(1, i);
    
    std::string w(1, *begin++);
    std::string result = w;
    std::string entry;
    for ( ; begin != end; begin++) {
        int k = *begin;
        if (dictionary.count(k))
            entry = dictionary[k];
        else if (k == dictSize)
            entry = w + w[0];
        else
            throw "Bad compressed k";
        
        result += entry;
        
        dictionary[dictSize++] = w + entry[0];
        
        w = entry;
    }
    return result;
}

void archivator::decompress(int, char const**) {
    string bitstring("");
    string filename("lr5.doc");
    
    input.open(filename, ios::binary);
    char c;
    if (input) {
        
        bitstring = zlwDecode(bitstring);
        
    } else
        printf("[Could not open file %s]\n", filename.c_str());
    
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
            arc.decompress(argc, argv);
        else
            cout << "Enter only a file to decompress";
    } else
        cout << "Choose an option --compress/--decompress";
    
    return 0;
}

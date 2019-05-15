#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
#include <limits>
#include <map>

using namespace std;

using byte = unsigned char;
using bits_in_byte = bitset<size_t(16)>;
constexpr size_t BITS_PER_BYTE = 16;


class archivator {
private:
    string content;
    
    ifstream input;
    ofstream output;
public:
    void compress(int, char const**);
    string zlwEncode(string);
    void decompress(int, char const**);
    string zlwDecode(string bitstring);
    
    string itob(unsigned long int, int);
    int bits_to_int(string);
};

string archivator::itob(unsigned long int n, int lenght) {
    string result = "", buff;
    
    while (n > 0) {
        buff = result;
        result = ('0' + (n % 2));
        result += buff;
        n = n / 2;
    }
    while (result.size() < lenght) {
        result.insert(0, "0");
    }
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

string archivator::zlwEncode(string uncompressed) {
    string result = "";
    int dictSize = 256;
    std::map<std::string,int> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[std::string(1, i)] = i;
    
    std::string w;
    for (std::string::const_iterator it = uncompressed.begin();
         it != uncompressed.end(); ++it) {
        char c = *it;
        std::string wc = w + c;
        if (dictionary.count(wc))
            w = wc;
        else {
            result.append(itob(dictionary[w], BITS_PER_BYTE));
            dictionary[wc] = dictSize++;
            w = std::string(1, c);
        }
    }
    
    if (!w.empty())
        result.append(itob(dictionary[w], BITS_PER_BYTE));
    
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

void archivator::compress(int argc, const char **argv) {
    cout << "compressing...\n";
    string bitstring("");
    
    string filename("22.png");
    
    input.open(filename, ios::binary);
    char c;
    if (input) {
        for (size_t i = 0; i < filename.size(); i++)
            bitstring.append(bits_in_byte(byte(filename.at(i))).to_string());
        
        bitstring += bits_in_byte(byte('\a')).to_string();
        
        while (input.get(c)) {
            bitstring.append(bits_in_byte(byte(c)).to_string());
        }
        
        //        cout << endl << bitstring.size() << endl<< endl<< endl;
        
        bitstring = zlwEncode(bitstring);
        
        
        bitstring = zlwDecode(bitstring);
        
        //        cout << endl << bitstring.size();
        
        
        filename = "";
        int size = 0;
        bool isFilenameRecording = true;
        for (int i = 0; i < bitstring.size(); i += BITS_PER_BYTE) {
            char c = bits_to_int(bitstring.substr(i, BITS_PER_BYTE));
            
            if (isFilenameRecording) {
                filename += c;
                
                if (c == '\a') {
                    isFilenameRecording = false;
                    output.open("0"+filename, ios::binary);
                }
            } else {
                byte b = bits_to_int(bitstring.substr(i, BITS_PER_BYTE));
                output << b;
            }
            
        }
    } else
        printf("[Could not open file %s]\n", filename.c_str());
    
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


class Interface{
public:
    Interface(int argc, const char* argv[]){
        archivator arc;
        
        if (!(argc > 1)) {
            cout << "Not enough arguments";
            return;
        }
        else if(!strncmp(argv[1], "--compress", 10)){
            if(argc>=4)
                arc.compress(argc, argv);
            else{
                cout << "Enter at 1 output file and at least 1 file to compress";
                return;
            }
        }
        else if(!strncmp(argv[1], "--decompress", 12)){
            if(argc==3)
                arc.decompress(argc, argv);
            else{
                cout << "Enter only a file to decompress";
                return;
            }
        }
        else{
            cout << "Choose an option --compress/--decompress";
            return;
        }
        return;
    }
};



int main(int argc, const char * argv[]) {
    Interface(argc, argv);
    
}

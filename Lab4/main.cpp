#include <iostream>
#include <string>
using namespace std;

void compress(int argc,const char *argv[]){
    cout << "compress";
}
void decompress(int argc, const char *argv[]){
    cout << "decompress";
}

class Interface{
public:
    Interface(int argc, const char* argv[]){
        if (!(argc > 1)) {
            cout << "Not enough arguments";
            return;
        }
        else if(!strncmp(argv[1], "--compress", 10)){
            if(argc>=4)
                compress(argc, argv);
            else{
                cout << "Enter at 1 output file and at least 1 file to compress";
                return;
            }
        }
        else if(!strncmp(argv[1], "--decompress", 12)){
            if(argc==3)
                decompress(argc, argv);
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

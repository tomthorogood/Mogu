#include <iostream>
using std::cout;
using std::endl;


#include <fstream>
using std::ifstream;
using std::ofstream;

#define RANDOM_FILE "/dev/random"
#define OUTPUT_FILE "RANDOM_KEY"
#define KEY_BYTES 16
void random_key (unsigned char key[])
{
    ifstream f;
    f.open(RANDOM_FILE);

    for (int i =0; i < KEY_BYTES; i++)
    {
        unsigned char ch;
        f >> ch;
        key[i] = ch;
    }
    f.close();
}

void send_key (unsigned char key[])
{
    ofstream f;
    f.open(OUTPUT_FILE);
    const char* directive = "#define _BF_RANDOM_BYTE_";
    for (int i = 0; i < KEY_BYTES; i++)
    {
        int repr = (int) key[i];
        f << directive << i << " " << repr << '\n';
    }
    f << '\n';
    f.close();
}

/*
int main()
{
    unsigned char key[KEY_BYTES];
    random_key(key);
    send_key(key);
    size_t size = sizeof(key) * 8;
    cout << "Key size in bits: " << size << endl;
}
*/

#include <hash.h>
#include <iostream>
#include <string>
using namespace std;
int main (int argc, char** argv)
{
    const char* arg = argv[1];
    cout << Hash::toHash(arg) << endl;
    return 0;
}

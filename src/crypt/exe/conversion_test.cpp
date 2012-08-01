
#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;
using std::cin;
#include <string.h>
#include "BlowfishKey.h"

#include <stdlib.h>
#include <sstream>
using std::stringstream;


#define DBGPLZ(X) (cout << __LINE__ << ": " << (X) << endl)

#include <TurnLeftLib/Utils>
typedef unsigned char* CharArray;

CharArray strtouch (string in)
{
    char* copy = (char*) calloc(in.length(), sizeof(char));
    strcpy(copy, in.c_str());
    CharArray ucopy = 
        reinterpret_cast <CharArray> (copy);
    return ucopy;
}

string uchtostr (CharArray in)
{
    const char* c_in =
        reinterpret_cast <const char*> (in);
    return string(c_in);
}

string encstr (CharArray in)
{
    stringstream strm;
    for (int i = 0; i < 9; i++)
    {
        int ch = (int) in[i];
        strm << ch << " ";
    }
    return strm.str();
}

CharArray encarr (string in)
{
    stringstream strm;
    int len = in.length();
    CharArray u = (CharArray) calloc(9,1);
    int numset = 0;

    for (int i = 0; i < len; i++)
    {
        char ch = in.at(i);
        if (ch != ' ')
        {
            strm << ch;
        }
        else
        {
            string num = strm.str();
            int n = atoi(num.c_str());
            u[numset] = (unsigned char) n;
            numset++;
            strm.str("");
        }
    }
    return u;
}


int main()
{
    string input = "a cdefghijh";
    BlowfishKeyCreator keygen;
    BF_KEY* key = keygen.getKey();
    
    CharArray u_input = strtouch(input);
    DBGPLZ(u_input);    

    CharArray enc = (CharArray) calloc(9,1);
    CharArray dec = (CharArray) calloc(9,1);

    BF_ecb_encrypt(u_input, enc, key, BF_ENCRYPT);
    DBGPLZ(enc);

    string encrypted_str = encstr(enc);
    DBGPLZ(encrypted_str);

    CharArray enc2 = encarr(encrypted_str);
    DBGPLZ(enc2);

    BF_ecb_encrypt(enc2, dec, key, BF_DECRYPT);
    DBGPLZ(dec);

    string str = uchtostr(dec);
    DBGPLZ(str);

}



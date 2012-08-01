#include <openssl/blowfish.h>

typedef unsigned char* KeyArray;

class BlowfishKeyCreator
{
private:
    KeyArray key;
    BF_KEY* bf_key;
public:
    BlowfishKeyCreator();
    ~BlowfishKeyCreator();
    BF_KEY* getKey() const;
};

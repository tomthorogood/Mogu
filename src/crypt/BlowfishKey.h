#include <openssl/blowfish.h>

typedef unsigned char* KeyArray;

class BlowfishKeyCreator
{
private:
    KeyArray key;
    std::shared_ptr <BF_KEY> bf_key;
public:
    BlowfishKeyCreator();
    BlowfishKeyCreator(const BlowfishKeyCreator&);
    ~BlowfishKeyCreator();
    BF_KEY* getKey() const;
};

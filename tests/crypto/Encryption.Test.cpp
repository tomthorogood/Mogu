#include "../../src/Security/Encryption.h"
#include <cpptest.h>
#include <string>

class EncryptionTestSuite : public Test::Suite
{
public:
    EncryptionTestSuite()
    {
        TEST_ADD(EncryptionTestSuite::size8_alphanum);
        TEST_ADD(EncryptionTestSuite::size_12_alphanum);
        TEST_ADD(EncryptionTestSuite::size_26_mixed);
        TEST_ADD(EncryptionTestSuite::size_52_int);
        TEST_ADD(EncryptionTestSuite::size_16_int);
        TEST_ADD(EncryptionTestSuite::utf8);
    }

private:

    void size8_alphanum();
    void size_12_alphanum();
    void size_26_mixed();
    void size_52_int();
    void size_16_int();
    void utf8();

    inline std::string strdif(const std::string& a,const std::string& b)
    {
        int d = a.size()-b.size();
        std::string s = std::to_string(d);
        return s;
    }
};

void EncryptionTestSuite::size8_alphanum()
{
    std::string plain = "Testing1";
    std::string enc = Security::encrypt(plain);
    std::string dec = Security::decrypt(enc);


    std::string diff = strdif(enc,plain);
    
    TEST_ASSERT_MSG(diff=="8", diff.c_str());
    TEST_ASSERT_MSG(plain==dec, dec.c_str())
}

void EncryptionTestSuite::size_12_alphanum()
{
    std::string plain = "1Test23ing90";
    std::string enc = Security::encrypt(plain);
    std::string dec = Security::decrypt(enc);

    std::string diff = strdif(enc,plain);
    TEST_ASSERT_MSG(plain==dec, dec.c_str());
}

void EncryptionTestSuite::size_26_mixed()
{
    std::string plain = "{17}!1^49dhdak3l1^^()??jii";
    std::string enc = Security::encrypt(plain);
    std::string dec = Security::decrypt(enc);
    std::string diff = strdif(enc,plain);
    TEST_ASSERT_MSG(diff=="6", diff.c_str());
    TEST_ASSERT_MSG(plain==dec, dec.c_str());
}

void EncryptionTestSuite::size_52_int()
{
    std::string plain = "1234567890123456789012345678901234567890123456789012";
    std::string enc = Security::encrypt(plain);
    std::string dec = Security::decrypt(enc);
    std::string diff = strdif(enc,plain);
    TEST_ASSERT_MSG(plain==dec, dec.c_str());
}

void EncryptionTestSuite::size_16_int()
{
    std::string plain = "1234876512348765";
    std::string enc = Security::encrypt(plain);
    std::string dec = Security::decrypt(enc);
    std::string diff = strdif(enc,plain);
    TEST_ASSERT_MSG(diff=="8",diff.c_str());
    TEST_ASSERT_MSG(plain==dec, dec.c_str());
}

void EncryptionTestSuite::utf8()
{
    std::string plain = "Héllæ!";
    std::string enc = Security::encrypt(plain);
    std::string dec = Security::decrypt(enc);
    TEST_ASSERT_MSG(plain==dec, dec.c_str());
}

int main()
{
    Test::TextOutput output(Test::TextOutput::Verbose);
    EncryptionTestSuite ets;
    return ets.run(output,false) ? EXIT_SUCCESS : EXIT_FAILURE;
}

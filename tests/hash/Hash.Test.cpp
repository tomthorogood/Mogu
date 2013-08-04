#include "../../src/hash.h"
#include <string>
#include <cpptest.h>

class HashTestSuite : public Test::Suite
{
public:
    HashTestSuite()
    {
        TEST_ADD(HashTestSuite::test1);
        TEST_ADD(HashTestSuite::test2);
        TEST_ADD(HashTestSuite::test3);
    }
private:
    void test1()
    {
        std::string a = Hash::toHash("abcdefgh");
        std::string b = Hash::toHash("abcdefgh");
        TEST_ASSERT(a==b);
    }

    void test2()
    {
        std::string a = Hash::toHash("123456789");
        std::string b = Hash::toHash("123456789");
        TEST_ASSERT(a==b);
    }

    void test3()
    {
        std::string a = Hash::toHash("Once upon a time a long way away, I ate a baby.");
        std::string b = Hash::toHash("Once upon a time a long way away, I ate a baby.");
        TEST_ASSERT(a==b);
    }
};

int main()
{

    Test::TextOutput output(Test::TextOutput::Verbose);
    HashTestSuite hts;
    return hts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;

}


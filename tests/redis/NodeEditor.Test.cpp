#include <cpptest.h>
#include <src/Redis/NodeEditor.h>


#define ADD_TESTS(X)        \
    redis_connect();        \
    TEST_ADD(X::write);     \
    TEST_ADD(X::read);      \
    TEST_ADD(X::remove);    \
    TEST_ADD(X::cleanup);

#define ADD_SUITE(X) t.add(std::auto_ptr<Test::Suite>(new X));

template <class T>
std::string size_cmp (const T& a, const T& b)
{
    std::stringstream buf;
    buf << "A: " << a.size() << ", B: " << b.size() << std::endl;
    return buf.str();
}

class NETS : public Test::Suite
{
public:
    NETS (){}
    NETS(const std::string& node) : Test::Suite(), e(Prefix::meta, node) {}

protected:
    Redis::Node_Editor e {};
    Redis::Mogu_Query_Handler q {Prefix::meta};
    std::string sub {"sub"};
    virtual void redis_connect() =0;
    virtual void write()    =0;
    virtual void read()     =0;
    virtual void remove()   =0;
    virtual void cleanup()  =0;
};

class NETS_String : public NETS
{
public:
    NETS_String() : NETS("test_str") {ADD_TESTS(NETS_String);}
protected:
    virtual void write()
    {
        TEST_THROWS_NOTHING(e.set_type("string"));
        TEST_THROWS_NOTHING(e.write(success));
        TEST_ASSERT(e.node_exists());
    }
    virtual void read() override
    {
        std::string r {e.read()};
        TEST_ASSERT_MSG(r==success,r.c_str());
    }
    virtual void remove() override
    {
        TEST_ASSERT(e.remove());
    }

    virtual void cleanup() override
    {
        q.append_query("exists meta.test_str");
        TEST_ASSERT(!q.yield_response<bool>());
    }

    std::string success {"success"};
    virtual void redis_connect() {}
};

class NETS_Sub_String : public NETS_String
{
protected:
    virtual void redis_connect() override
    {
        NETS_String::redis_connect();
        e.set_sub(sub);
    }

    virtual void cleanup () override
    {
        q.append_query("exists meta.test_str.sub");
        TEST_ASSERT(!q.yield_response<bool>());
    }
};

class NETS_List : public NETS
{
public:
    NETS_List() : NETS("test_list") { ADD_TESTS(NETS_List) }
protected:
    virtual void write() override 
    {
        TEST_THROWS_NOTHING(e.set_type("list"));
        TEST_THROWS_NOTHING(e.write(test_list));
        TEST_ASSERT(e.node_exists());
    }
    virtual void read() override
    {
        std::vector <std::string> v {};
        e.read(v);
        bool match {v.size()==test_list.size()};
        std::string r {size_cmp(v,test_list)};
        TEST_ASSERT_MSG(match, r.c_str());
        if (!match) return;
        for (int i = 0; i < test_list.size(); ++i)
            TEST_ASSERT(v[i] == test_list[i]);
    }

    virtual void remove() override
    {
        TEST_ASSERT(e.remove());
    }

    virtual void cleanup() override
    {
        q.append_query("exists meta.test_list");
        TEST_ASSERT(!q.yield_response<bool>());
    }

    virtual void redis_connect() override {}

    std::vector <std::string> test_list {"A","B","C","D","E","F"};
};

class NETS_Sub_List : public NETS_List
{
protected:
    virtual void redis_connect()
    {
        NETS_List::redis_connect();
        e.set_sub(sub);
    }

    virtual void cleanup() final 
    {
        q.append_query("exists meta.test_list.sub");
        TEST_ASSERT(!q.yield_response<bool>());
    }
};

class NETS_List_Item : public NETS_List
{
protected:
    virtual void write() override
    {
        NETS_List::write();
        TEST_THROWS_NOTHING(e.write("G"));
        TEST_ASSERT(e.node_exists());
    }
    virtual void read() override
    {
        Node_Value arg {(int) test_list.size()};
        e.swap_arg(&arg);
        std::string r {e.read()};
        TEST_ASSERT_MSG(r=="G", r.c_str());
        e.swap_arg();
    }
    virtual void remove() override
    {
        TEST_ASSERT(e.remove("G"));
        q.execute_query("llen meta.test_list");
        TEST_ASSERT(q.yield_response<int>()==test_list.size());
        NETS_List::remove();
    }
};

class NETS_Hash : public NETS
{
public:
    NETS_Hash() : NETS("test_hash") { ADD_TESTS(NETS_Hash) }
protected:
    virtual void write() override
    {
        TEST_THROWS_NOTHING(e.set_type("hash"));
        TEST_THROWS_NOTHING(e.write(test_map));
        TEST_ASSERT(e.node_exists());
    }
    virtual void read() override 
    {
        std::map <std::string,std::string> m {};
        e.read(m);
        for (auto i : m)
        {
            TEST_ASSERT(i.second == test_map[i.first]);
        }
    }
    virtual void remove() override
    {
        TEST_ASSERT(e.remove());
    }
    virtual void cleanup() override
    {
        q.append_query("exists meta.test_hash");
        TEST_ASSERT(!q.yield_response<bool>());
    }

    virtual void redis_connect() override {}

    std::map<std::string,std::string> test_map
    {
        {"1","A"},
        {"2","B"},
        {"3","C"},
        {"4","D"},
        {"5","E"},
        {"6","F"}
    };
};

class NETS_Sub_Hash : public NETS_Hash
{
protected:
    virtual void redis_connect() override
    {
        NETS_Hash::redis_connect();
        e.set_sub(sub);
    }
    virtual void cleanup() override
    {
        q.append_query("exists meta.test_hash.sub");
        TEST_ASSERT(!q.yield_response<bool>());
    }
};

class NETS_Hash_Item : public NETS_Hash
{
protected:
    Node_Value arg {"key"};
    std::string value {"value"};
    virtual void write() override
    {
        NETS_Hash::write();
        e.swap_arg(&arg);
        TEST_THROWS_NOTHING(e.write(value));
        e.swap_arg();
    }

    virtual void read() override
    {
        e.swap_arg(&arg);
        std::string r {e.read()};
        TEST_ASSERT_MSG(r==value,r.c_str());
        e.swap_arg();
    }

    virtual void remove()
    {
        e.swap_arg(&arg);
        TEST_ASSERT(e.remove());
        q.execute_query("hexists meta.test_hash key");
        TEST_ASSERT(!q.yield_response<bool>());
        e.swap_arg();
        NETS_Hash::remove();
    }
};

bool run()
{
    Test::TextOutput o(Test::TextOutput::Verbose);
    Test::Suite t {};
    ADD_SUITE(NETS_String);
    ADD_SUITE(NETS_Sub_String);
    ADD_SUITE(NETS_List);
    ADD_SUITE(NETS_Sub_List);
    ADD_SUITE(NETS_List_Item);
    ADD_SUITE(NETS_Hash);
    ADD_SUITE(NETS_Sub_Hash);
    ADD_SUITE(NETS_Hash_Item);
    return t.run(o);
}

int main()
{
    return run() ? EXIT_SUCCESS : EXIT_FAILURE;
}

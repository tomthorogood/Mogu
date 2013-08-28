#include <cpptest.h>
#include <src/Redis/QueryHandler/QueryHandler.h>


std::string responseType(int i)
{   
    switch(i)
    {
        case 1: return "STRING";
        case 2: return "ARRAY";
        case 3: return "INT";
        case 4: return "NIL";
        case 5: return "STATUS";
        default: return std::to_string(i);
    };
}

class QueryHandlerTestSuite : public Test::Suite
{
public:
    QueryHandlerTestSuite()
    {
        context = new Redis::Context();
        context->host="localhost";
        context->db_num = 0;
        context->port=6379;


        TEST_ADD(QueryHandlerTestSuite::single_response);
        TEST_ADD(QueryHandlerTestSuite::multi_response);
        TEST_ADD(QueryHandlerTestSuite::multi_flush);
        TEST_ADD(QueryHandlerTestSuite::map_fill);
        TEST_ADD(QueryHandlerTestSuite::combined_use);
        TEST_ADD(QueryHandlerTestSuite::multiple_handlers);
        TEST_ADD(QueryHandlerTestSuite::errors_handled);
        TEST_ADD(QueryHandlerTestSuite::set_delete_hkey);
    }

    ~QueryHandlerTestSuite() 
    {
        delete context;
    }

protected:
    virtual void setup() {
        Redis::Query_Handler* qh = spawnHandler(); 
        qh->execute_query("flushall");
        delete qh;
    }

private:
    Redis::Context* context;
    inline Redis::Query_Handler* spawnHandler() {
        return new Redis::Query_Handler(context);
    }
    void single_response();
    void multi_response();
    void multi_flush();
    void map_fill();
    void combined_use();
    void multiple_handlers();
    void errors_handled();
    void set_delete_hkey();

};

void QueryHandlerTestSuite::single_response()
{
    Redis::Query_Handler* qh = spawnHandler();

    qh->execute_query("set foo bar");
    TEST_ASSERT_MSG(qh->last_response_type()==REDIS_REPLY_STATUS,
        responseType(qh->last_response_type()).c_str())
    std::string response = qh->yield_response<std::string>();
    TEST_ASSERT_MSG(response=="OK", response.c_str());
    qh->execute_query("del foo");
    int iresponse = qh->yield_response<int>();
    TEST_ASSERT(iresponse==1);
    delete qh;
}

void QueryHandlerTestSuite::multi_response()
{
    Redis::Query_Handler* qh = spawnHandler();

    qh->append_query("set %s %s", "foo", "bar");
    qh->append_query("del %s", "foo");
    qh->append_query("hset %s %s %s", "foo", "bar", "baz");
    qh->append_query("hset %s %s %s", "foo", "qux", "quux");
    qh->append_query("hget %s %s", "foo","bar");
    qh->append_query("hget %s %s", "foo", "qux");
    qh->append_query("del %s", "foo");
    qh->append_query("set foo 7");
    qh->append_query("get foo");
    qh->append_query("del foo");

    std::string reply_set_string = qh->yield_response<std::string>();
    TEST_ASSERT_MSG(qh->last_response_type()==REDIS_REPLY_STATUS,
        responseType(qh->last_response_type()).c_str() );

    std::string reply_del_string = qh->yield_response<std::string>();
    TEST_ASSERT(qh->last_response_type()==REDIS_REPLY_INTEGER);
    int reply_hset_1 = qh->yield_response<int>();
    int reply_hset_2 = qh->yield_response<int>();
    std::string reply_hget_1 = qh->yield_response <std::string>();
    std::string reply_hget_2 = qh->yield_response <std::string>();
    int reply_del_hash = qh->yield_response<int>();
    std::string reply_set_int = qh->yield_response <std::string>();
    int reply_get_int = qh->yield_response <int>();
    int reply_del_int = qh->yield_response <int>();

    TEST_ASSERT_MSG(reply_set_string=="OK", reply_set_string.c_str());
    TEST_ASSERT_MSG(reply_del_string=="1", reply_del_string.c_str());
    TEST_ASSERT(reply_hset_1==1);
    TEST_ASSERT(reply_hset_2==1);
    TEST_ASSERT(reply_hget_1=="baz");
    TEST_ASSERT(reply_hget_2=="quux");
    TEST_ASSERT(reply_del_hash==1);
    TEST_ASSERT(reply_set_int=="OK");
    TEST_ASSERT(reply_get_int==7);
    TEST_ASSERT(reply_del_int==1);

    delete qh;
}

void QueryHandlerTestSuite::multi_flush()
{
    Redis::Query_Handler* qh = spawnHandler();

    qh->execute_query("dbsize");
    TEST_ASSERT(qh->yield_response<int>()==0);

    qh->append_query("rpush foo bar");
    qh->append_query("rpush foo baz");
    qh->append_query("rpush foo qux");
    qh->append_query("rpush foo quux");

    qh->flush();

    qh->append_query("llen foo");
    int sz = qh->yield_response<int>();
    TEST_ASSERT_MSG(sz==4, std::to_string(sz).c_str());
    qh->append_query("lrange foo 0 %d", sz);

    std::vector <std::string> v = qh->
        yield_response<std::vector<std::string>>();
    TEST_ASSERT_MSG(
        qh->last_response_type()==REDIS_REPLY_ARRAY,
        responseType(qh->last_response_type()).c_str());

    TEST_ASSERT_MSG(v.size()==4,
            std::to_string(v.size()).c_str());

    TEST_ASSERT(v[0]=="bar");
    TEST_ASSERT(v[1]=="baz");
    TEST_ASSERT(v[2]=="qux");
    TEST_ASSERT(v[3]=="quux");

    qh->execute_query("del foo");
    delete qh;
}

void QueryHandlerTestSuite::combined_use()
{
    Redis::Query_Handler* qh = spawnHandler();

    qh->append_query("hset foo bar baz");            //a1
    qh->append_query("hset foo qux quux");           //a2
    qh->execute_query("hexists foo bar");            //e1
    TEST_ASSERT(qh->yield_response<bool>()==false);  //-e1
    qh->append_query("hget foo bar");                //a3
    qh->append_query("hget foo qux");                //a4
    qh->execute_query("hget foo bar");               //e2
    TEST_ASSERT(qh->yield_response<bool>()==false);  //-e2
    TEST_ASSERT(qh->yield_response<int>()==1);       //-a1
    qh->execute_query("hget foo bar");               //e3
    TEST_ASSERT(qh->yield_response<std::string>()=="baz"); //-e3
    qh->flush();                                    //-a2,-a3,-a4
    TEST_ASSERT(qh->reply_string()=="quux");         //*a4
    qh->execute_query("del foo");

    delete qh;
}

void QueryHandlerTestSuite::map_fill()
{
    Redis::Query_Handler* qh = spawnHandler();
    qh->append_query("hset foo bar baz");
    qh->append_query("hset foo qux quux");
    qh->append_query("hgetall foo");
    qh->flush();
    std::map <std::string,std::string> m= qh-> yield_response<std::map<std::string,std::string>>();
    TEST_ASSERT(m["bar"] == "baz");
    TEST_ASSERT(m["qux"] == "quux");

    delete qh;
}

void QueryHandlerTestSuite::set_delete_hkey()
{
    Redis::Query_Handler* q {spawnHandler()};
    q->append_query("hset foo bar baz");
    q->append_query("hexists foo bar");
    q->append_query("hdel foo bar");
    q->append_query("hexists foo bar");
    
    TEST_ASSERT(q->yield_response<int>()==1);
    TEST_ASSERT(q->yield_response<bool>());
    TEST_ASSERT(q->yield_response<int>()==1);
    TEST_ASSERT(!q->yield_response<bool>());

}

void QueryHandlerTestSuite::multiple_handlers()
{
    Redis::Query_Handler* qh1 = spawnHandler();
    Redis::Query_Handler* qh2 = spawnHandler();

    qh1->append_query("set foo bar");            //1a1
    qh1->append_query("set bar baz");            //1a2
    qh2->append_query("set bar bop");            //2a1
    qh2->append_query("set fizz bang");          //2a2
    qh1->execute_query("get fizz");              //1e1

    std::string res_1e1 = qh1->yield_response<std::string>(); //-1e1
    TEST_ASSERT_MSG(res_1e1.empty(), res_1e1.c_str());
    qh1->flush();
    qh2->flush();
    qh1->execute_query("get fizz");
    TEST_ASSERT(qh1->yield_response<std::string>()=="bang");
    qh2->execute_query("get bar");
    qh1->execute_query("get bar");
    TEST_ASSERT(
        qh1->yield_response<std::string>() ==
        qh2->yield_response<std::string>());

    qh1->append_query("del foo");
    qh2->append_query("del bar");
    qh1->append_query("del fizz");
    qh2->append_query("dbsize");
    qh1->flush();
    qh2->flush();
    TEST_ASSERT(qh2->yield_response<int>()==0);

    delete qh1;
    delete qh2;
}

void QueryHandlerTestSuite::errors_handled()
{
    Redis::Query_Handler* qh = spawnHandler();
    qh->append_query("get foo");
    std::string r = qh->yield_response<std::string>();
    TEST_ASSERT_MSG(r=="",r.c_str());
    delete qh;
}

int main()
{
    Test::TextOutput output(Test::TextOutput::Verbose);
    QueryHandlerTestSuite qhts;
    return qhts.run(output,false) ? EXIT_SUCCESS : EXIT_FAILURE;
}

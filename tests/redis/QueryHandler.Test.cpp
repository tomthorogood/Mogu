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
    }

    ~QueryHandlerTestSuite() 
    {
        delete context;
    }

protected:
    virtual void setup() {
        Redis::QueryHandler* qh = spawnHandler(); 
        qh->executeQuery("flushall");
        delete qh;
    }

private:
    Redis::Context* context;
    inline Redis::QueryHandler* spawnHandler() {
        return new Redis::QueryHandler(context);
    }
    void single_response();
    void multi_response();
    void multi_flush();
    void map_fill();
    void combined_use();
    void multiple_handlers();
    void errors_handled();

};

void QueryHandlerTestSuite::single_response()
{
    Redis::QueryHandler* qh = spawnHandler();

    qh->executeQuery("set foo bar");
    TEST_ASSERT_MSG(qh->lastResponseType()==REDIS_REPLY_STATUS,
        responseType(qh->lastResponseType()).c_str())
    std::string response = qh->yieldResponse<std::string>();
    TEST_ASSERT_MSG(response=="OK", response.c_str());
    qh->executeQuery("del foo");
    int iresponse = qh->yieldResponse<int>();
    TEST_ASSERT(iresponse==1);
    delete qh;
}

void QueryHandlerTestSuite::multi_response()
{
    Redis::QueryHandler* qh = spawnHandler();

    qh->appendQuery("set %s %s", "foo", "bar");
    qh->appendQuery("del %s", "foo");
    qh->appendQuery("hset %s %s %s", "foo", "bar", "baz");
    qh->appendQuery("hset %s %s %s", "foo", "qux", "quux");
    qh->appendQuery("hget %s %s", "foo","bar");
    qh->appendQuery("hget %s %s", "foo", "qux");
    qh->appendQuery("del %s", "foo");
    qh->appendQuery("set foo 7");
    qh->appendQuery("get foo");
    qh->appendQuery("del foo");

    std::string reply_set_string = qh->yieldResponse<std::string>();
    TEST_ASSERT_MSG(qh->lastResponseType()==REDIS_REPLY_STATUS,
        responseType(qh->lastResponseType()).c_str() );

    std::string reply_del_string = qh->yieldResponse<std::string>();
    TEST_ASSERT(qh->lastResponseType()==REDIS_REPLY_INTEGER);
    int reply_hset_1 = qh->yieldResponse<int>();
    int reply_hset_2 = qh->yieldResponse<int>();
    std::string reply_hget_1 = qh->yieldResponse <std::string>();
    std::string reply_hget_2 = qh->yieldResponse <std::string>();
    int reply_del_hash = qh->yieldResponse<int>();
    std::string reply_set_int = qh->yieldResponse <std::string>();
    int reply_get_int = qh->yieldResponse <int>();
    int reply_del_int = qh->yieldResponse <int>();

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
    Redis::QueryHandler* qh = spawnHandler();

    qh->executeQuery("dbsize");
    TEST_ASSERT(qh->yieldResponse<int>()==0);

    qh->appendQuery("rpush foo bar");
    qh->appendQuery("rpush foo baz");
    qh->appendQuery("rpush foo qux");
    qh->appendQuery("rpush foo quux");

    qh->flush();

    qh->appendQuery("llen foo");
    int sz = qh->yieldResponse<int>();
    TEST_ASSERT_MSG(sz==4, std::to_string(sz).c_str());
    qh->appendQuery("lrange foo 0 %d", sz);

    std::vector <std::string> v = qh->
        yieldResponse<std::vector<std::string>>();
    TEST_ASSERT_MSG(
        qh->lastResponseType()==REDIS_REPLY_ARRAY,
        responseType(qh->lastResponseType()).c_str());

    TEST_ASSERT_MSG(v.size()==4,
            std::to_string(v.size()).c_str());

    TEST_ASSERT(v[0]=="bar");
    TEST_ASSERT(v[1]=="baz");
    TEST_ASSERT(v[2]=="qux");
    TEST_ASSERT(v[3]=="quux");

    qh->executeQuery("del foo");
    delete qh;
}

void QueryHandlerTestSuite::combined_use()
{
    Redis::QueryHandler* qh = spawnHandler();

    qh->appendQuery("hset foo bar baz");            //a1
    qh->appendQuery("hset foo qux quux");           //a2
    qh->executeQuery("hexists foo bar");            //e1
    TEST_ASSERT(qh->yieldResponse<bool>()==false);  //-e1
    qh->appendQuery("hget foo bar");                //a3
    qh->appendQuery("hget foo qux");                //a4
    qh->executeQuery("hget foo bar");               //e2
    TEST_ASSERT(qh->yieldResponse<bool>()==false);  //-e2
    TEST_ASSERT(qh->yieldResponse<int>()==1);       //-a1
    qh->executeQuery("hget foo bar");               //e3
    TEST_ASSERT(qh->yieldResponse<std::string>()=="baz"); //-e3
    qh->flush();                                    //-a2,-a3,-a4
    TEST_ASSERT(qh->replyString()=="quux");         //*a4
    qh->executeQuery("del foo");

    delete qh;
}

void QueryHandlerTestSuite::map_fill()
{
    Redis::QueryHandler* qh = spawnHandler();
    qh->appendQuery("hset foo bar baz");
    qh->appendQuery("hset foo qux quux");
    qh->appendQuery("hgetall foo");
    qh->flush();
    std::map <std::string,std::string> m= qh-> yieldResponse<std::map<std::string,std::string>>();
    TEST_ASSERT(m["bar"] == "baz");
    TEST_ASSERT(m["qux"] == "quux");

    delete qh;
}

void QueryHandlerTestSuite::multiple_handlers()
{
    Redis::QueryHandler* qh1 = spawnHandler();
    Redis::QueryHandler* qh2 = spawnHandler();

    qh1->appendQuery("set foo bar");            //1a1
    qh1->appendQuery("set bar baz");            //1a2
    qh2->appendQuery("set bar bop");            //2a1
    qh2->appendQuery("set fizz bang");          //2a2
    qh1->executeQuery("get fizz");              //1e1

    std::string res_1e1 = qh1->yieldResponse<std::string>(); //-1e1
    TEST_ASSERT_MSG(res_1e1.empty(), res_1e1.c_str());
    qh1->flush();
    qh2->flush();
    qh1->executeQuery("get fizz");
    TEST_ASSERT(qh1->yieldResponse<std::string>()=="bang");
    qh2->executeQuery("get bar");
    qh1->executeQuery("get bar");
    TEST_ASSERT(
        qh1->yieldResponse<std::string>() ==
        qh2->yieldResponse<std::string>());

    qh1->appendQuery("del foo");
    qh2->appendQuery("del bar");
    qh1->appendQuery("del fizz");
    qh2->appendQuery("dbsize");
    qh1->flush();
    qh2->flush();
    TEST_ASSERT(qh2->yieldResponse<int>()==0);

    delete qh1;
    delete qh2;
}

void QueryHandlerTestSuite::errors_handled()
{
    Redis::QueryHandler* qh = spawnHandler();
    qh->appendQuery("get foo");
    std::string r = qh->yieldResponse<std::string>();
    TEST_ASSERT_MSG(r=="",r.c_str());
    delete qh;
}

int main()
{
    Test::TextOutput output(Test::TextOutput::Verbose);
    QueryHandlerTestSuite qhts;
    return qhts.run(output,false) ? EXIT_SUCCESS : EXIT_FAILURE;
}

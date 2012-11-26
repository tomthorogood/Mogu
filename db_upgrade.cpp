#include <declarations.h>
#include <Static.h>
#include <Types/GroupManager.h>
#include <Parsers/TokenGenerator.h>
#include <TurnLeftLib/Utils/inlines.h>
#include <Redis/RedisCore.h>
#include <Redis/RedisUtils.h>

#define FREPLY freeReplyObject(reply)
#define SREPLY reply = (redisReply*)

using std::cout;
using std::endl;

int main()
{
    redisContext* redis = redisConnect("localhost", 6379);
    const std::string GROUP = __NODE_DEFAULT_MEMBERSHIP;
    const std::string RANK = "student";
    
    /* First, we need to get a list of all the users in the database */
    redisReply* reply = (redisReply*)
        redisCommand(redis, "hkeys %s", __NODE_SESSION_LOOKUP);
    Redis::strvector vec_userlogins;
    Redis::toVector(reply, vec_userlogins);

    cout << "Retrieved logins." << endl;

    for (size_t i = 0; i < vec_userlogins.size(); ++i)
    {
        const char* muid = vec_userlogins[i].c_str();
        SREPLY
            redisCommand(redis, "zadd %s %d %s", 
                    __NODE_DEFAULT_MEMBERSHIP, 0, muid);
        FREPLY;

        // Get the user's first session
        SREPLY
            redisCommand(redis, "hget %s %s", __NODE_SESSION_LOOKUP, muid);
            std::string prev = reply->str;
        FREPLY;
        std::string first = EMPTY;
        std::string session_meta;
        while (prev != "global")
        {
            first = prev;
            session_meta = "s.";
            session_meta += prev;
            session_meta += ".";
            session_meta += __META_HASH;
            const char* ch_session_meta = session_meta.c_str();
            SREPLY
                redisCommand(redis, "hget %s %s", ch_session_meta, __PREV_HASH);
                prev = reply->str;
            FREPLY;
        }
        // Add the group to s.[session].&meta&.&groups&
        std::string user_groups = session_meta + ".";
        user_groups += __GROUPS_HASH;
        SREPLY
            redisCommand(redis, "rpush %s %s", user_groups.c_str(), __DEFAULT_GROUP_HASH);
        FREPLY;
    }

    


    redisFree(redis);
    return 0;
}

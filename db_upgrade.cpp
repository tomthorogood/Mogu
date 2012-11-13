#include <declarations.h>
#include <Redis/RedisCore.h>
#include <Redis/RedisUtils.h>

#define FREPLY freeReplyObject(reply)
#define SREPLY reply = (redisReply*)

using std::cout;
using std::endl;

int main()
{
    redisContext* redis = redisConnect("localhost", 6379);
    
    /* First, we need to get a list of all the users in the database */
    redisReply* reply = (redisReply*)
        redisCommand(redis, "hkeys %s", __NODE_SESSION_LOOKUP);
    Redis::strvector enc_userlogins;
    Redis::toVector(reply, enc_userlogins);

    size_t num_users = enc_userlogins.size();

    cout << num_users << " users to fix." << endl;
    for (size_t i = 0; i < num_users; ++i)
    {
        /* Create a userid */
        std::string userlogin = enc_userlogins[i];

        cout << "Processing user " << i << endl;
        std::string userid = Redis::makeUniqueRChar(redis, __NODE_USERID_SET);

        cout << "Fixing " << userlogin << " -> " << userid << endl;

        /* Assign userid to encrypted userlogin */
        SREPLY    
            redisCommand(redis, "sadd %s %s", __NODE_USERID_SET, userid.c_str());
        FREPLY;

        SREPLY
            redisCommand(redis, "hset %s %s %s", __NODE_USERID_LOOKUP, userlogin.c_str(), userid.c_str());
        FREPLY;

        /* Get the session id associated with the userlogin */
        SREPLY
            redisCommand(redis, "hget %s %s", __NODE_SESSION_LOOKUP, userlogin.c_str());
            std::string last_session = reply->str;
        FREPLY;

        /* Assign the session id to the new userid */
        SREPLY
            redisCommand(redis, "hset %s %s %s", __NODE_SESSION_LOOKUP, userid.c_str(), last_session.c_str());
        FREPLY;

        /* Remove the previous entry */
        SREPLY
            redisCommand(redis, "hdel %s %s", __NODE_SESSION_LOOKUP, userlogin.c_str());
        FREPLY;
        
        /* Get the user salt associated with the userlogin */
        SREPLY
            redisCommand(redis, "hget %s %s", __NODE_SALT_LOOKUP, userlogin.c_str());
        std::string salt = reply->str;
        FREPLY;
        
        /* Assign the salt to thew new userid */
        SREPLY
            redisCommand(redis, "hset %s %s %s", __NODE_SALT_LOOKUP, userid.c_str(),salt.c_str());
        FREPLY;

        /* Remove the previous entry */
        SREPLY
            redisCommand(redis, "hdel %s %s", __NODE_SALT_LOOKUP, userlogin.c_str());
        FREPLY;

        /* Determine if there are auth string collisions for the userlogin */
        SREPLY
            redisCommand(redis, "hexists %s %s", __NODE_COLLISION_STR_LOOKUP, userlogin.c_str());
            bool exists = (bool) reply->integer;
        FREPLY;

        if (exists)
        {
            /* Get the collision */
            SREPLY
                redisCommand(redis, "hget %s %s", __NODE_COLLISION_STR_LOOKUP, userlogin.c_str());
                std::string collisions = reply->str;
            FREPLY;

            /* Assign the collision to the userid */
            SREPLY
                redisCommand(redis, "hset %s %s %s", __NODE_COLLISION_STR_LOOKUP, userid.c_str(), collisions.c_str());
            FREPLY;

            /* Remove the previous entry */
            SREPLY
                redisCommand(redis, "hdel %s %s", __NODE_COLLISION_STR_LOOKUP, userlogin.c_str());
            FREPLY;
        }

    }    
    redisFree(redis);
    return 0;
}

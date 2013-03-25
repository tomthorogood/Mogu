namespace Redis {

class QuerySet 
{
private:
    std::vector <uint8_t> queryflags;
    int flag_iter =-1; //<! Marks the flags for the last query sent.
    uint8_t     array_type = 0;
    int         reply_type = -1;

    redisContext* rdb;
    redisReply* reply;

    int         reply_int;
    std::string reply_string;
    std::vector <std::string> reply_array_str;
    std::vector <int> reply_array_int;

    void* vreply;
    const char* host;
    int port;

    /*! Executes the next command sent to Redis, and
     * increases the flag iterator. 
     */
    inline redisReply* getNextReply() {
        redisGetReply(rdb,&vreply);
        reply = (redisReply*) vreply; //!< in cpp, have to explicitly cast this.
        ++flag_iter;
        return reply;
    }

    inline uin8_t responseFlags() {
        return queryflags[flag_iter];
    }

    /*!\brief Based on the type of reply received from Redis,
     * sets the correct variable to hold the state, and
     * then releases the reply object.
     */
    inline void assignReply() {
        reply_type = reply->type;
        
        reply_int = 0;
        reply_string = "";
        reply_array_int.clear();
        reply_array_str.clear();

        switch(reply_type)
        {
            case REDIS_REPLY_INTEGER: 
                reply_int = reply->integer;
                break;
            case REDIS_REPLY_STRING:  
                reply_string = reply->str;
                break;  
            case REDIS_REPLY_ARRAY:
                assignArray();
                break;
            default:
                break;
        };
        freeReplyObject(reply);
    }

    /*!\brief Iterates through the multi bulk reply object, assigning
     * the elements to the proper vector. The vector is chosen based
     * upon the type of the first element. All other elements are coerced
     * into the proper type, as long as they are either an integer or string.
     * Otherwise, the elements are ignored.
     */
    inline void assignArray() {
        int num_elements = reply->elements;
        int first_element_type = reply->element[0]->type;
        for (int e = 0; e < num_elements; ++e) {
            switch (first_element_type) {
                case REDIS_REPLY_INTEGER:
                    if (reply->element[e]->type == REDIS_REPLY_INTEGER) {
                        reply_array_int.push_back(reply->element[e]->integer);
                    }
                    else if (reply->element[e]->type == REDIS_REPLY_STRING) {
                        reply_array_int.push_back(std::atoi(reply->element[e]->str));
                    }
                    break;
                case REDIS_REPLY_STRING:
                    if (reply->element[e]->type == REDIS_REPLY_STRING) {
                        reply_array_str.push_back(reply->element[e]->str);
                    }
                    else if (reply->element[e]->type == REDIS_REPLY_INTEGER) {
                        reply_array_str.push_back(std::to_string(reply->element[e]->integer));
                    }
                    break;
            };
        }
    }

public:
    static const int IGNORE_RESPONSE    = 1;
    static const int REQUIRE_INT        = 2;
    static const int REQUIRE_STRING     = 4;
    static const uint8_t ARRAY_TYPE_INT     = 0;
    static const uint8_t ARRAY_TYPE_STR     = 1;
    
    //TODO set flag_iter to 0
    QuerySet(const char* host, int port);
    ~QuerySet();

    inline void appendQuery(std::shared_ptr<Query> query,uint8_t flags=0) {
        redisvAppendCommand(rdb, query->querystring, query->args);
        queryflags.push_back(flags);
    }
    inline bool hasQueue() { return flag_iter < queryflags.size(); }

    /*!\brief If no type is expected, the default is to store
     * the value of the reply object, but make no returns. 
     * Will continue to execute statements until a reply is expected.
     */
    template <> void yieldResponse() {
        do {
            getNextReply();
        } while ( (queryflags[flag_iter] & IGNORE_RESPONSE) && hasQueue() );
        assignReply();
    }

    /*!\brief Forces the return of an integer response, and continues
     * to execute redis commands until the response is not ignored. 
     * If the REQUIRE_INT flag is set, and the response is a string,
     * a '0' will always be returned.
     */
    template <int> int yieldResponse() {
        do {
            getNextReply();
        } while ( (queryflags[flag_iter] & IGNORE_RESPONSE) && hasQueue() );

        assignReply();
        
        if (reply_type == REDIS_REPLY_STRING) {
            if (queryflags[flag_iter] & REQUIRE_INT) return 0;
            return std::atoi(reply_str.c_str());
        }
        return reply_int;
    }

    /*!\brief Forces the return of a string response, and
     * continues to execute redis commands until the response is
     * not ignored. If the REQUIRE_STR flag is set, and the response
     * is an integer, the empty string will be returned.
     */
    template <std::string> std::string yieldResponse() {
        do {
            getNextReply();
        } while ( (queryflags[flag_iter] & IGNORE_RESPONSE) && hasQueue() );
        assignReply();

        if (reply_type == REDIS_REPLY_INTEGER) {
            if (queryflags[flag_iter] & REQUIRE_STR) return "";
            return std::to_string(reply_int);
        }
        return reply_str;
    }

    template <bool> bool yieldResponse() {
        do {
            getNextReply();
        } while ((queryflags[flag_iter] & IGNORE_RESPONSE) && hasQueue() );
        assignReply();
        
        if (reply_type == REDIS_REPLY_STRING) {
            if (queryflags[flag_iter] & REQUIRE_INT) return false;
            if (reply_string != "") return true;
            return false;
        }

        else if (reply_type == REDIS_REPLY_INTEGER) {
            return bool(reply_int);
        }

        else if (reply_type == REDIS_REPLY_ARRAY) return true;

        else return false;
    }

    template <std::vector <int>> yieldResponse() {
        do {
            getNextReply();
        } while ((queryflags[flag_iter] & IGNORE_RESPONSE) && hasQueue() );
        assignReply();
        return reply_array_int;
    }

    template <std::vector <std::string>> yieldResponse() {
        do {
            getNextReply();
        } while ((queryflags[flag_iter] & IGNORE_RESPONSE) && hasQueue() );
        assignReply();

        return reply_array_str;
    }

    inline int replyType() { return reply_type;}
    inline std::string replyString() { return reply_str;}
    inline int replyInt() { return reply_int;}
    inline std::vector& <int> replyIntArray() { return reply_array_int;}
    inline std::vector& <std::string> replyStringArray() { return reply_array_str;}
};

}//namespace Redis

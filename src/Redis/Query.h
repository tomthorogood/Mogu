namespace Redis {

class Query {
    public:
        const char* querystring;
        va_list args;
};

}//namespace Redis

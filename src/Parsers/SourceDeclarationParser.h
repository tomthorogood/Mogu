#ifndef SOURCE_DECLARATION_PARSER_H_
#define SOURCE_DECLARATION_PARSER_H_

#include <string>

enum class Source_Declaration_Type : unsigned char 
{
    invalid_type
    , group_members
    , alias
    , group_list
    , user_list
    , user_groups
    , static_list
};

class Source_Declaration_Parser
{
public:
    Source_Declaration_Parser(const std::string&);
    inline std::string& get_identifier() { return identifier; }
    inline Source_Declaration_Type get_type() const { return type; }
    inline int get_alias_type() const { return alias_type; }
    inline std::string get_data() const { return data; }

private:
    std::string s{};
    std::string identifier {};
    int alias_type {};
    Source_Declaration_Type type {Source_Declaration_Type::invalid_type};
    std::string data {};
};

#endif

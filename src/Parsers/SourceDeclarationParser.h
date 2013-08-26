#ifndef SOURCE_DECLARATION_PARSER_H_
#define SOURCE_DECLARATION_PARSER_H_

#include <string>

enum class Source_Declaration_Type : unsigned char 
{
    invalid_type
    , group_members
    , group_list
    , user_list
    , static_list
};

class Source_Declaration_Parser
{
public:
    Source_Declaration_Parser(const std::string&);
    inline std::string& get_identifier() { return identifier; }
    inline Source_Declaration_Type get_type() { return type; }

private:
    std::string s{};
    std::string identifier {};
    Source_Declaration_Type type {Source_Declaration_Type::invalid_type};
};

#endif

#include "SourceDeclarationParser.h"
#include "../Types/syntax.h"
#include <string.h>

template <typename T>
bool str_contains(const std::string& s, T v)
{
    return s.find(v) != std::string::npos;
}

Source_Declaration_Parser::Source_Declaration_Parser(const std::string& s)
{
    constexpr const char* group {Mogu_Syntax::group.str};
    constexpr const char* member {Mogu_Syntax::member.str};
    constexpr const char* user {Mogu_Syntax::user.str};
    constexpr const char* data {Mogu_Syntax::data.str};

    if (str_contains(s, member))
        type = Source_Declaration_Type::group_members;
    else if (str_contains(s, group))
    {
        type = Source_Declaration_Type::group_list;
        size_t identifier_index {strlen(group)+1};
        identifier = s.substr(identifier_index);
    }
    else if (str_contains(s,user))
    {
        type = Source_Declaration_Type::user_list;
        size_t identifier_index {strlen(user)+1};
        identifier = s.substr(identifier_index);
    }
    else if (str_contains(s, data))
    {
        type = Source_Declaration_Type::static_list;
        size_t identifier_index {strlen(data)+1};
        identifier = s.substr(identifier_index);
    }
}

#include "SourceDeclarationParser.h"
#include "NodeValueParser.h"
#include "../utilities.h"
#include "../Types/syntax.h"
#include <string.h>

Source_Declaration_Parser::Source_Declaration_Parser(const std::string& s)
{
    constexpr const char* group {Mogu_Syntax::group.str};
    constexpr const char* member {Mogu_Syntax::member.str};
    constexpr const char* user {Mogu_Syntax::user.str};
    constexpr const char* data_const {Mogu_Syntax::data.str};
    constexpr const char* prep {Mogu_Syntax::preposition.str};

    if (str_contains(s, member))
        type = Source_Declaration_Type::group_members;
    else if (str_contains(s, prep))
    {
        constexpr char space {' '};

        //[object] [identifier] [prep] [object]
        size_t obj_delim {s.find_first_of(space)};
        size_t id_delim {s.find_first_of(space, obj_delim+1)};
        size_t prep_delim {s.find_last_of(space)};
        std::string alias {trim(s.substr(prep_delim))};
        std::string reduceable {s.substr(0,id_delim)};
        
        Parsers::Node_Value_Parser p {};
        Node_Value v {};
        p.give_input(reduceable,v);
        data = trim(v.get_string());
        alias_type = atoi(alias.c_str());
        type = Source_Declaration_Type::alias;
    }
    else if (str_contains(s, group))
    {
        if (!str_contains(s, user))
        {
            type = Source_Declaration_Type::group_list;
            size_t identifier_index {strlen(group)+1};
            identifier = s.substr(identifier_index);
        }
        else
        {
            type = Source_Declaration_Type::user_groups;
        }
    }
    else if (str_contains(s,user))
    {
        type = Source_Declaration_Type::user_list;
        size_t identifier_index {strlen(user)+1};
        identifier = s.substr(identifier_index);
    }
    else if (str_contains(s, data_const))
    {
        type = Source_Declaration_Type::static_list;
        size_t identifier_index {strlen(data_const)+1};
        identifier = s.substr(identifier_index);
    }
}

#include "../Mogu.h"
#include "DynamicServer.h"
#include "../Types/SyntaxDef.h"
#include "../Config/inline_utils.h"

namespace{

template <class T>
bool str_contains(const std::string& s, T v)
{
    return s.find(v) != std::string::npos;
}

std::tuple<const Syntax_Def&,int> parse_sort_definition (
        const std::string& s)
{
    int sort_index {};
    size_t delim_index {s.find_first_of(" ")};
    if (delim_index!=std::string::npos)
    {
        std::string i {s.substr(delim_index+1)};
        sort_index = atoi(i.c_str());
        const Syntax_Def& y {Mogu_Syntax::get(s.substr(0,delim_index))};
        return std::make_tuple(y,sort_index);
    }
    const Syntax_Def& y {Mogu_Syntax::get(s)};
    return std::make_tuple(y,0);
}

}//anonymous namespace

std::vector <std::string> Dynamic_Server::fill_data_list (
        Redis::Node_Editor& e, Source_Declaration_Parser& p)
{
    std::vector <std::string> data_list {};
    Source_Declaration_Type source_type = p.get_type();
    if (source_type == Source_Declaration_Type::group_members)
    {
        Redis::Mogu_Query_Handler q {Prefix::group};
        q.execute_query("smembers group.%d.__meta__.m", group_id);
        data_list = q.yield_response<std::vector<std::string>>();
    }
    else if (source_type == Source_Declaration_Type::user_groups)
    {
        mApp;
        int u {app->get_user()};
        Redis::Mogu_Query_Handler q {Prefix::user};
        q.execute_query("smembers user.%d.__meta__.g", u);
        data_list = q.yield_response<std::vector<std::string>>();
    }
    else if (source_type == Source_Declaration_Type::alias)
    {
        data_list.push_back(p.get_data());
    }
    else
    {
        std::string identifier {p.get_identifier()};
        Prefix p {};
        int& id {user_id};
        bool requires_id {true};
        switch (source_type)
        {
            case Source_Declaration_Type::group_list:
                p = Prefix::group;
                id = group_id;
                break;
            case Source_Declaration_Type::user_list:
                p = Prefix::user;
                break;
            case Source_Declaration_Type::static_list:
                p = Prefix::data;
                requires_id = false;
                break;
            default: break;
        }
        Redis::Node_Editor e(p, identifier);
        if (requires_id) e.set_id(id);
        e.read(data_list);
    }
    return data_list;
}

void Dynamic_Server::sort(
        const Syntax_Def& direction, std::vector <Sortable>& v)
{
    auto sort_asc = [](const Sortable& a, const Sortable& b) { return a < b; };
    auto sort_des = [](const Sortable& a, const Sortable& b) { return b < a; };

    if (direction==Mogu_Syntax::decrement)
    {
        std::sort(v.begin(),v.end(),sort_asc);
    }
    else
    {
        std::sort(v.begin(),v.end(),sort_des);        
    }
}

std::string Dynamic_Server::get_template_name(const std::string& declaration)
{
    const static std::string using_ {Mogu_Syntax::preposition.str};
    const static std::string template_ {Mogu_Syntax::template_.str};

    if (!str_contains(declaration, using_))
        return "";
    // Get the index of "template"
    size_t tmpl_index {declaration.find(template_)};

    if (tmpl_index==std::string::npos) return "";

    size_t tmpl_name_start = tmpl_index+2;
    std::string sub = declaration.substr(tmpl_name_start);
    return trim(sub);

}

std::string Dynamic_Server::get_resolvable_string(const std::string& declaration)
{
    const static std::string using_ {Mogu_Syntax::preposition.str};
    size_t using_index {declaration.find(using_)};
    if (using_index==0) return "";
    return declaration.substr(0,using_index-1);
}

Widget_Assembly Dynamic_Server::spawn_anonymous_assembly(
    const std::string& alias_value
    , const std::string& anon_tmpl
    , const int& alias_type)
{
    mApp;
    Parsers::Node_Value_Parser& p = app->get_interpreter();
    std::string resolvable {get_resolvable_string(anon_tmpl)};
    std::string tmpl {get_template_name(anon_tmpl)};
    Widget_Assembly a {};
    Node_Value v {};
    a.node = create_unique_node_name();
    if (alias_type == Mogu_Syntax::user.integer)
    {
        p.set_user_id(atoi(alias_value.c_str()));
    }
    else if (alias_type == Mogu_Syntax::group.integer)
    {
        p.set_group_id(atoi(alias_value.c_str()));
    }
    p.give_input(resolvable, v);
    Attribute_Tuple t {merge_node_attributes("",tmpl)};
    a.children = std::get<0>(t);
    a.trigger_map = std::get<1>(t);
    a.attrdict = std::get<2>(t);
    a.attrdict[Mogu_Syntax::text.integer] = v;
    return a;

}

Widget_Assembly Dynamic_Server::spawn_anonymous_assembly
    ( const std::string& data_point
    , const std::string& anon_tmpl
    , const Source_Declaration_Type& type)
{
    mApp;
    Parsers::Node_Value_Parser& p = app->get_interpreter();


    std::string resolvable {get_resolvable_string(anon_tmpl)};
    std::string tmpl {get_template_name(anon_tmpl)};

    Widget_Assembly a {};
    Node_Value v{};
    a.node = create_unique_node_name();

    if (type == Source_Declaration_Type::group_members) 
    {
        p.set_user_id(atoi(data_point.c_str()));
    }
    else if (type == Source_Declaration_Type::user_groups)
    {
        p.set_group_id(atoi(data_point.c_str()));
    }
    else
    {
        if (type==Source_Declaration_Type::user_list)
            p.set_user_id(atoi(data_point.c_str()));
        else if (type==Source_Declaration_Type::group_list)
            p.set_group_id(atoi(data_point.c_str()));
        resolvable = data_point;
    }

    p.give_input(resolvable,v);
    Attribute_Tuple t {merge_node_attributes("",tmpl)};
    a.children = std::get<0>(t);
    a.trigger_map = std::get<1>(t);
    a.attrdict = std::get<2>(t);
    a.attrdict[Mogu_Syntax::text.integer] = v;
    return a;
}

Widget_Assembly Dynamic_Server::request
    ( const std::string& node
    , const int& u
    , const int& g)
{
    user_id = u;
    group_id = g;

    Widget_Assembly assembly {};
    Redis::Node_Editor wnode {Prefix::widget, node};
    std::string source_declaration {get_attribute(&wnode, Mogu_Syntax::source)};
    Source_Declaration_Parser sdp {source_declaration};
    Source_Declaration_Type source_type {sdp.get_type()};

    std::vector <std::string>&& data_list = fill_data_list(wnode,sdp);

    Attribute_Tuple&& t {
        merge_node_attributes(node, get_attribute(
            &wnode, Mogu_Syntax::template_))
    };

    Attribute_Map&& m {std::get<2>(t)};

    std::vector <std::string>&& child_declarations {std::get<0>(t)};
    size_t num_children {data_list.size()};

    std::tuple <const Syntax_Def&,int>&& sort_info 
        {parse_sort_definition(m[Mogu_Syntax::sort.integer])};

    std::vector <Sortable> sortable_containers {};

    for (size_t i = 0; i < num_children; ++i)
    {
        Sortable k {};
        std::string key {};

        std::string data_point {data_list[i]};

        Widget_Assembly& w = k.get_assembly();

        for (size_t j = 0; j < child_declarations.size(); ++j)
        {
            Widget_Assembly&& anon
            {
                source_type != Source_Declaration_Type::alias ?
                    spawn_anonymous_assembly(
                        data_point, child_declarations[j], source_type)
                    : spawn_anonymous_assembly(
                        data_point, child_declarations[j]
                        , sdp.get_alias_type())
            };

            local_cache.add(anon);

            if (j==(size_t) std::get<1>(sort_info))
                key = (std::string) anon.attrdict[Mogu_Syntax::text.integer];

            w.children.push_back(anon.node);
        }

        k.set_key(key);
        w.attrdict = m;
        w.node = create_unique_node_name();
        w.trigger_map = std::get<1>(t);
        local_cache.add(w);
        sortable_containers.push_back(k);
    }

    sort(std::get<0>(sort_info),sortable_containers);

    for (size_t i = 0; i < sortable_containers.size(); ++i)
    {
        assembly.children.push_back(sortable_containers[i].get_assembly().node);
    }

    return assembly;
}

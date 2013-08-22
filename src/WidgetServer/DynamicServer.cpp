#include "DynamicServer.h"

std::tuple<const Syntax_Def&,int> parse_sort_definition (const std::string& s)
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

std::vector <std::string> fill_data_list (Redis::Node_Editor& e, Source_Declaration_Parser& p)
{
    std::vector <std::string> data_list {};
    Source_Declaration_Type = p.get_type();
    if (source_type == Source_Declaration_Type::group_members)
    {
        Redis::Query_Handler q {Prefix::group};
        q.execute_query("smembers group.%d.__meta__.members", group_id);
        data_list = q.yield_response<std::vector<std::string>>();
    }
    else
    {
        std::string identifier {sdp.get_identifier()};
        Prefix p {};
        int& id {user_id};
        bool requires_id {true};
        switch (source_type)
        {
            case Source_Declaration_Type::group_list:
                p = Prefix::group;
                i = group_id;
                break;
            case Source_Declaration_Type::user_list:
                p = Prefix::user;
                break;
            case Source_Declaration_Type::static_list:
                p = Prefix::data;
                requires_id = false;
                break;
        }
        Redis::Node_Editor e(p, identifier);
        if (requires_id) e.set_id(id);
        e.read(data_list);
    }
    return data_list;
}

void Dynamic_Server::sort(Syntax_Def& direction, std::vector <Keyed_Assembly>& v)
{
    if (direction==Mogu_Syntax::decrement)
    {
        std::sort(v.begin(),v.end(),[](Keyed_Assembly& a, Keyed_Assembly &b) 
                { return b.get_key() < a.get_key()});
    }
    else
        std::sort(v.begin(),v.end(),[](Keyed_Assembly& a, Keyed_Assembly& b)
                { return a.get_key() < b.get_key()});        
}

Widget_Assembly spawn_anonymous_assembly
    ( const std::string& data_point
    , const std::string& anon_tmpl
    , const Source_Type_Declaration& type)
{
    mApp;
    Parsers::Node_Value_Parser& p {app->get_interpreter()};

    const std::string using_ {Mogu_Syntax::preposition.str};
    const std::string template_ {Mogu_Syntax::template_.str};

    std::string resolvable {anon_tmpl};
    std::string tmpl {};

    Widget_Assembly a {};
    Node_Value v{};
    a.node = create_unique_node_name();
    size_t using_index = anon_tmpl.find(Mogu_Syntax::preposition.str);

    if (using_index != std::string::npos)
    {
        size_t u {using_index + using_.size()};
        size_t q {u+1+template_.size()};
        size_t suffix_index {q+1};
        a.tmpl = anon_tmpl.substr(q);
        resolvable = anon_tmpl.substr(0,using_index);
    }

    if (type == Source_Declaration_Type::group_members) 
    {
        p.set_user_id(atoi(data_point.c_str()));
    }
    else if (type == Source_Declaration_Type::user_list)
    {
        p.set_user_id(user_id);
        resolvable = data_point;
    }
    else if (type == Source_Declaration_Type::group_list)
    {
        p.set_group_id(group_id);
        resolvable = data_point;
    }
    p.give_input(resolvable,v);
    Assembly_Tuple t {merge_attribute_nodes("",tmpl)};
    a.children = t.get<0>();
    a.triggers = t.get<1>();
    a.attrdict = t.get<2>();
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

    Widget_Assembly assembly;
    Redis::Node_Editor wnode {Prefix::widgets, node};
    std::string source_declaration {get_attribute(&wnode, Mogu_Syntax::source)};
    Source_Declaration_Parser sdp {source_declaration};
    Source_Declaration_Type source_type {sdp.get_type()};
    std::vector <std::string>&& data_list = fill_data_list(wnode,sdp);
    Attribute_Tuple&& t {merge_node_attributes(
            node, get_attribute(&wnode, Mogu_Syntax::template_))};
    Attribute_Map& m {t.get<2>};
    std::vector <std::string>& child_declarations {t.get<0>};

    std::tuple <const Mogu_Syntax&,int>&& sort_info 
        {parse_sort_definition(m[Mogu_Syntax::sort.integer])};

    std::vector <Keyed_Assembly> sortable_containers {data_list.size(), {}};
    for (int i = 0; i < sortable_containers.size(); ++i)
    {
        Keyed_Assembly& k {sortable_containers[i]};
        Widget_Assembly& w {k.get_assembly()};
        w.children.reserve(child_declarations.size(), "");
        std::string key {};
        std::string data_point {data_list[i]};
        for (int j = 0; j < child_declarations.size(); ++j)
        {
            Widget_Assembly&& anon {spawn_anonymous_assembly(
                        data_point, child_declarations[j], source_type)};
            local_cache.add(anon);
            if (j==sort_info.get<1>())
                key = anon.attrdict[Mogu_Syntax::text.integer];
            w.children[j] = anon.node;
        }
        k.set_key(key);
        w.attrdict = m;
        w.node = create_unique_node_name();
        w.triggers = t.get<1>();
        local_cache.add(w);
    }
    sort(sort_info.get<0>(),sortable_containers);
    assembly.children.reserve(sortable_containers.size(), "");

    for (int i = 0; i < sortable_containers.size(); ++i)
    {
        assembly.children[i] = sortable_containers[i].get_assembly().node;
    }

    return assembly;
}

#ifndef ABSTRACT_WIDGET_SERVER_H_
#define ABSTRACT_WIDGET_SERVER_H_

#include "../Types/WidgetAssembly.h"
#include "../Types/AssemblyCache.h"
#include "../Redis/NodeEditor.h"
#include "../Parsers/NodeValueParser.h"

#include <string>
#include <map>
#include <vector>
#include <type_traits>

class Widget_Server_Interface
{
public:
    using Attribute_Map = std::map<int,Node_Value>;
    using Attribute_Tuple = std::tuple <
        std::vector<std::string>
        , Trigger_Map
        , Attribute_Map >;
    Widget_Server_Interface (){}
    virtual ~Widget_Server_Interface() {}

    void populate_map(Redis::Node_Editor*, std::map<int,Node_Value>&);

    template <class T, class U>
    bool resolve_map_values(std::map<T,U>&);

    void unpack_node(
            Redis::Node_Editor*
            , std::vector<std::string>&
            , std::map <int, Node_Value>&);

    Attribute_Tuple merge_node_attributes(
        const std::string& node_name, std::string template_name="");

    virtual Widget_Assembly request 
        ( const std::string& node_name
        , const int& user_id=-1
        , const int& group_id=-1) =0;
    
    bool has_template (Redis::Node_Editor*, const std::string& node_name);

    inline Assembly_Cache& cache() { return local_cache; }

protected:

    int user_id {};
    int group_id {};

    inline std::string get_attribute(
            Redis::Node_Editor* node, const Syntax_Def& attr)
    {
        Node_Value arg {attr.str};
        node->swap_arg(&arg);
        std::string val {node->read()};
        node->swap_arg();
        return val;
    }

    std::string create_unique_node_name();
    
    Assembly_Cache local_cache {};
};

template <class T,class U>
bool Widget_Server_Interface::resolve_map_values(std::map<T,U>& m)
{
    static_assert(std::is_convertible<U,std::string>(), 
            "Map values must be convertible to string for use in NVP");
    bool cacheable {true};
    Parsers::Node_Value_Parser nvp {};
    nvp.set_user_id(user_id);
    nvp.set_group_id(group_id);
    for (auto i : m)
    {
        T k {i.first};
        std::string v { (std::string) i.second};
        Node_Value n {};
        nvp.give_input(v,n);
        m[k] = (U) n;
        if (cacheable) cacheable = !nvp.resolved_dynamic_value(); 
    }
    return cacheable;
}

#endif //ABSTRACT_WIDGET_SERVER_H_

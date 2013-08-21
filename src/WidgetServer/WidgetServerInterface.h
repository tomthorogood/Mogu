#ifndef ABSTRACT_WIDGET_SERVER_H_
#define ABSTRACT_WIDGET_SERVER_H_

#include "../Types/WidgetAssembly.h"

#include <string>
#include <map>
#include <vector>
#include <type_traits>
#include "../Redis/NodeEditor.h"
#include "../Parsers/Node_Value_Parser.h"

class Widget_Server_Interface
{
    using Assembly_Tuple = std::tuple <
        std::vector<std::string>
        , std::vector<std::string>
        , std::map<int,Node_Value>>;
public:
    Abstract_Widget_Server (){}

    void populate_map(Redis::Node_Editor*, std::map<std::string,std::string>&);

    template <class T, class U>
    void resolve_map_values(std::map<T,std::string>&);

    void unpack_node(
            Node_Editor*
            , std::vector<std::string>&
            , std::vector<std::string>&
            , std::map <int, Node_Value>&);

    Assembly_Tuple merge_node_attributes(
        const std::string& node_name, std::string template_name="");

    Widget_Assembly* request (const std::string& node_name) =0;

};

template <class T,class U>
void Abstract_Widget_Server::resolve_map_values(std::map<T,U>& m)
{
    static_assert(std::is_convertible<U,std::string>(), 
            "Map values must be convertible to string for use in NVP");

    Node_Value_Parser nvp {};
    for (auto i : m)
    {
        T k {i.first};
        std::string v { (std::string) i.second};
        Node_Value n {};
        nvp.give_input(v,n);
        m[k] = (U) n;
    }
}

#endif //ABSTRACT_WIDGET_SERVER_H_

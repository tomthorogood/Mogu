#include "WidgetServerInterface.h"
#include <sstream>
#include <TurnLeftLib/Utils/randomcharset.h>

void Widget_Server_Interface::populate_map(
        Redis::Node_Editor* e, std::map<int,Node_Value>& m)
{
    std::map <std::string,std::string> s {};
    e->read(s);
    for (auto key : s)
    {

        int i {Mogu_Syntax::get(key.first).integer};
        m[i] = Node_Value(key.second); 
    }
}

void Widget_Server_Interface::unpack_node(
    Redis::Node_Editor* e
    , std::vector <std::string>& children
    , std::map <int,Node_Value>& attributes)
{
    constexpr const char* ch {"children"};

    populate_map(e,attributes);
    e->set_sub(ch);
    e->read(children);
    e->clear_sub();
}

std::tuple 
   <std::vector<std::string>, Trigger_Map, std::map<int,Node_Value>>
Widget_Server_Interface::merge_node_attributes (
    const std::string& node_name, std::string template_name)
{
    Redis::Node_Editor w(Prefix::widgets, node_name);
    std::vector <std::string> w_children {}, t_children {};
    Attribute_Map attributes {};
    Trigger_Map *t_trigger_map {};

    if (!template_name.empty())
    {
        Redis::Node_Editor t(Prefix::templates, template_name);
        unpack_node(&t, t_children, attributes);
        t_trigger_map = new Trigger_Map {template_name, Prefix::templates};
    }

    Trigger_Map w_trigger_map {node_name, Prefix::widgets};
    unpack_node(&w, w_children, attributes);

    if (!w_children.size() && t_children.size())
        w_children = std::move(t_children);
    if (t_trigger_map) t_trigger_map->extend(w_trigger_map); 
    Trigger_Map&& m {*t_trigger_map};
    return std::make_tuple(w_children, m, attributes);
}

std::string Widget_Server_Interface::create_unique_node_name()
{
    std::stringstream buf;
    TurnLeft::Utils::RandomCharSet r {};
    do {
        buf << r.generate(1);
    } while (local_cache.cached(buf.str()));
    return buf.str();
}

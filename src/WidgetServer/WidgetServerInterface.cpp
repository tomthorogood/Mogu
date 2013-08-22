#include "AbstractWidgetServer.h"

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
    , std::vector <std::string>& triggers
    , std::map <int,Node_Value>& attributes)
{
    constexpr const char* ch {"children"};
    constexpr const char* ev {"events"};

    populate_map(attributes);
    e->set_sub(ch);
    e->read(children);
    e->set_sub(ev);
    e->read(triggers);
    e->clear_sub();
}

std::tuple <
    std::vector<std::string>, //children
    std::vector<std::string>, //triggers
    std::map<int,Node_Value>  //attributes
> Widget_Server_Interface::merge_node_attributes (
    const std::string& node_name, std::string template_name)
{
    Redis::Node_Editor w(Prefix::widgets, node_name);
    std::vector <std::string> w_children {}, t_children {};
    std::vector <std::string> w_triggers {}, t_triggers {};
    std::map <int, Node_Value> attributes {};

    if (!template_name.empty())
    {
        Redis::Node_Editor t(Prefix::templates, template_name);
        unpack_node(&t, t_chldren, t_triggers, attributes);
    }

    unpack_node(&w, w_children, w_triggers, attributes);

    if (!w_children.size() && t_children.size())
        w_children = std::move(t_children);
    if (!w_triggers.size() && t_triggers.size())
        w_triggers = std::move(t_triggers);

    return std::make_tuple(w_children, w_triggers, attributes);
}

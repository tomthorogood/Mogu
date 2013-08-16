#include <Mogu.h>
#include "WidgetServer.h"
#include <Types/WidgetAssembly.h>
#include <sstream>
#include <Redis/MoguQueryHandler.h>

void Widget_Server::populate_map(Redis::Node_Editor* e)
{
    std::map <std::string, std::string> node_map;
    e->read(node_map);
    for (auto key : node_map)
    {
        int i_key = Mogu_Syntax::get(key.first).integer;
        assembly->attrdict[i_key] = Node_Value(key.second);
    }
}

void Widget_Server::resolve_values(std::map <int, Node_Value>& map_)
{
    mApp;
    Parsers::Node_Value_Parser& nvp = app->get_interpreter();
    Node_Value resolved;
    for (auto iter : map_)
    {
        int attr = iter.first;
        std::string value = iter.second;
        nvp.give_input(
            value, resolved, Mogu_Syntax::get(member_context), &arg);
        if (resolved.is_string())
            map_[attr] = resolved.get_string();
        else if (resolved.is_int())
            map_[attr] = std::to_string(resolved.get_int());
        else
            map_[attr] = std::to_string(resolved.get_float());
    }
}

void Widget_Server::unpack_node(
    Redis::Node_Editor* e
    , std::vector <std::string>& triggers
    , std::vector <std::string>& children)
{
    const static std::string ch {"children"};
    const static std::string ev {"events"};

    populate_map(e);
    e->set_sub(ch);
    e->read(children);
    e->set_sub(ev);
    e->read(triggers);
    e->clear_sub();
}

void Widget_Server::setup_states()
{
    Node_Value arg {Mogu_Syntax::type.integer};
    if (!wnode)
        wnode = new Redis::Node_Editor {Prefix::widgets, assembly->node};
    else
        wnode->set_node(assembly->node);

    assembly->tmpl = get_attribute(wnode, Mogu_Syntax::template_);

    /* This will create a map of properties first from the template, if
     * it exists, and then overwriting the map with entries from the widget,
     * thereby giving the more abstracted node higher precedence, as it 
     * should be.
     */
    std::vector <std::string> tmpl_triggers {};
    std::vector <std::string> tmpl_children {};
    if (!assembly->tmpl.empty())
    {
        if (!tnode)
            tnode = new Redis::Node_Editor(Prefix::templates, assembly->tmpl);
        else
            tnode->set_node(assembly->tmpl);

        unpack_node(tnode, tmpl_triggers, tmpl_children);        
    }

    unpack_node(wnode, assembly->children, assembly->triggers);

    /* Use the triggers from the template if one for the widget does
     * not exist.
     */
    if (tmpl_triggers.size() > 0 && assembly->triggers.size() == 0)
    {
        assembly->triggers = tmpl_triggers;
        assembly->set_trigger_prefix(Prefix::templates);
    }

    /* Use the triggers from the widget */
    else
    {
        assembly->set_trigger_prefix(Prefix::widgets);
    }

    /* Do the same thing with the children. */
    if (tmpl_children.size() > 0 && assembly->children.size() == 0)
    {
        assembly->children = tmpl_children;
    }

    std::string s = assembly->attrdict[Mogu_Syntax::type.integer];
    int t = Mogu_Syntax::get(s).integer;

    bool is_member_widget = t == Mogu_Syntax::member.integer;

    if (is_member_widget)
    {
        resolve_iter_values();
    }
    else
    {
        resolve_values(assembly->attrdict);
    }
}

Widget_Assembly* Widget_Server::request(const std::string& node)
{
    assembly = new Widget_Assembly();
    assembly->node= node;
    setup_states();
    return assembly;
}

void Widget_Server::resolve_iter_values()
{
    std::string s = assembly->attrdict[Mogu_Syntax::member.integer];
    member_context = Mogu_Syntax::get(s).integer;

    switch(member_context)
    {
    case Mogu_Syntax::user:
        get_values_from_user_ids();
        break;
    case Mogu_Syntax::list:
    case Mogu_Syntax::data:
        get_values_from_list_node();
        break;
    default:
        break;
    }
}

void Widget_Server::get_values_from_user_ids()
{
    const static std::string group_key {"__meta__.members"};
    const static std::string anon_base {"__anon__user__"};
    mApp;
    TurnLeft::Utils::RandomCharSet r;
    Redis::Node_Editor group {Prefix::group, group_key};
    std::vector <std::string> membership {};
    group.read(membership);
    for (std::string member : membership)
    {
        app->set_temporary_user(atoi(member.c_str()));
        Widget_Assembly anon_widget = *assembly;
        anon_widget.node = anon_base + member + r.generate(2);
        resolve_values(anon_widget.attrdict);
        assembly->anonymous_children.push_back(anon_widget);
    }
    app->unset_temporary_user();
}

int Widget_Server::get_max_iters(const std::string& node)
{
    std::stringstream buf;
    Prefix p = member_context == Mogu_Syntax::list.integer ?
        Prefix::user : Prefix::data;

    std::string s = (p == Prefix::user) ? "user" : "data";
    buf << "llen" << s << "." << node;
    std::string cmd = buf.str();

    Redis::Mogu_Query_Handler h {p};
    h.append_query(cmd);
    return h.yield_response<int>();
}

void Widget_Server::get_values_from_list_node()
{
    const static std::string anon_base {"__anon__ulist__"};
    TurnLeft::Utils::RandomCharSet r {};
    int max_iters = get_max_iters (
            extract_node_name(
                find_member_call (assembly->attrdict)));

    for (int i=0; i < max_iters; ++i)
    {
        arg.set_int(i);
        Widget_Assembly anon_widget = *assembly;
        anon_widget.node = anon_base + r.generate(4);
        resolve_values(anon_widget.attrdict);
        assembly->anonymous_children.push_back(anon_widget);
    }
}

std::string Widget_Server::find_member_call (std::map <int, Node_Value>& m)
{
    for (auto iter : m)
    {
        /* Don't count it if the string is quoted, and therefore not a command! */
        if ((iter.second.get_string().find("member") != std::string::npos)
            && iter.second.get_string()[0] != '"')
            return iter.second;
    }
    return "";
}

Widget_Server::~Widget_Server()
{
    if (wnode) delete wnode;
    if (tnode) delete tnode;
}

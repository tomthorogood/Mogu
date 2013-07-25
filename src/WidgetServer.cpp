#include "WidgetServer.h"
#include <Types/WidgetAssembly.h>

WidgetServer::WidgetServer (Mogu* application)
    : application(application)
{

}


void WidgetServer::populateMap(Redis::NodeEditor* node)
{
    std::map <std::string, std::string> node_map;
    node->read(node_map);
    for (auto key : node_map)
    {
        int i_key = MoguSyntax::get(key.first).integer;
        assembly->attrdict[i_key] = NodeValue(key.second);
    }
}

void resolveValues(std::map <int, std::string>& map_)
{
    for (auto iter : map_)
    {
        std::string value = iter.second;
        if (isMemberCall(value))
        nvp.giveInput(value,resolved,memberContext, arg);
        if (resolved.isString())
        {
            map_[attr] = resolved.getString();
        }
        else if (resolved.isInt())
        {
            map_[attr] = std::to_string(resolved.getInt());
        }
        else
        {
            map_[attr] = std::to_string(resolved.getFloat());
        }
    }
}

void WidgetServer::setupStates()
{
    const static std::string children = "children";
    const static std::string events = "events";
    NodeValue arg((int)MoguSyntax::type);

    if (wnode == NULL)
        wnode = new Redis::NodeEditor(Prefix::widgets, assembly->node);
    else
        wnode->setNode(assembly->node);

    assembly->tmpl = getAttribute(wnode, MoguSyntax::template_);

    /* This will create a map of properties first from the template, if
     * it exists, and then overwriting the map with entries from the widget,
     * thereby giving the more abstracted node higher precedence, as it 
     * should be.
     */
    std::vector <std::string> tmpl_triggers;
    std::vector <std::string> tmpl_children;
    if (!assembly->tmpl.empty())
    {
        if (tnode == NULL)
            tnode = new Redis::NodeEditor(Prefix::templates, assembly->tmpl);
        else
            tnode->setNode(assembly->tmpl);

        populateMap(tnode);
        tnode->setSub(children);
        tnode->read(tmpl_children);
        tnode->setSub(events);
        tnode->read(tmpl_triggers);
        tnode->clearSub();
        
    }

    populateMap(wnode);
    wnode->setSub(children);
    wnode->read(assembly->children);
    wnode->setSub(events);
    wnode->read(assembly->triggers);
    wnode->clearSub();

    /* Use the triggers from the template if one for the widget does
     * not exist.
     */
    if (assembly->triggers.size() == 0 && tmpl_triggers.size() > 0)
    {
        assembly->triggers = tmpl_triggers;
        assembly->setTriggerPrefix(Prefix::templates);
    }

    /* Use the triggers from the widget */
    else
    {
        assembly->setTriggerPrefix(Prefix::widgets);
    }

    /* Do the same thing with the children. */
    if (assembly->children.size() == 0 && tmpl_children.size() > 0)
    {
        assembly->children = tmpl_children;
    }

    int assemblyType = MoguSyntax::get(
            assembly->attrdict[MoguSyntax::type.integer]).integer;

    bool isMemberWidget = assemblyType == MoguSyntax::member.integer;

    if (isMemberWidget)
    {
        resolveIterValues();
    }
    else
    {
        resolveValues(assembly->attrdict);
    }
}

WidgetAssembly* WidgetServer::request(const std::string& node)
{
    assembly = new WidgetAssembly();
    assembly->node= node;
    setupStates();
    if (assembly->attrdict[MoguSyntax::type.integer] == MoguSyntax::member)
    {
        createAnonymousWidgets();
    }
    return assembly;
}

void WidgetServer::resolveIterValues()
{
    memberContext = MoguSyntax::get(
        assembly->attrdict[MoguSyntax::member.integer]).integer;

    switch(memberContext)
    {
        case MoguSyntax::user.integer:
            getValuesFromUserIds();
            break;
        case MoguSyntax::list.integer:
        case MoguSyntax::data.integer:
            getValuesFromListNode();
            break;
        default:
            break;
    }
}

void WidgetServer::getValuesFromUserIds()
{
    mApp;
    TurnLeft::Utils::RandomCharSet rchar;
    const static std::string group_key = "__meta__.members";
    const static std::string anon_base = "__anon__user__"
    Redis::NodeEditor group(Prefix::group, group_key);
    std::vector <std::string> membership;
    group.read(membership);
    int orig_id = app->getUser();
    for (std::string member : membership)
    {
        app->setUser(atoi(member.c_str()));
        WidgetAssembly anon_widget = *assembly;
        anon_widget.node = anon_base + member + rchar.generate(2);
        resolveValues(anon_widget.attrdict);
        assembly->anonymous_children.push_back(anon_widget);
    }
    app->setUser(orig_id);
}

int WidgetServer::getMaxIters(const std::string& node)
{
    std::stringstream buf;
    Prefix prefix = (memberContext == MoguSyntax::list.integer) ? Prefix::user : Prefix::data;
    std::string s_prefix = (prefix == Prefix::user) ? "user" : "data";
    buf << "llen " << s_prefix < "." << node;
    std::string cmd = buf.str();
    Redis::QuerySet db(prefix);
    db.appendQuery(cmd);
    return db.yieldResponse <int>();
}

void WidgetServer::getValuesFromListNode()
{
    TurnLeft::Utils::RandomCharSet rchar;
    const std::string anon_base = "__anon__ulist__";
    int maxIters = getMaxIters(
            extractNodeName(
                findMemberCall(assembly->attrdict)));
    int iter = 0;
    while (iter < maxIters)
    {
        arg.setInt(iter);
        WidgetAssembly anon_widget = *assembly;
        anon_widget.node = anon_base + rchar.generate(4);
        resolveValues(anon_widget.attrdict);
        assembly->anonymous_children.push_back(anon_widget);
        ++iter;
    }
}

std::string WidgetServer::findMemberCall(std::map <int, std::string>& map_)
{
    for (auto iter : map_)
    {
        /* Don't count it if the string is quoted, and therefore not a command! */
        if (iter.second.find("member") != std::string::npos && iter.second[0] != '"')
            return iter.second;
    }
    return "";
}

WidgetServer::~WidgetServer()
{
    if (wnode) delete wnode;
    if (tnode) delete tnode;
}

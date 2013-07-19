#include "WidgetServer.h"
#include <Types/WidgetAssembly.h>

WidgetServer::WidgetServer (Mogu* application)
    : __application(application)
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

void WidgetServer::setupStates()
{
    if (tnode)
        delete tnode;
    if (wnode)
        delete wnode;
    const static std::string children = "children";
    const static std::string events = "events";
    NodeValue arg((int)MoguSyntax::type);
    wnode = new Redis::NodeEditor(Prefix::widgets, assembly->node);
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
        tnode = new Redis::NodeEditor(Prefix::templates, assembly->tmpl);
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

    if (assembly->triggers.size() == 0 && tmpl_triggers.size() > 0)
    {
        assembly->triggers = tmpl_triggers;
        assembly->setTriggerPrefix(Prefix::templates);
    }
    else
    {
        assembly->setTriggerPrefix(Prefix::widgets);
    }
    if (assembly->children.size() == 0 && tmpl_children.size() > 0)
    {
        assembly->children = tmpl_children;
    }
}

WidgetAssembly* WidgetServer::request(const std::string& node)
{
    assembly = new WidgetAssembly();
    assembly->node= node;
    setupStates();
    return assembly;
}

WidgetServer::~WidgetServer()
{
    if (wnode) delete wnode;
    if (tnode) delete tnode;
}

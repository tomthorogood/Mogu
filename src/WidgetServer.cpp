#include "WidgetServer.h"

WidgetServer::WidgetServer (Mogu* application)
    : __application(application)
{

}

bool getAttribute(MoguSyntax attribute)
{
    return getAttribute(std::to_string((int) attribute));
}

bool getAttribute(std::string attribute)
{
    NodeValue arg(attribute);
    NodeValue result;
    wnode->setArg(&arg);
    result.setString(wnode->read());
    map[attribute] = result;
    return (std::string) result != EMPTY;
}

void populateMap(Redis::NodeEditor* node)
{
    std::map <std::string, std::string>& node_map = node->readAll();
    for (auto key : node_map)
    {
        int i_key = atoi(key.c_str());
        MoguSyntax m_key = (MoguSyntax) i_key;
        map[m_key] = Nodevalue(node_map[key]);
    }
}

WidgetMap readWidget(const std::string& node)
{
    map.clear();
    NodeValue arg(MoguSyntax::type);
    wnode = new Redis::NodeEditor(Prefix::widgets, node);
    bool has_tmpl = getAttribute(MoguSyntax::template_);

    if (has_tmpl)
    {
        tnode = new Redis::NodeEditor(Prefix::templates, node);
        populateMap(tnode);
    }

    populateMap(wnode);    
}

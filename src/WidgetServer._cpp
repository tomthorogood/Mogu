#include "WidgetServer.h"

WidgetServer::WidgetServer (Mogu* application)
    : __application(application)
{

}

bool getAttribute(const SyntaxDef& attribute)
{
    NodeValue arg((int)attribute);
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
        const SyntaxDef& m_key = MoguSyntax::get(key);
        map[m_key] = Nodevalue(node_map[key]);
    }
}

WidgetMap readWidget(const std::string& node)
{
    map.clear();
    NodeValue arg((int)MoguSyntax::type);
    wnode = new Redis::NodeEditor(Prefix::widgets, node);
    bool has_tmpl = getAttribute(MoguSyntax::template_);

    if (has_tmpl)
    {
        tnode = new Redis::NodeEditor(Prefix::templates, node);
        populateMap(tnode);
    }

    populateMap(wnode);    
}

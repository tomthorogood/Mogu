#include <sstream>

#include "Mogu.h"
#include "WidgetServer.h"
#include "Types/WidgetAssembly.h"
#include "Redis/MoguQueryHandler.h"

namespace Application {
    extern Widget_Cache global_cache;
}

Widget_Assembly Widget_Server::request(const std::string& node)
{
    if (Application::global_cache.cached(node))
    {
        return Application::global_cache.get(node);
    }
    else if (local_cache.cached(node))
    {
        return local_cache.get(node);
    }
    else if (dynamic.cache().cached(node))
    {
        return dynamic.cache().get(node);
    }

    Widget_Assembly assembly;
    Redis::Node_Editor wnode {Prefix::widgets, node};
    std::string type {get_attribute(wnode, Mogu_Syntax::type)};
    
    if (type==Mogu_Syntax::container.str
        && !get_attribute(&wnode, Mogu_Syntax::source).empty())
    {
        return dynamic.request(node);
    }
    
    std::string tmpl {get_attribute(&wnode, Mogu_Syntax::template_)};

    Assembly_Tuple&& t {merge_node_attributes(node, tmpl)};

    assembly.node = node;
    assembly.tmpl = tmpl;
    assembly.children = t.get<1>();
    assembly.triggers = t.get<2>();
    assembly.attrdict = t.get<3>();

    return assembly;
}

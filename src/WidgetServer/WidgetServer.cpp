#include <sstream>

#include "Mogu.h"
#include "WidgetServer.h"
#include "../Types/WidgetAssembly.h"
#include "../Redis/MoguQueryHandler.h"
#include "../Types/AssemblyCache.h"

namespace Application { extern Assembly_Cache global_cache; }

Widget_Assembly Widget_Server::request(
        const std::string& node, const int& uid, const int& gid)
{
    user_id = uid;
    group_id = gid;
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
    std::string type {get_attribute(&wnode, Mogu_Syntax::type)};
    
    if ( (type==Mogu_Syntax::container.str || type==Mogu_Syntax::stack.str)
        && !get_attribute(&wnode, Mogu_Syntax::source).empty())
    {
        return dynamic.request(node, user_id, group_id);
    }
    
    std::string tmpl {get_attribute(&wnode, Mogu_Syntax::template_)};

    Assembly_Tuple&& t {merge_node_attributes(node, tmpl)};

    assembly.node = node;
    assembly.tmpl = tmpl;
    assembly.children = std::move(std::get<0>(t));
    assembly.trigger_map = std::move(std::get<1>(t));
    assembly.attrdict = std::move(std::get<2>(t));

    if (resolve_map_values(assembly.attrdict)) Application::global_cache.add(assembly);

    return assembly;
}

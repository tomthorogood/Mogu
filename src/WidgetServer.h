#ifndef WIDGETSERVER_H_
#define WIDGETSERVER_H_

#include <declarations.h>
#include <Redis/NodeEditor.h>
#include <Types/SyntaxDef.h>
#include <Types/WidgetAssembly.h>

/*\brief The WidgetServer acts as the intermediary between Redis and
 * the Mogu renderer, putting together a nice package to be passed into
 * the MoldableFactory, handling all the reslutions for widget templates and
 * foreach recursion.
 */
class WidgetServer 
{
public:
    typedef std::map <int, NodeValue> AttributeMap;

    WidgetServer (Mogu*);
    ~WidgetServer();

    WidgetAssembly* request(const std::string& node);
    
private:

    Redis::NodeEditor* wnode    =nullptr;
    Redis::NodeEditor* tnode    =nullptr;

    std::string stateful_node;
    std::string stateful_tmpl;

    WidgetAssembly* assembly;

    Mogu* application;

    void populateMap(Redis::NodeEditor*);
    void setupStates();

    inline std::string getAttribute(Redis::NodeEditor* node,
        const SyntaxDef& attribute)
    {
        NodeValue arg(attribute.str);
        node->swapArg(&arg);
        std::string attrValue = node->read();
        node->swapArg();
        return attrValue;
    }
};

#endif

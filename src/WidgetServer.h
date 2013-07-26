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

    /* This will always be the root widget of that which has been
     * requested.
     */
    WidgetAssembly* request(const std::string& node);

private:
    int memberContext   =0;

    Redis::NodeEditor* wnode    =nullptr;
    Redis::NodeEditor* tnode    =nullptr;
    NodeValue arg;
    bool list_iter_complete     =false;

    std::string stateful_node;
    std::string stateful_tmpl;

    WidgetAssembly* assembly;

    Mogu* application;

    void populateMap(Redis::NodeEditor*);
    void setupStates();
    void resolveIterValues();
    void resolveValues(std::map <int, NodeValue>&);
    void createAnonymousWidgets();
    void getValuesFromUserIds();
    void getValuesFromListNode();
    std::string findMemberCall(std::map <int, NodeValue>&);
    std::string extractNodeName(const std::string& input)
    {
        int nodename_start = input.find("member")+7;
        int nodename_end = input.find_first_of(" \0",nodename_start);
        return input.substr(nodename_start,
                (nodename_end-nodename_start));
    }

    inline std::string getAttribute(Redis::NodeEditor* node,
        const SyntaxDef& attribute)
    {
        NodeValue arg(attribute.str);
        node->swapArg(&arg);
        std::string attrValue = node->read();
        node->swapArg();
        return attrValue;
    }

    int getMaxIters(const std::string& node);



};

#endif

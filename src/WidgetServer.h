#ifndef WIDGETSERVER_H_
#define WIDGETSERVER_H_

#include <declarations.h>
#include <Redis/NodeEditor.h>

typedef std::map <MoguSyntax, NodeValue> WidgetMap;

class WidgetServer 
{
public:
    WidgetServer (Mogu*);
    ~WidgetServer();
private:
    NodeValue getAttribute(MoguSyntax);
    Redis::NodeEditor* wnode;
    Redis::NodeEditor* tnode;

    Mogu* __application;
    WidgetMap map;
}

#endif

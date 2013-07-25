#ifndef WIDGETASSEMBLY_H_
#define WIDGETASSEMBLY_H_

#include <map>
#include <string>
#include <vector>
#include <Events/TriggerMap.h>
#include "NodeValue.h"

struct WidgetAssembly
{
    std::map <int,NodeValue> attrdict;
    std::vector <std::string> children;
    std::vector <std::string> triggers;
    
    /* Anonymous children are those created due to 'member' templates that
     * don't actually live anywhere in the database. These will be assigned
     * names on the fly at runtime, and registered like any other widget, but
     * all interactions will be abstracted and handled by their templates.
     */
    std::vector <WidgetAssembly> anonymous_children;
    std::string node = "";
    std::string tmpl = "";
    TriggerMap* triggerMap = nullptr;

    void setTriggerPrefix(Prefix prefix)
    {
        std::string triggerNode = (prefix == Prefix::widgets) ?
            node : tmpl;
        if (triggerMap != nullptr) delete triggerMap;
        triggerMap = new TriggerMap(triggers.size(), prefix, triggerNode);
    }

    WidgetAssembly& operator=(const WidgetAssembly& other)
    {
        attrdict = other.attrdict;
        children = other.children;
        triggers = other.triggers;
        anonymous_children = other.anonymous_children;
        node = other.node;
        tmpl = other.tmpl;
        triggerMap = other.triggerMap;
    }

};

#endif

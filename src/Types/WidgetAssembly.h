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
    std::string node = "";
    std::string tmpl = "";
    bool tmpl_stored_events   = false;
    TriggerMap* triggerMap = nullptr;

    void setTriggerPrefix(Prefix prefix)
    {
        std::string triggerNode = (prefix == Prefix::widgets) ?
            node : tmpl;
        if (triggerMap != nullptr) delete triggerMap;
        triggerMap = new TriggerMap(triggers.size(), prefix, triggerNode);
    }
};

#endif

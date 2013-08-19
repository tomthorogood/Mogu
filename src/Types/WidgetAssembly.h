#ifndef WIDGETASSEMBLY_H_
#define WIDGETASSEMBLY_H_

#include <map>
#include <string>
#include <vector>
#include <Events/TriggerMap.h>
#include "NodeValue.h"

struct Widget_Assembly
{
    std::map <int,Node_Value> attrdict {};
    std::vector <std::string> children {};
    std::vector <std::string> triggers {};
    
    /* Anonymous children are those created due to 'member' templates that
     * don't actually live anywhere in the database. These will be assigned
     * names on the fly at runtime, and registered like any other widget, but
     * all interactions will be abstracted and handled by their templates.
     */
    std::vector <Widget_Assembly> anonymous_children {};
    std::string node {};
    std::string tmpl {};
    Trigger_Map* trigger_map {};

    Widget_Assembly(){}

    Widget_Assembly& operator=(const Widget_Assembly& other)
    {
        attrdict = other.attrdict;
        children = other.children;
        triggers = other.triggers;
        anonymous_children = other.anonymous_children;
        node = other.node;
        tmpl = other.tmpl;
        trigger_map = new Trigger_Map(*other.trigger_map);
        return *this;
    }

    Widget_Assembly(const Widget_Assembly& other)
    {
        attrdict = other.attrdict;
        children = other.children;
        triggers = other.triggers;
        anonymous_children = other.anonymous_children;
        node = other.node;
        tmpl = other.tmpl;
        trigger_map = new Trigger_Map(*other.trigger_map);
    }

    Widget_Assembly(Widget_Assembly&& other)
    {
        attrdict = other.attrdict;
        children = other.children;
        triggers = other.triggers;
        anonymous_children = other.anonymous_children;
        node = other.node;
        tmpl = other.tmpl;
        trigger_map = other.trigger_map;
    }

    ~Widget_Assembly()
    {
        if (trigger_map) delete trigger_map;
    }
    
    void set_trigger_prefix(Prefix p)
    {
        if (!triggers.size()) return;
        std::string trigger_node = (p==Prefix::widgets) ? node : tmpl;
        if (trigger_map) delete trigger_map;
        trigger_map = new Trigger_Map(triggers.size(), p, trigger_node);
    }


};

#endif

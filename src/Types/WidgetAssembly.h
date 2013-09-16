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
    std::vector <std::string> traits {};
    Trigger_Map trigger_map {};
    
    std::string node {};
    std::string tmpl {};

    Widget_Assembly(){}

    Widget_Assembly& operator=(const Widget_Assembly& other)
    {
        attrdict = other.attrdict;
        children = other.children;
        trigger_map = other.trigger_map;
        node = other.node;
        tmpl = other.tmpl;
        return *this;
    }

    Widget_Assembly(const Widget_Assembly& other)
        : attrdict(other.attrdict)
        , children (other.children)
        , traits (other.traits)
        , trigger_map(other.trigger_map)
        , node(other.node)
        , tmpl(other.tmpl)
    {}

    Widget_Assembly(Widget_Assembly&& other)
        : attrdict(other.attrdict)
        , children (other.children)
        , traits (other.traits)
        , trigger_map(other.trigger_map)
        , node(other.node)
        , tmpl(other.tmpl)
    {
        other.attrdict.clear();
        other.children.clear();
        other.traits.clear();
        other.node = "";
        other.tmpl = "";
        other.trigger_map.clear();
    }

    ~Widget_Assembly()
    {}
};

#endif

/*
 * MoldableAbstractParent.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "MoldableAbstractParent.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>

MoldableAbstractParent::MoldableAbstractParent
    (const std::string& node, const MoguSyntax widget_type)
: Moldable(node, widget_type)
{
    __init__();
}

void MoldableAbstractParent::__init__()
{
    Prefix children_prefix = 
        testFlag(MoldableFlags::template_children) ?
        Prefix::templates : Prefix::widgets;
    std::string children_node = 
        testFlag(MoldableFlags::template_children) ?
        __template_name : __node;
    Redis::ContextQuery db(children_prefix);
    std::string s_prefix = prefixMap.at(children_prefix);
    const char* c_prefix = s_prefix.c_str();

    // Only containers can have children, though they may not 
    // (if they are only being instantiated to hold children 
    // at a later time). 
    //
    // widgets.[id].children will hold a list of the children's identifiers.
    // llen will thus show how many default children this widget will have.
    
    db.appendQuery("llen %s.%s.children", c_prefix, children_node.c_str());
    num_children = db.yieldResponse <int>();
}

// This is called when the widget is to be rendered on screen.
void MoldableAbstractParent::load(){

#ifdef DEBUG
    std::cout << "MoldableAbstractParent::load:"
        << __LINE__ <<" " <<  __node << std::endl;
#endif

    // Do not reload unless the reload action is explicitly called.
    if (loaded() && !testFlag(MoldableFlags::allow_reload)) return;

    // First run through standard loading procedures for the parent class.
    Moldable::load();

    // Then render all children. Note that with this design, children will not
    // be instantiated until this widget is rendered, which can potentially
    // save loading times up front by delaying recursive loading. Alternatively,
    // rendering all widgets can yield faster response time at the cost of 
    // longer loading times up front.
    if (num_children > 0) 
    {
#ifdef DEBUG
        std::cout << "MoldableAbstractParent::load:"<<__LINE__
            <<" " << __node << std::endl;
#endif
        mApp;
        const MoldableFactory& factory = app->getFactory();
        
        Prefix children_prefix = 
            testFlag(MoldableFlags::template_children) ?
            Prefix::templates : Prefix::widgets;
        std::string children_node = 
            testFlag(MoldableFlags::template_children) ?
            __node : __template_name;
        Redis::ContextQuery db(children_prefix);

        // We will first populate a vector with the list of children's 
        // identifiers.
        db.appendQuery("lrange widgets.%s.children 0 %d", getNode().c_str(),
                num_children);
        
        auto children_v = db.yieldResponse <std::vector <std::string>>();
        int s_children_v = (int) children_v.size();
        assert(s_children_v == num_children);
        for (int i = 0; i < num_children; ++i)
        {
           std::string s_child = children_v[i];
#ifdef DEBUG
           std::cout << "MoldableAbstractParent::load:"<< __LINE__
               << " " << __node << ", child found: " << s_child << std::endl;
#endif
           Moldable* m_child = factory.createMoldableWidget(s_child);
#ifdef DEBUG
           std::cout << "MoldableAbstractParent::load:"<<__LINE__
               << " " << __node << ", child created: " << m_child->getNode()
               <<std::endl;
#endif
           addWidget(m_child);
#ifdef DEBUG
           std::cout << "MoldableAbstractParent::load:"<<__LINE__
               << " " << __node << ", child added: " << m_child->getNode()
               << std::endl;
#endif
        }
   }
}

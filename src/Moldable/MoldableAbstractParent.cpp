/*
 * MoldableAbstractParent.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "MoldableAbstractParent.h"
#include <Redis/ContextQuery.h>
#include <Mogu.h>

MoldableAbstractParent::MoldableAbstractParent (const std::string& node)
: Moldable(node)
{
    __init__();
}

void MoldableAbstractParent::__init__()
{
    Moldable::__init__();
    const char* c_node = __node.c_str();
    Redis::ContextQuery db(Prefix::widgets);

    // Only containers can have children, though they may not 
    // (if they are only being instantiated to hold children 
    // at a later time). 
    //
    // widgets.[id].children will hold a list of the children's identifiers.
    // llen will thus show how many default children this widget will have.
    db.appendQuery("llen widgets.%s.children", c_node);
    num_children = db.yieldResponse <int>();
}

// This is called when the widget is to be rendered on screen.
void MoldableAbstractParent::load(){

    // Do not reload unless the reload action is explicitly called.
    if (loaded() && !force_reload) return;

    // First run through standard loading procedures for the parent class.
    Moldable::load();

    // Then render all children. Note that with this design, children will not
    // be instantiated until this widget is rendered, which can potentially
    // save loading times up front by delaying recursive loading. Alternatively,
    // rendering all widgets can yield faster response time at the cost of 
    // longer loading times up front.
    if (num_children > 0) 
    {
        mApp;
        const MoldableFactory& factory = app->getFactory();
        Redis::ContextQuery db(Prefix::widgets);

        // We will first populate a vector with the list of children's 
        // identifiers.
        db.appendQuery("lrange widgets.%s.children 0 %d", getNode().c_str(),
                num_children);
        
        auto children_v = db.yieldResponse <std::vector <std::string>>();

        for (int i = 0; i < num_children; ++i)
        {
           std::string s_child = children_v[i];
           Moldable* m_child = factory.createMoldableWidget(s_child);
           addWidget(m_child);
        }
   }
}

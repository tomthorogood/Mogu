/*
 * Moldable_Abstract_Parent.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "Moldable_Abstract_Parent.h"
#include <Types/Widget_Assembly.h>
#include <Types/MoguLogger.h>
#include <Mogu.h>

Moldable_Abstract_Parent::Moldable_Abstract_Parent
    (Widget_Assembly* assembly, const Syntax_Def&  widget_type)
: Moldable(assembly, widget_type)
{
    init(assembly);
}

void Moldable_Abstract_Parent::init(Widget_Assembly* assembly)
{
    child_nodes = assembly->children;
}

// This is called when the widget is to be rendered on screen.
void Moldable_Abstract_Parent::load(){
    Application::log.log(Log_Level::NOTICE, __FILE__, ":", __LINE__,
            ": ", node);
    // Do not reload unless the reload action is explicitly called.
    if (loaded() && !test_flag(Moldable_Flags::allow_reload)) return;

    // First run through standard loading procedures for the parent class.
    Moldable::load();

    // Then render all children. Note that with this design, children will not
    // be instantiated until this widget is rendered, which can potentially
    // save loading times up front by delaying recursive loading. Alternatively,
    // rendering all widgets can yield faster response time at the cost of 
    // longer loading times up front.
    if (child_nodes.size() > 0)
    {
        Application::log.log(Log_Level::NOTICE
                ,__FILE__,"::load:",__LINE__," ", node);
        mApp;
        const Moldable_Factory & f = app->get_factory();
        for (stdd:string s : child_nodes)
        {
            Moldable* m = factor.create_moldable_widget(s);
            addWidget(m);
            Application::log.log(Log_Level::NOTICE,__FILE__,"::load:",__LINE__,
                " ", node, "child added: ", m_child->getNode());
        }
    }
}

/*
 * MoldableAbstractParent.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: tom
 */

#include "MoldableAbstractParent.h"
#include <Types/WidgetAssembly.h>
#include <Types/MoguLogger.h>
#include <Mogu.h>

MoldableAbstractParent::MoldableAbstractParent
    (WidgetAssembly* assembly, const SyntaxDef&  widget_type)
: Moldable(assembly, widget_type)
{
    init(assembly);
}

void MoldableAbstractParent::init(WidgetAssembly* assembly)
{
    child_nodes = assembly->children;
}

// This is called when the widget is to be rendered on screen.
void MoldableAbstractParent::load(){
    Application::log.log(LogLevel::NOTICE, __FILE__, ":", __LINE__,
            ": ", node);
    // Do not reload unless the reload action is explicitly called.
    if (loaded() && !testFlag(MoldableFlags::allow_reload)) return;

    // First run through standard loading procedures for the parent class.
    Moldable::load();

    // Then render all children. Note that with this design, children will not
    // be instantiated until this widget is rendered, which can potentially
    // save loading times up front by delaying recursive loading. Alternatively,
    // rendering all widgets can yield faster response time at the cost of 
    // longer loading times up front.
    if (child_nodes.size() > 0)
    {
        Application::log.log(LogLevel::NOTICE
                ,__FILE__,"::load:",__LINE__," ", node);
        mApp;
        const MoldableFactory& factory = app->getFactory();
        for (std::string child : child_nodes)
        {
            Application::log.log(LogLevel::NOTICE,__FILE__,"::load:",__LINE__,
                ", child found: ", child);
            Moldable* m_child = factory.createMoldableWidget(child);
            Application::log.log(LogLevel::NOTICE,__FILE__,"::load:", __LINE__,
                " ", node, "child created: ", m_child->getNode());               
            addWidget(m_child);
            Application::log.log(LogLevel::NOTICE,__FILE__,"::load:",__LINE__,
                " ", node, "child added: ", m_child->getNode());
        }
    }
}

/*
 * Moldable.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "Moldable.h"
#include <Mogu.h>
#include <Types/NodeValue.h>
#include <Wt/WStackedWidget>
#include <Events/EventHandler.h>
#include <Wt/WAnchor>
#include <Types/WidgetAssembly.h>
#include "../utilities.h"

namespace Application {
    extern Mogu_Logger log;
}

Moldable::Moldable (Widget_Assembly& assembly, const Syntax_Def& t )
    :sig_style_changed(this)
    ,sig_failed_test(this)
    ,sig_succeeded_test(this)
    ,sig_loaded(this)
    ,sig_hidden_changed(this)
    ,sig_index_changed(this)
    ,sig_error_reported(this)
    ,widget_type(t)
    ,node(assembly.node)
{
    static size_t num_constructed {};
    Application::log.log(Log_Level::notice, __FILE__, "::constructor:",__LINE__,
        "(", num_constructed++, "): ",node);
    init(assembly);
}

Moldable::~Moldable()
{
    mApp; 
    if (!test_flag(Moldable_Flags::shun))
        app->deregister_widget(node);
    if (bindery) delete bindery;
}

void Moldable::init (Widget_Assembly& assembly)
{
    mApp;
    app->register_widget(node, this);
    setObjectName(node); // used mostly for selenium

    assembly_style = assembly.attrdict[Mogu_Syntax::style.integer].get_string();
    assembly_tooltip = assembly.attrdict[Mogu_Syntax::tooltip.integer].get_string();
    initialize_global_attributes();
    if (assembly.trigger_map.size() > 0)
        bindery = new Event_Handler(*this, assembly.trigger_map);
}

void Moldable::initialize_global_attributes()
{
    if (!assembly_style.empty())
    {
        Node_Value n {assembly_style};
        set_attribute(Mogu_Syntax::style,n);
    }

    if (!assembly_tooltip.empty())
    {
        Node_Value n(assembly_tooltip);
        Wt::WString w {n.get_string()};
        setToolTip(w,Wt::XHTMLText);
    }
}

void Moldable::get_attribute(const Syntax_Def& state, Node_Value& val)
{

    switch ((int)state) {
        case (int) Mogu_Syntax::index: {
            Wt::WStackedWidget* stack {(Wt::WStackedWidget*) widget(0)};
            int n = stack->currentIndex();
            val.set_int(n);

            break;
        }
        case (int) Mogu_Syntax::hidden: {
            bool v = isHidden();
            val.set_int((int) v);
            break;
        }
        case (int) Mogu_Syntax::text: {
            val.set_string("\""+get_moldable_value()+"\"");
            break;
        }
        case (int) Mogu_Syntax::style: {
            val.set_string("\""+styleClass().toUTF8()+"\"");
            break;
        }
        //!\TODO test/verify
        case (int) Mogu_Syntax::source: {
            Wt::WAnchor* anchor {(Wt::WAnchor*) widget(0)};
            std::string attrval("href");
            val.set_string(anchor->attributeValue(attrval).toUTF8());
            break;
        }

        //!\TODO
        case (int) Mogu_Syntax::location: {
           break;
        }
        default:
            val.set_int(0);
            break;
    }
}

bool Moldable::update_stack_index (size_t index)
{
    // If this is attempted on a widget without a stack, do nothing.
    if (widget_type != Mogu_Syntax::stack) return false;
    Wt::WStackedWidget* stack {static_cast<Wt::WStackedWidget*>(widget(0))};
    size_t max_index = stack->children().size() -1;
    Moldable* cur {static_cast<Moldable*>(stack->currentWidget())};

    // Ensure the index exists.
    if (index > max_index) return false;


    stack->setCurrentIndex(index);
    Moldable* new_ {static_cast<Moldable*>(stack->currentWidget())};
    cur->hiddenChanged().emit();
    new_->hiddenChanged().emit();
    indexChanged().emit();
    return true;
}

bool Moldable::set_attribute(const Syntax_Def& state, Node_Value& val)
{
    if (val.is_string()) 
        val.set_string(stripquotes(val.get_string()));

    switch(state) {
        case Mogu_Syntax::index: {
            update_stack_index(val.get_int());
            break;
        }
        case Mogu_Syntax::hidden: {
            setHidden((bool) val.get_int());
            sig_hidden_changed.emit();
            break;
        }
        case Mogu_Syntax::text: {
            set_moldable_value(val.get_string());
            break;
        }
        case Mogu_Syntax::style: {
            setStyleClass(val.get_string());
        }

        //!\TODO
        case Mogu_Syntax::source: {
            break;
        }
        //!\TODO
        case Mogu_Syntax::location: {
            break;
        }

        default: return false;
    }

    return true;
}

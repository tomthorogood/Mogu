/*
 * Moldable.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "Moldable.h"
#include <Mogu.h>
#include <Types/Node_Value.h>
#include <Wt/WStackedWidget>
#include <Events/EventHandler.h>
#include <Wt/WAnchor>
#include <Types/Widget_Assembly.h>

Moldable::Moldable (Widget_Assembly* assembly, const Syntax_Def& t )
    :sig_style_changed(this)
    ,sig_failed_test(this)
    ,sig_succeeded_test(this)
    ,sig_loaded(this)
    ,sig_hidden_changed(this)
    ,sig_index_changed(this)
    ,sig_error_reported(this)
    ,widget_type(t)
    ,node(assembly->node)
{
    static size_t num_constructed {};
    Application::log.log(LogLevel::NOTICE, __FILE__, "::constructor:",__LINE__,
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

void Moldable::init (Widget_Assembly* assembly)
{
    mApp;
    app->register_widget(node, this);
    setObjectName(node); // used mostly for selenium

    assembly_style = assembly->attrdict[Mogu_Syntax::style.integer];
    assembly_tooltip = assembly->attrdict[Mogu_Syntax::tooltip.integer];
    initialize_global_attributes();
    if (assembly->triggers.size() > 0)
        bindery = new EventHandler(*this, *(assembly->triggerMap));
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
        Node_Value n {assembly_tooltip};
        set_attribute(MoguSyntax::tooltip,n);
    }
}

void Moldable::get_attribute(const Syntax_Def& state, Node_Value& val)
{

    switch ((int)state) {
        case (int) MoguSyntax::index: {
            Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget(0);
            int n = stack->currentIndex();
            val.setInt(n);

            break;
        }
        case (int) MoguSyntax::hidden: {
            bool v = isHidden();
            val.setInt((int) v);
            break;
        }
        case (int) MoguSyntax::text: {
            val.setString("\""+moldableValue()+"\"");
            break;
        }
        case (int) MoguSyntax::style: {
            val.setString("\""+styleClass().toUTF8()+"\"");
            break;
        }
        //!\TODO test/verify
        case (int) MoguSyntax::source: {
            Wt::WAnchor* anchor = (Wt::WAnchor*) widget(0);
            std::string attrval("href");
            val.setString(anchor->attributeValue(attrval).toUTF8());
            break;
        }

        //!\TODO
        case (int) MoguSyntax::location: {
           break;
        }
        default:
            val.setInt(0);
            break;
    }
}

bool Moldable::updateStackIndex (size_t index)
{
    // If this is attempted on a widget without a stack, do nothing.
    if (widget_type != MoguSyntax::stack) return false;
    Wt::WStackedWidget* stack = static_cast<Wt::WStackedWidget*>( widget(0) );
    size_t max_index = stack->children().size() -1;
    Moldable* cur = (Moldable*) stack->currentWidget();

    // Ensure the index exists.
    if (index > max_index) return false;


    stack->setCurrentIndex(index);
    Moldable* new_ = (Moldable*) stack->currentWidget();
    cur->hiddenChanged().emit();
    new_->hiddenChanged().emit();
    return true;
}

bool Moldable::set_attribute(const Syntax_Def& state, Node_Value& val)
{
    if (val.is_string()) 
        val.set_string(stripquotes(val.get_string()));

    switch(state) {
        case MoguSyntax::index: {
            update_stack_index(val.get_int());
            break;
        }
        case MoguSyntax::hidden: {
            setHidden((bool) val.get_int());
            sig_hidden_changed.emit();
            break;
        }
        case MoguSyntax::text: {
            set_moldable_value(val.get_string());
            break;
        }
        case MoguSyntax::style: {
            setStyleClass(val.get_string());
        }

        //!\TODO
        case MoguSyntax::source: {
            break;
        }
        //!\TODO
        case MoguSyntax::location: {
            break;
        }

        default: return false;
    }

    return true;
}

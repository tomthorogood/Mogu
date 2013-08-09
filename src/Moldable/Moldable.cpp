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

Moldable::Moldable (WidgetAssembly* assembly, const SyntaxDef& widget_type_)
    :sig_style_changed(this)
    ,sig_failed_test(this)
    ,sig_succeeded_test(this)
    ,sig_loaded(this)
    ,sig_hidden_changed(this)
    ,sig_index_changed(this)
    ,sig_error_reported(this)
    ,widget_type(widget_type_)
    ,node(assembly->node)
{
    static size_t num_constructed = 0;
    Application::log.log(LogLevel::NOTICE, __FILE__, "::constructor:",__LINE__,
        "(", num_constructed++, "): ",node);
    init(assembly);
}

Moldable::~Moldable()
{
    mApp; 
    if (!testFlag(MoldableFlags::shun))
        app->deregisterWidget(node);
    if (bindery != nullptr) delete bindery;
}

void Moldable::init (WidgetAssembly* assembly)
{
    mApp;
    app->registerWidget(node, *this);
    setObjectName(node);
    assembly_style = (std::string)
        assembly->attrdict[MoguSyntax::style.integer];
    assembly_tooltip = (std::string)
        assembly->attrdict[MoguSyntax::tooltip.integer];
    initializeGlobalAttributes();
    if (assembly->triggers.size() > 0)
        bindery = new EventHandler(*this, *(assembly->triggerMap));
}

void Moldable::initializeGlobalAttributes()
{
    if (!assembly_style.empty())
    {
        NodeValue nv_style(assembly_style);
        setAttribute(MoguSyntax::style,nv_style);
    }

    if (!assembly_tooltip.empty())
    {
        NodeValue nv_tooltip(assembly_tooltip);
        Wt::WString w {nv_tooltip.getString()};
        setToolTip(w,Wt::XHTMLText);
    }
}

void Moldable::getAttribute(const SyntaxDef& state, NodeValue& val)
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

bool Moldable::setAttribute(const SyntaxDef& state, NodeValue& val)
{
    if (val.isString()) {
        val.setString(stripquotes(val.getString()));
    }
    switch(state) {
        case MoguSyntax::index: {
            updateStackIndex(val.getInt());
            break;
        }
        case MoguSyntax::hidden: {
            setHidden((bool) val.getInt());
            sig_hidden_changed.emit();
            break;
        }
        case MoguSyntax::text: {
            setMoldableValue(val.getString());
            break;
        }
        case MoguSyntax::style: {
            setStyleClass(val.getString());
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

    return true; //!<\TODO
}

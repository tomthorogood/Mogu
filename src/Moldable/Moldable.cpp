/*
 * Moldable.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "Moldable.h"
#include <Mogu.h>
#include <Types/NodeValue.h>
#include <Redis/ContextQuery.h>
#include <Wt/WStackedWidget>
#include <Events/EventHandler.h>
#include <Wt/WAnchor>
#include <Types/WidgetAssembly.h>

Moldable::Moldable (WidgetAssembly* assembly, const SyntaxDef& widget_type) :
    __style_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
    ,__loaded(this)
    ,__hidden_changed(this)
    ,__index_changed(this)
    ,__widget_type(widget_type)
    ,__node(assembly->node)
{
#ifdef DEBUG
    static size_t num_constructed = 0;
    std::cout << "Moldable Constructor("<< ++num_constructed <<
        "): " << __node << std::endl;
#endif
    __init__(assembly);
}

Moldable::~Moldable()
{
    mApp; 
    if (!testFlag(MoldableFlags::shun))
        app->deregisterWidget(__node);
    if (__bindery != nullptr) delete __bindery;
}

void Moldable::__init__ (WidgetAssembly* assembly)
{
    mApp;
    app->registerWidget(__node, *this);
    __assembly_style = (std::string)
        assembly->attrdict[MoguSyntax::style.integer];
    __assembly_tooltip = (std::string)
        assembly->attrdict[MoguSyntax::tooltip.integer];
    initializeGlobalAttributes();
    if (assembly->triggers.size() > 0)
        __bindery = new EventHandler(*this, *(assembly->triggerMap));
}

void Moldable::initializeGlobalAttributes()
{
    mApp;
    Parsers::NodeValueParser& nvp = app->interpreter();
    if (!__assembly_style.empty())
    {
        NodeValue nv_style;
        nvp.giveInput(__assembly_style,nv_style);
        std::string style = stripquotes(nv_style.getString());
        setAttribute(MoguSyntax::style,style);
    }

    if (!__assembly_style.empty())
    {
        NodeValue nv_tooltip;
        nvp.giveInput(__assembly_tooltip,nv_tooltip);
        std::string tooltip = stripquotes(nv_tooltip.getString());
        setAttribute(MoguSyntax::tooltip,tooltip);
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
    if (__widget_type != MoguSyntax::stack) return false;
    Wt::WStackedWidget* stack = static_cast<Wt::WStackedWidget*>( widget(0) );
    size_t max_index = stack->children().size() -1;

    // Ensure the index exists.
    if (index > max_index) return false;

    stack->setCurrentIndex(index);
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
            __hidden_changed.emit();
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

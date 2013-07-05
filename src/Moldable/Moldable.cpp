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
#include <Redis/NodeMerger.h>

Moldable::Moldable (const std::string& node, const MoguSyntax widget_type)
:
    __style_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
    ,__loaded(this)
    ,__hidden_changed(this)
    ,__index_changed(this)
    ,__widget_type(widget_type)
    ,__node(node)
{
#ifdef DEBUG
    static size_t num_constructed = 0;
    std::cout << "Moldable Constructor("<< ++num_constructed <<
        "): " << __node << std::endl;
#endif
    __init__();
}

Moldable::~Moldable()
{
    mApp; 
    if (!testFlag(MoldableFlags::shun))
        app->deregisterWidget(__node);
    if (__bindery != nullptr) delete __bindery;
}

void Moldable::__init__ ()
{
    mApp; // Access application instance

    // Add widget to registry
    app->registerWidget(__node,*this);

    // Access application instance interpreter
    Parsers::NodeValueParser& nvp = app->interpreter();

    std::string param;
    NodeValue v;
    Redis::NodeEditor node(Prefix::widgets, __node);
    setFlags(node);

    // Any widget type can have style or tooltip declarations
    param = getParameter(node, MoguSyntax::style);
    if (param != EMPTY) {
        // input will either be a string literal or a resolvable Mogu command
        // pointing to a string literal (eventually)
        nvp.giveInput(param,v);
        setAttribute(MoguSyntax::style, v);
    }

    param = getParameter(node, MoguSyntax::tooltip);
    if (param != EMPTY)
    {
        // input will either be a string literal or a resolvable Mogu command
        // pointing to a string literal (eventually). This will possibly
        // have valid HTML markup embedded, which is allowed as long as it's
        // well formed.
        nvp.giveInput(param,v);
        setAttribute(MoguSyntax::tooltip, v);
    }

    // And any widget can have events. We only see if they exist now; we
    // do not do any event handling at this time.
    //
    // The widgets.[id].events key will be a list of the event triggers.
    // Therefore, the llen query will show how many triggers we have for
    // this widget.
   
    Prefix event_prefix =
        testFlag(MoldableFlags::template_events) ? 
        Prefix::templates : Prefix::widgets;
    std::string event_node =
        testFlag(MoldableFlags::template_events) ?
        __template_name :  __node;
    std::string s_prefix = prefixMap.at(event_prefix);
    Redis::ContextQuery events(event_prefix);
    events.appendQuery(
        "llen %s.%s.events", s_prefix.c_str(), event_node.c_str());
    num_triggers = (size_t) events.yieldResponse <int>();

    if (num_triggers > 0)
        __bindery = new EventHandler(*this, event_prefix, event_node);
}

void Moldable::setFlags(Redis::NodeEditor& node)
{
    const char* c_node = __node.c_str();
    std::string tpl = getParameter(node, MoguSyntax::template_);
    Redis::ContextQuery& db = node.getContext();

    if (tpl != EMPTY)
        __flags |= (uint8_t) MoldableFlags::is_templated;

    /* Set flags whether or not this widget has children or events */
    db.appendQuery("exists widgets.%s.events", c_node);
    db.appendQuery("exists widgets.%s.children", c_node);
    if (db.yieldResponse <bool>())
        setFlag(MoldableFlags::has_events);
    if (db.yieldResponse <bool>())
        setFlag(MoldableFlags::has_children);

    /* If there's no template, we're done. */
    if (tpl == EMPTY) return;

    __template_name = tpl;
   
    Redis::ContextQuery tpldb(Prefix::templates);

    /* If the children and event flags aren't already set, check to see
     * whether the template has those.
     */
    if (! (__flags & (uint8_t) MoldableFlags::has_events) )
    {
        tpldb.appendQuery("exists templates.%s.events", tpl.c_str());
        if (tpldb.yieldResponse <bool>())
            setFlag(MoldableFlags::template_events);
    }
    if (! (__flags & (uint8_t) MoldableFlags::has_children))
    {
        tpldb.appendQuery("exists templates.%s.children", tpl.c_str());
        if (tpldb.yieldResponse <bool>())
            setFlag(MoldableFlags::template_children);
    }

    /* Check to see whether the template has already been cached.*/
    Redis::ContextQuery temp(Prefix::temp);
    temp.appendQuery("exists temp.%s", __node.c_str());

    if (temp.yieldResponse <bool>()) 
    {
        setFlag(MoldableFlags::is_cached);
        return;
    }


    Redis::NodeMerger merger(__node);
    merger.addPrefix(Prefix::widgets, __node, 1);
    merger.addPrefix(Prefix::templates, __template_name, 2);
    std::map <std::string, std::string> iomap = merger.merge();
    merger.writeTemporary(iomap);
    node.setPrefix(Prefix::temp);
    setFlag(MoldableFlags::is_cached);
}

std::string Moldable::getParameter(Redis::NodeEditor& node, MoguSyntax param)
{
    NodeValue nv(std::to_string((int) param));
    node.setArg(&nv);
    return node.read();
}

void Moldable::getAttribute(MoguSyntax state, NodeValue& val)
{

    switch (state) {
        case MoguSyntax::index: {
            Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget(0);
            int n = stack->currentIndex();
            val.setInt(n);
            break;
        }
        case MoguSyntax::hidden: {
            bool v = isHidden();
            val.setInt((int) v);
            break;
        }
        case MoguSyntax::text: {
            val.setString(moldableValue());
            break;
        }
        case MoguSyntax::style: {
            val.setString(styleClass().toUTF8());
            break;
        }
        //!\TODO test/verify
        case MoguSyntax::source: {
            Wt::WAnchor* anchor = (Wt::WAnchor*) widget(0);
            std::string attrval("href");
            val.setString(anchor->attributeValue(attrval).toUTF8());
            break;
        }

        //!\TODO
        case MoguSyntax::location: {
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

bool Moldable::setAttribute(const MoguSyntax state, NodeValue& val)
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

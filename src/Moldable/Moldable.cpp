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

Moldable::Moldable (const std::string& node, const MoguSyntax widget_type)
:
    __style_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
    ,__loaded(this)
    ,__hidden_changed(this)
    ,__index_changed(this)
    ,__node(node)
    ,__widget_type(widget_type)
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
    if (!shunned)
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
    Redis::ContextQuery db(Prefix::widgets);

    // Any widget type can have style or tooltip declarations
    param = getParameter(db, MoguSyntax::style);
    if (param != EMPTY) {
        // input will either be a string literal or a resolvable Mogu command
        // pointing to a string literal (eventually)
        nvp.giveInput(param,v);
        setAttribute(MoguSyntax::style, v);
    }

    param = getParameter(db, MoguSyntax::tooltip);
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
    db.appendQuery("llen widgets.%s.events", __node.c_str());
    num_triggers = (size_t) db.yieldResponse <int>();
    if (num_triggers > 0)
        __bindery = new EventHandler(*this);
}

void Moldable::load()
{
    if (loaded() && !force_reload) return;
    Wt::WContainerWidget::load();
}

void Moldable::setPrefix(Redis::ContextQuery& db)
{
    getParameter(db, MoguSyntax::template_);
    std::string tpl = db.yieldResponse <std::string>();
    if (tpl == EMPTY) return;
    Redis::NodeMerger merger(node_name);
    std::map <std::string, std::string> iomap;
    merger.addPrefix(Prefix::widgets, 1);
    merger.addPrefix(Prefix::template_, 2);
    merger.merge(iomap);
    merger.writeTemporary(iomap);
    db.setPrefix(Prefix::tmp);
}

std::string Moldable::getParameter(Redis::ContextQuery& db, MoguSyntax param)
{
    db.appendQuery( "hget widgets.%s %d",__node.c_str(), (int) param);
    db.clear();
    return db.yieldResponse <std::string>();
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

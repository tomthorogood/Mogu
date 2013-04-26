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



Moldable::Moldable (const std::string& node)
:
    __style_changed(this)
    ,__failed_test(this)
    ,__succeeded_test(this)
    ,__loaded(this)
    ,__hidden_changed(this)
    ,__index_changed(this)
    ,__node(node)
{
    __init__();
}

Moldable::~Moldable()
{
    if (__bindery != nullptr) delete __bindery;
}

void Moldable::__init__ ()
{
    mApp;
    app->registerWidget(__node,this);
    Parsers::NodeValueParser& nvp = app->interpreter();
    std::string param;
    NodeValue v;
    Redis::ContextQuery db(Prefix::widgets);

    // Any widget type can have style or tooltip declarations
    param = getParameter(db, MoguSyntax::style);
    if (param != EMPTY) {
        nvp.giveInput(param,v);
        setStyleClass(v.getString());
    }

    param = getParameter(db, MoguSyntax::tooltip);
    if (param != EMPTY)
    {
        nvp.giveInput(param,v);
        setToolTip(v.getString());
    }

    // And any widget can have events. We only see if they exist now; we
    // do not do any event handling at this time.
    CreateQuery(db,
        new Redis::Query("llen widgets.%s.events", __node.c_str()));
    num_triggers = (size_t) db.yieldResponse <int>();
}

void Moldable::load()
{
    if (loaded() && !force_reload) return;
    Wt::WContainerWidget::load();
}

std::string Moldable::getParameter(Redis::ContextQuery& db, MoguSyntax param)
{
    CreateQuery(db,
        new Redis::Query("hget widgets.%s %d",__node.c_str(), (int) param));
    return db.yieldResponse <std::string>();
}

void Moldable::getAttribute(MoguSyntax state, NodeValue& val)
{

    switch (state) {
    case MoguSyntax::children: {
        int n = children().size();
        val.setInt(n);
        break;
    }
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
    default:
        val.setInt(0);
        break;
    }

    //!\TODO test/verify
    case MoguSyntax::source: {
        Wt::WAnchor* anchor = (Wt::WAnchor*) widget(0);
        val.setString((std::string)anchor->attributeValue("href"));
        break;
    }

    //!\TODO
    case MoguSyntax::location: {
       break;
    }
}

bool Moldable::setAttribute(const MoguSyntax state, const NodeValue& val)
{
    switch(state) {
        case MoguSyntax::index: {
            Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget(0);
            stack->setCurrentIndex(val.getInt());
            __index_changed.emit();
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

    }

    return true; //!<\TODO
}

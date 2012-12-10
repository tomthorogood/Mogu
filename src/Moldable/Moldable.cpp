/*
 * Moldable.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "Moldable.h"
#include <Mogu.h>
#include <Events/Bindery.h>
#include <Types/NodeValue.h>


Moldable::Moldable (const std::string& node)
: __node(node)
,__style_changed(this)
,__failed_test(this)
,__succeeded_test(this)
,__loaded(this)
,__hidden_changed(this)
{
    __init__();
}

void Moldable::__init__ ()
{
    mApp;
    __bindery = NULL;
    std::string param;

    param = getParameter("name");
    if (param != EMPTY) app->registerWidget(param, this);

    param = getParameter("class");
    if (param != EMPTY) setStyleClass(param);

    param = getParameter("tooltip");
    if (param != EMPTY) setToolTip(param);

    std::string events = __node.addPrefix("widgets") + ".events";
    app->redisExec(Mogu::Keep, "exists %s", events.c_str());
    if (redisReply_TRUE) __bindery = new Events::EventBindery(this);
}

std::string Moldable::getParameter(const std::string& param)
{
    mApp;
    std::string dbnode = __node.addPrefix("widgets");
    app->redisExec(Mogu::Keep, "hexists %s %s", dbnode.c_str(), param.c_str());
    if (redisReply_TRUE)
    {
        app->redisExec(Mogu::Keep, "hget %s %s", dbnode.c_str(), param.c_str());
        return redisReply_STRING;
    }
    else
    {
        app->redisExec(Mogu::Keep, "hexists %s template", dbnode.c_str());
        if (redisReply_TRUE)
        {
            app->redisExec(Mogu::Keep, "hget %s template", dbnode.c_str());
            MoguNode tpl(redisReply_STRING);
            std::string template_ = tpl.addPrefix("templates");
            app->redisExec(Mogu::Keep, "hexists %s %s"
                , template_.c_str(), param.c_str());
            if (redisReply_TRUE)
            {
                app->redisExec(Mogu::Keep, "hget %s %s"
                    , template_.c_str(), param.c_str());
                return redisReply_STRING;
            }
        }
    }
    return EMPTY;
}

void Moldable::getState(Enums::WidgetTypes::States state, NodeValue& val)
{
    using namespace Enums::WidgetTypes;
    switch (state) {
    case num_children: {
        int n = children().size();
        val.setInt(n);
        break;
    }
    case current_index: {
        Wt::WStackedWidget* stack = (Wt::WStackedWidget*) widget(0);
        int n = stack->currentIndex();
        val.setInt(n);
        break;
    }
    case is_hidden: {
        bool v = isHidden();
        val.setInt((int) v);
        break;
    }
    case value: {
        val.setString(moldableValue());
        break;
    }
    case currentstyle: {
        val.setString(styleClass().toUTF8());
        break;
    }
    default:
        val.setInt(0);
        break;
    }
}

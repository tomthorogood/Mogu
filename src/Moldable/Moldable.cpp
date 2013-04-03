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
#include <Wt/WStackedWidget>


TemplateNodeReply::TemplateNodeReply(const char* widgetnode, const char* field)
{
    mApp;
    app->redisExec(Mogu::Keep, "hexists %s template", widgetnode);
    exists = redisReply_TRUE;

    if (exists)
    {
       app->redisExec(Mogu::Keep, "hget %s template", widgetnode);
       MoguNode tpl(redisReply_STRING);
       std::string template_ = tpl.addPrefix("templates");
       app->redisExec(Mogu::Keep, "hexists %s %s", template_.c_str(), field);
       exists = redisReply_TRUE;
       if (exists)
       {
           app->redisExec(Mogu::Keep, "hget %s %s", template_.c_str(), field);
           response = redisReply_STRING;

       }
       else response = EMPTY;
    }
    else response = EMPTY;
}

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
#ifdef DEBUG
    std::cout << "\nCREATING " << getNode() << std::endl;
#endif
    __init__();
}

Moldable::~Moldable()
{
    if (__bindery != nullptr) delete __bindery;
}

void Moldable::__init__ ()
{
    mApp;
    Parsers::NodeValueParser& nvp = app->interpreter();
    __bindery = nullptr;
    std::string param;
    NodeValue v;

    param = getParameter("name");
    if (param != EMPTY) app->registerWidget(param, this);

    param = getParameter("class");
    if (param != EMPTY) {
        nvp.giveInput(param,v);
        setStyleClass(v.getString());
    }

    param = getParameter("tooltip");
    if (param != EMPTY)
    {
        nvp.giveInput(param,v);
        setToolTip(v.getString());
    }

    std::string events = __node.addPrefix("widgets") + ".events";
    app->redisExec(Mogu::Keep, "exists %s", events.c_str());
    has_events = redisReply_TRUE ? true : false;
}

void Moldable::load()
{
    if (loaded() && !force_reload) return;
    Wt::WContainerWidget::load();

    if (has_events) __bindery = new Events::EventBindery(this);
}

bool Moldable::hasProperty(const std::string& property){

    mApp;
    Redis::strvector v_reply;
    std::string dbnode = __node.addPrefix("widgets") + ".properties";
    app->redisExec(Mogu::Keep, "exists %s", dbnode.c_str());
    if (redisReply_TRUE)
    {
        app->redisExec(Mogu::Keep, "sismember %s %s",
            dbnode.c_str(), property.c_str());
        return redisReply_TRUE;
    }
    else
    {
        app->redisExec(Mogu::Keep, "hexists %s template", dbnode.c_str());
        if (redisReply_TRUE)
        {
            app->redisExec(Mogu::Keep, "hget %s template", dbnode.c_str());
            MoguNode tpl(redisReply_STRING);
            std::string tplnode = tpl.addPrefix("templates");
            std::string tplprop = tplnode + ".properties";
            app->redisExec(Mogu::Keep, "exists %s", tplprop.c_str());
            if (redisReply_TRUE)
            {
                app->redisExec(Mogu::Keep, "sismember %s %s",
                    tplprop.c_str(), property.c_str());
                return redisReply_TRUE;
            }
            else return false;
        }
        else return false;
    }
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
        TemplateNodeReply reply(dbnode.c_str(), param.c_str());
        return reply.response;
    }
    return EMPTY;
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

bool Moldable::setAttribute(MoguSyntax state, NodeValue& val)
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

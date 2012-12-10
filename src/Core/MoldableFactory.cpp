/*
 * MoldableFactory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include "MoldableFactory.h"
#include <Moldable/Implementations.h>
#include <Mogu.h>

MoldableFactory::MoldableFactory ()
{

}

Moldable* MoldableFactory::createMoldableWidget(const std::string& node) const
{
    mApp;
    using namespace Enums::WidgetTypes;
    std::string s_type;
    WidgetTypes e_type;
    NodeValue v;
    app->redisExec(Mogu::Keep, "hexists %s %s", node.c_str(), "type");
    if (!redisReply_TRUE)
    {
        app->redisExec(Mogu::Keep, "hexists %s %s", node.c_str(), "template");
        if (redisReply_TRUE)
        {
            app->redisExec(Mogu::Keep, "hget %s %s", node.c_str(), "template");
            MoguNode template_(redisReply_STRING);
            std::string tmpl = template_.addPrefix("templates.");
            app->redisExec(Mogu::Keep, "hget %s %s", tmpl.c_str(), "type");
            s_type = redisReply_STRING;
        }
        s_type = "{container}";
    }
    else
    {
        app->redisExec(Mogu::Keep, "hget %s %s", node.c_str(), "type");
        s_type = redisReply_STRING;
    }
    app->interpreter().giveInput(s_type, v, NULL,
        Parsers::enum_callback <Parsers::WidgetTypeParser>);
    e_type = (WidgetTypes) v.getInt();
    switch(e_type)
    {
    case container:
        return new MoldableContainer(node);
    case stack:
        return new MoldableStack(node);
    case text:
        return new MoldableText(node);
    case Enums::WidgetTypes::link:
        return new MoldableLink(node);
    case image:
        return new MoldableImage(node);
    case image_link:
        return new MoldableImageLink(node);
    case input_text:
        return new MoldableInput(node);
    case password:
        return new MoldablePassword(node);
    }

}

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
    std::string s_type;
    Tokens::MoguTokens e_type;
    NodeValue v;
    app->redisExec(Mogu::Keep, "hexists %s %s", node.c_str(), "type");
    if (!redisReply_TRUE)
    {
        app->redisExec(Mogu::Keep, "hexists %s %s", node.c_str(), "template");
        if (redisReply_TRUE)
        {
            app->redisExec(Mogu::Keep, "hget %s %s", node.c_str(), "template");
            MoguNode template_(redisReply_STRING);
            std::string tmpl = template_.addPrefix("templates");
            app->redisExec(Mogu::Keep, "hget %s %s", tmpl.c_str(), "type");
            s_type = redisReply_STRING;
        }
        else s_type = "{container}";
    }
    else
    {
        app->redisExec(Mogu::Keep, "hget %s %s", node.c_str(), "type");
        s_type = redisReply_STRING;
    }
    app->interpreter().giveInput(s_type, v);
    e_type = (Tokens::MoguTokens) v.getInt();
    switch(e_type)
    {
    case Tokens::container:
        return new MoldableContainer(node);
    case Tokens::stack:
        return new MoldableStack(node);
    case Tokens::text:
        return new MoldableText(node);
    case Tokens::anchor:
        return new MoldableLink(node);
    case Tokens::image:
        return new MoldableImage(node);
    case Tokens::image_link:
        return new MoldableImageLink(node);
    case Tokens::input:
        return new MoldableInput(node);
    case Tokens::password:
        return new MoldablePassword(node);
    case Tokens::foreach:
        return new MoldableForEach(node);
    case Tokens::memberlist:
        return new MoldableForEachUser(node);
    default: return new MoldableContainer("");
    }
}

/*
 * MoldableFactory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include "MoldableFactory.h"
#include <Moldable/Implementations.h>
#include <Types/syntax.h>
#include <Redis/ContextQuery.h>
#include <Mogu.h>

MoldableFactory::MoldableFactory ()
{

}

Moldable* MoldableFactory::createMoldableWidget(const std::string& node) const
{
    const char* c_node = node.c_str();

    MoguSyntax widget_type;

    Redis::ContextQuery db(Prefix::widgets);
    std::shared_ptr <Redis::Query> get_type;

    get_type = std::make_shared <Redis::Query>(
        new Redis::Query("hget widgets.%s %d", c_node, MoguSyntax::type)
    );

    db.appendQuery(get_type);
    widget_type = (MoguSyntax) db.yieldResponse <int>();

    // If there was an error here, the syntax type will be 'none'
    if (widget_type == MoguSyntax::__NONE__)
    {
        std::shared_ptr <Redis::Query> get_template;
        get_template = std::make_shared <Redis::Query>(
            new Redis::Query("hget widgets.%s %d", c_node, MoguSyntax::template_)
        );
        db.appendQuery(get_template);
        widget_type = (MoguSyntax) db.yieldResponse <int>();
        if (widget_type == MoguSyntax::__NONE__)
        {
            // If we're still in error, just treat it as a container.
            widget_type = MoguSyntax::container;
        }
    }

    switch(widget_type)
    {
    case MoguSyntax::container:
        return new MoldableContainer(node);
    case MoguSyntax::stack:
        return new MoldableStack(node);
    case MoguSyntax::text:
        return new MoldableText(node);
    case MoguSyntax::anchor:
        return new MoldableLink(node);
    case MoguSyntax::image:
        return new MoldableImage(node);
    case MoguSyntax::image_link:
        return new MoldableImageLink(node);
    case MoguSyntax::input:
        return new MoldableInput(node);
    case MoguSyntax::password:
        return new MoldablePassword(node);
    default:
        return new MoldableContainer("");
    }
}

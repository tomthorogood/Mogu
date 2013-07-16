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
#ifdef DEBUG
    std::cout << "Moldable Factory: " << c_node << std::endl;
#endif
    Redis::ContextQuery db(Prefix::widgets);
    db.appendQuery("hget widgets.%s %d", c_node, MoguSyntax::type);
    SyntaxDef widget_type = MoguSyntax::get(db.yieldResponse <MoguSyntax>());

    // If there was an error here, the syntax type will be 'none'
    if (widget_type == MoguSyntax::__NONE__)
    {
        // If there is no type, we have to check to see if there is
        // a template assigned to the widget which may contain this
        // information.
        db.appendQuery(
            "hget widgets.%s %d"
            , c_node, (int) MoguSyntax::template_);
        std::string template_id = db.yieldResponse <std::string>();

        const char* tmpl_id = template_id.c_str();
        Redis::ContextQuery tmpldb(Prefix::templates);
        tmpldb.appendQuery(
            "hget templates.%s %d"
            , tmpl_id, (int) MoguSyntax::type);
        widget_type = MoguSyntax::get(tmpldb.yieldResponse <int>());

        if (widget_type == MoguSyntax::__NONE__)
        {
            // If we're still in error, just treat it as a container. It 
            // will not render properly, but Mogu can keep running.
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
